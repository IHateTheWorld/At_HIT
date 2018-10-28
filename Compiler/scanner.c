#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "words.h"
#include "global_define.h"

extern id * symbol_table[BUCKETS];
FILE *source_fp, *token_fp;
/*
stack scan_stack; //objective is to help to add the symbol_table, stores the num of {} regions
int reg_count; //record the count of the regions
*/

char _key_words[_key_num][32] = {"char", "int", "real", "bool", "for", "while", "break", "continue", "if", "else", "return"};
char _key_bools[2][32] = {"False", "True"};

//the functions relating to the symbol table
extern void init_symbol_table();
extern id* insert(char * name);
extern void destroy();
//the functions relating to the const tables
extern void init_string_table();
extern string* insert_string(char * s);
extern void destroy_strings();
extern void init_int_table();
extern in* insert_int(int v);
extern void destroy_ints();
extern void init_real_table();
extern rn* insert_real(float r);
extern void destroy_reals();
//the functions relating to the buffer
extern _byte scan_buf[half_size*2+2];
extern _scan_pos lexeme_beginning, forward;
extern void retract(int n);
extern void init_buf();
void get_half_buf(FILE * source_fp, int left_right); //0->left 1->right
char forward_a_pace(FILE * source_fp);
void copytoken(char* token);

int is_letter(char ch)
{
	if((ch>='a' && ch<='z') || (ch>='A' && ch<='Z'))
		return 1;
	return 0;
}

int is_digit(char ch)
{
	if(ch>='0' && ch<='9')
		return 1;
	return 0;
}

int is_key_bool(char* token)
{
	int i;

	for(i = 0; i < 2; ++i) //is bool?
		if(strcmp(_key_bools[i], token) == 0)
			return _bool_base+i;
	for(i = 0; i < _key_num; ++i) //is key?
	{
		if(strcmp(_key_words[i], token) == 0)
			return _key_base+i;
	}

	return -1; //is not a key
}

struct _token token_scan(char first_ch)
{
	char ch;
	char token[half_size*2+1];
	struct _token a_token;
	int tmp_key_bool, id_region;

	ch = first_ch;

	if(is_letter(ch))
	{
		ch = forward_a_pace(source_fp);
		while(is_letter(ch) || is_digit(ch))
		{
			ch = forward_a_pace(source_fp);
		}
		retract(1);
		copytoken(token);
		tmp_key_bool = is_key_bool(token);
		if(tmp_key_bool >= 0) //is not a id, is a key or bool
		{
			if(tmp_key_bool < _key_base) //is a bool
			{
				a_token.type = CONST_BOOL;
				if(tmp_key_bool-_bool_base)
					a_token._point = &a_token; //True is a pointer that point to itself
				else
					a_token._point = NULL;
			}
			else //is a key
			{
				a_token.type = tmp_key_bool;
				a_token._point = NULL;
				if(tmp_key_bool >= 29 && tmp_key_bool <= 32) //is data type
					; //to do ... may be a declare expression
			}
			return a_token;
		}
		a_token.type = ID;
		/*
		if(!get_head(&scan_stack, &id_region))
			id_region = 0; //the id in global region
		a_token._point = insert(token, id_region); //pointer point to the symbol_table
		*/
		a_token._point = insert(token); //scanner don't care the region problem, all set 0
		return a_token;
	}
	else if(is_digit(ch))
	{
		ch = forward_a_pace(source_fp);
		while(is_digit(ch)) //current is a int
		{
			ch = forward_a_pace(source_fp);
		}
		a_token.type = CONST_INT;
		if(ch == '.') //is float
		{
			ch = forward_a_pace(source_fp);
			while(is_digit(ch))
			{
				ch = forward_a_pace(source_fp);
			}
			a_token.type = CONST_REAL;
		}
		retract(1);
		copytoken(token);
		if(a_token.type == CONST_INT)
		{
			a_token._point = insert_int(atoi(token)); //string ==> int
		}
		else if(a_token.type == CONST_REAL)
		{
			a_token._point = insert_real(atof(token)); //string ==> double
		}
		return a_token;
	}
	else if(ch == '\"') //string
	{
		ch = forward_a_pace(source_fp);
		while(ch != '\"' && ch != '\n')
		{
			ch = forward_a_pace(source_fp);
		}
		if(ch == '\"')
		{
			a_token.type = CONST_STRING;
			lexeme_beginning++; //ingore the left_"
			forward--; //ignore the right_"
			copytoken(token);
			a_token._point = insert_string(token); //save to the string_table
			forward++; //point to the next word
			return a_token;
		}
		else if(ch == '\n')
			printf("COSNT_STRING meet the \\n"); // error...
	}
	else if(ch == '\'')
	{
		ch = forward_a_pace(source_fp);
		if(ch == '\'')
			printf("CONST_CHAR is empty"); // error...
		else if(ch == '\n')
			printf("CONST_CHAR meet the \\n"); // error...
		else
		{
			ch = forward_a_pace(source_fp);
			if(ch == '\'')
			{
				a_token.type = CONST_CHAR;
				lexeme_beginning++;
				forward--;
				copytoken(token);
				a_token._point = insert_string(token); //save to the string_table
				forward++;
				return a_token;
			}
			else
				printf("CONST_CHAR does't meet the right \'"); // error...
		}
	}
	else
	{
		switch(ch)
		{
			case '+':
				a_token.type = PLUS;
				a_token._point = NULL;
				return a_token;
			case '-':
				a_token.type = MINUS;
				a_token._point = NULL;
				return a_token;
			case '*':
				a_token.type = MULTI;
				a_token._point = NULL;
				return a_token;
			case '/':
				a_token.type = RDIV;
				a_token._point = NULL;
				return a_token;
			case '!':
				ch = forward_a_pace(source_fp);
				if(ch == '=')
				{
					a_token.type = NE;
					a_token._point = NULL;
					return a_token;
				}
				retract(1);
				a_token.type = NOT;
				a_token._point = NULL;
				return a_token;
			case '|':
				ch = forward_a_pace(source_fp);
				if(ch == '|')
				{
					a_token.type = OR;
					a_token._point = NULL;
					return a_token;
				}
				retract(1);
				// error...
				break;
			case '&':
				ch = forward_a_pace(source_fp);
				if(ch == '&')
				{
					a_token.type = AND;
					a_token._point = NULL;
					return a_token;
				}
				retract(1);
				// error...
				break;
			case '=':
				ch = forward_a_pace(source_fp);
				if(ch == '=')
				{
					a_token.type = EQ;
					a_token._point = NULL;
					return a_token;
				}
				retract(1);
				a_token.type = ASSIGN;
				a_token._point = NULL;
				return a_token;
			case '>':
				ch = forward_a_pace(source_fp);
				if(ch == '=')
				{
					a_token.type = GE;
					a_token._point = NULL;
					return a_token;
				}
				retract(1);
				a_token.type = GT;
				a_token._point = NULL;
				return a_token;
			case '<':
				ch = forward_a_pace(source_fp);
				if(ch == '=')
				{
					a_token.type = LE;
					a_token._point = NULL;
					return a_token;
				}
				retract(1);
				a_token.type = LT;
				a_token._point = NULL;
				return a_token;
			case ',':
				a_token.type = COMMA;
				a_token._point = NULL;
				return a_token;
			case ';':
				a_token.type = SEMICOLON;
				a_token._point = NULL;
				return a_token;
			case '(':
				a_token.type = LR_BRAC;
				a_token._point = NULL;
				return a_token;
			case ')':
				a_token.type = RR_BRAC;
				a_token._point = NULL;
				return a_token;
			case '[':
				a_token.type = LS_BRAC;
				a_token._point = NULL;
				return a_token;
			case ']':
				a_token.type = RS_BRAC;
				a_token._point = NULL;
				return a_token;
			case '{':
				a_token.type = LC_BRAC;
				a_token._point = NULL;
				/*
				reg_count++;
				push(&scan_stack, &reg_count);
				*/
				return a_token;
			case '}':
				a_token.type = RC_BRAC;
				a_token._point = NULL;
				/*
				int tmp;
				if(!pop(&scan_stack, &tmp))
					printf("{ does't match }\n");
				*/
				return a_token;
		}
	}
}

