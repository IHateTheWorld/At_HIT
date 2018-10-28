#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "global_define.h"
#include "words.h"
//#include "stack.h" //the difinition of stack
#include "sub_funs.h" //this is a simple metheods that I can consider

//the functions relating to the buffer
extern _byte scan_buf[half_size*2+2];
extern _scan_pos lexeme_beginning, forward;
extern void retract(int n);
extern void init_buf();
void get_half_buf(FILE * source_fp, int left_right); //0->left 1->right
char forward_a_pace(FILE * source_fp);
void copytoken(char* token);
//the parts of the lr1 parse list
extern int goto_tab[max_stat][maxs];
extern int action_tab[max_stat][maxs];
extern pro prods[max_pro]; //the produtions
extern int start_create();
extern int get_p(int tors);
extern int st; //the counter of the terminal symbols
extern char syms[maxs][20];

#define stat_no int
#define lex_no int
#define val_no void* //store the address of the terminal symbol or variable or const in the symbol_table or const_table
#define offset_no int
stack stat_stack, lex_stack; //double stacks in lr
FILE * sequ_fp;

void wri_a_pro(int i)
{
	int j;

	fprintf(sequ_fp, "%d %d", i, prods[i].var);
	
	for(j = 0; j < prods[i].vl; ++j)
	{
		fprintf(sequ_fp, " %d", prods[i].pv[j]);
	}

	fprintf(sequ_fp, " ==");

	fprintf(sequ_fp, " %s->", syms[get_p(prods[i].var)]);

	for(j = 0; j < prods[i].vl; ++j)
	{
		fprintf(sequ_fp, "%s", syms[get_p(prods[i].pv[j])]);
	}
	fprintf(sequ_fp, "\n");
}

void lr1_go()
{
	FILE * to_fp = fopen("token.t", "rb");
	ic_fp = fopen("inter_code.ic", "wb");
	char ch, token_str[50];
	int ty, pa, pno, popl, i, iseng;
	void * p, * tmp, * new_tmp;
	stat_no stno, sth, nst;
	lex_no leno;

	sequ_fp = fopen("sequ.sq", "wb"); //store the sequance of the productions
	init_buf();
	init_stack(&stat_stack, sizeof(stat_no));
	init_stack(&lex_stack, sizeof(stat_no));
	init_stack(&val_stack, sizeof(val_no)); //
	init_stack(&tblptr, sizeof(sym_tab *));
	init_stack(&offset, sizeof(offset_no));
	init_stack(&tmp_offset, sizeof(offset_no));
	stno = 0; //start from the state s0
	leno = -1; //stand for the '#'
	push(&stat_stack, &stno); //init the stat_stack
	push(&lex_stack, &leno); //init the lex_stack
	init_global_tab(global_tab);
	push(&tblptr, &global_tab);
	push(&offset, &stno); //just using stno, but not stand for stno, is a temp variable
	push(&tmp_offset, &stno);
	get_half_buf(to_fp, 0);
	lexeme_beginning = forward = 0;
	ch = forward_a_pace(to_fp);
	iseng = 2;
	while(iseng > 0 || ch != EOF)
	{
		if(iseng == 2)
		{
			if(ch == EOF)
				pa = st; //stand for the '#', the reducitions finally
			else
			{
				lexeme_beginning = forward - 1;
				while(ch != EOF && ch != '\n')
				{
					ch = forward_a_pace(to_fp);
				}
				retract(1); //go back a pace
				copytoken(token_str);
				//beacause use the sscanf, so that if "token_str > '%d %p'" also right
				sscanf(token_str, "%d %p", &ty, &p); //reform the string to the data type
				lr1_DEBUG(printf(">>>>>>>>>>>>>>>>>>>>> %d %p ", ty, p);)
				if(p && lr1_DEBUG2) //get some information of the token, just to check
				{
					if(ty == ID)
						printf("ID %s ", ((id*)p)->name);
					else if(ty == CONST_INT)
						printf("CONST_INT %d ", ((in*)p)->val);
					else if(ty == CONST_REAL)
						printf("CONST_REAL %f ", ((rn*)p)->val);
					else if(ty == CONST_CHAR)
						printf("CONST_CHAR \'%s\' ", ((string*)p)->str);
					else if(ty == CONST_STRING)
						printf("CONST_STRING \"%s\" ", ((string*)p)->str);
					else
						printf("The token has some problem!\n");
				}
				lr1_DEBUG(putchar('\n');)
				retract(-1); //forward a pace, but doesn't read the char
				if(ch == '\n')
					ch = forward_a_pace(to_fp);
				pa = get_p(ty); //should translate
			}
		}
		get_head(&stat_stack, &sth); //get the current state
		if(action_tab[sth][pa] == -2)
		{
			lr1_DEBUG(printf("##################### succeeded...\n");)
			iseng = 0;
			wri_a_pro(0);
			return ;
		}
		else if(action_tab[sth][pa] == -1)
		{
			printf("error...\n");
			iseng = -1;
			return ;
		}
		else if(action_tab[sth][pa] & 1) //equal x%2==1, should be reduced
		{
			pno = action_tab[sth][pa] >> 1;
			printf("should reduce... %d\n", pno);
			//deal with the pa(th) production,run the sub_function
			new_tmp = (*sub_fun_table[pno])(NULL); //according to the array consisting of the poiters of functions
			if(prods[pno].vl == 1 && prods[pno].pv[0] == -1) //reduce the empty production,shouldn't push ε
				popl = 0;
			else
				popl = prods[pno].vl; //pop as many as the length of the production
			for(i = 0; i < popl; ++i)
			{
				pop(&stat_stack, &stno);
				pop(&lex_stack, &leno);
				pop(&val_stack, &tmp);
			}
			leno = prods[pno].var;
			get_head(&stat_stack, &stno);
			nst = goto_tab[stno][get_p(leno)-st]; //the new state after reducition
			push(&lex_stack, &leno);
			push(&stat_stack, &nst);
			lr1_DEBUG(printf("reducing %dth production: %d %d->%d \n", pno, pa, sth, nst);)
			push(&val_stack, &new_tmp); //tmp may be null
			wri_a_pro(pno); //write the production that should be reduced
			iseng = 1;
		}
		else //should move in
		{
			stno = action_tab[sth][pa] >> 1;
			push(&stat_stack, &stno);
			leno = ty;
			push(&lex_stack, &leno);
			lr1_DEBUG(printf("moving_in: %d %d->%d \n", pa, sth, stno);)
			tmp = new_struct(pa); //pa stand for the counter of the symbol
			if(tmp) //the symbol moved in could be the key
				memcpy(tmp, &p, sizeof(p)); //copy the addr to the tmp->addr
			push(&val_stack, &tmp); //push the value to the val_stack
			iseng = 2;
		}
	}

	fclose(to_fp);
	fclose(sequ_fp);
	fclose(ic_fp);
	destroy_stack(&stat_stack);
	destroy_stack(&lex_stack);
	destroy_stack(&val_stack);
	destroy_stack(&tblptr);
	destroy_stack(&offset);
	destroy_stack(&tmp_offset);
}

int lr1_drive(void)
{
	start_create(); //create the lr1 parse list
	lr1_go(); //lr1 ing...

	destroy(); //destroy the symbol_table storing the name of the symbols

	return 0;
}
