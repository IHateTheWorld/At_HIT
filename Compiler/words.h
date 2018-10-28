#ifndef WORDS_DEFINE
	#define WORDS_DEFINE
	#define ID 1
	#define CONST_INT 2
	#define CONST_REAL 3
	#define CONST_CHAR 4
	#define CONST_STRING 5
	#define CONST_BOOL 6
	#define PLUS 7
	#define MINUS 8
	#define MULTI 9
	#define RDIV 10
	#define NOT 11
	#define OR 12
	#define AND 13
	#define EQ 14
	#define NE 15
	#define ASSIGN 16
	#define GT 17
	#define LT 18
	#define GE 19
	#define LE 20
	#define COMMA 21
	#define SEMICOLON 22
	#define LR_BRAC 23
	#define RR_BRAC 24
	#define LS_BRAC 25
	#define RS_BRAC 26
	#define LC_BRAC 27
	#define RC_BRAC 28
	#define CHAR 29
	#define INT 30
	#define REAL 31
	#define BOOL 32
	#define FOR 33
	#define WHILE 34
	#define BREAK 35
	#define CONTINUE 36
	#define IF 37
	#define ELSE 38
	#define RETURN 39
	#define CALL 40
	//_key_base: the start CHAR 29, _bool_base: the start CONST_BOOLEAN 6
	#define _key_num 11
	#define _key_base 29
	#define _bool_base 6
	//char _key_words[_key_num][32] = {"char", "int", "real", "bool", "for", "while", "break", "continue", "if", "else", "return"};
	//char _key_bools[2][32] = {"False", "True"};
#endif