void write_token(struct _token a_token)
{
	fprintf(token_fp, "%d %p ", a_token.type, a_token._point);
	
	if(SCAN_DEBUG) //objective is to debug setting 1
	{
		if(a_token.type == ID)
			//fprintf(token_fp, "ID %s %d ", ((id *)(a_token._point))->name, ((id *)(a_token._point))->reg);
			fprintf(token_fp, "ID %s ", ((id *)(a_token._point))->name);
		else if(a_token.type == CONST_INT)
			fprintf(token_fp, "CONST_INT %d ", ((in *)(a_token._point))->val);
		else if(a_token.type == CONST_REAL)
			fprintf(token_fp, "CONST_REAL %f ", ((rn *)(a_token._point))->val);
		else if(a_token.type == CONST_CHAR)
			fprintf(token_fp, "CONST_CHAR \'%s\' ", ((string *)(a_token._point))->str);
		else if(a_token.type == CONST_STRING)
			fprintf(token_fp, "CONST_STRING \"%s\" ", ((string *)(a_token._point))->str);
	}

	fprintf(token_fp, "\n");
}

int scanner_drive(void)
{
	char first_ch;
	struct _token a_token;

	source_fp = fopen("source.ch", "rb");
	token_fp = fopen("token.t", "wb");
	init_symbol_table(); //init the symbol_table
	init_string_table(); //init the const_tables
	init_int_table();
	init_real_table();
	init_buf(); //init the buffer
	get_half_buf(source_fp, 0);
	/*
	init_stack(&scan_stack, sizeof(int));
	reg_count = 0;
	*/

	lexeme_beginning = forward = 0;

	first_ch = forward_a_pace(source_fp);
	while(first_ch==' ' || first_ch=='\t' || first_ch=='\n')
	{
		first_ch = forward_a_pace(source_fp);
	}
	while(first_ch != EOF)
	{
		lexeme_beginning = forward - 1; //the start position of new word
		a_token = token_scan(first_ch);
		write_token(a_token);
		first_ch = forward_a_pace(source_fp);
		while(first_ch==' ' || first_ch=='\t' || first_ch=='\n')
		{
			first_ch = forward_a_pace(source_fp);
		}
	}

	//destroy(); //destroy the symbol_table
	//destroy_strings(); //destroy the const_tables
	//destroy_ints();
	//destroy_reals();
	//destroy_stack(&scan_stack);
	fclose(source_fp);
	fclose(token_fp);

	return 0;
}
