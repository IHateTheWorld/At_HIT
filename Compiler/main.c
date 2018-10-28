#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "global_define.h"

//init and declare the symbol_table and const_table as global variables
id * symbol_table[BUCKETS]; //this table just store the name of the symbols
string * string_table[STRING_BUCKETS];
in * int_table[10];
rn * real_table;
sym_tab * global_tab; //the global sym_tab
drop_vector * all_vector; //the pointer pointing to all drop_vectors

//the functions of scanner
extern int scanner_drive(void);
//the functions of lr1
extern int lr1_drive(void);

extern void init_global_tab();
extern void init_drop_vector();
extern void destroy_strings();
extern void destroy_ints();
extern void destroy_reals();
extern void destroy_all_vector();
extern void destroy_sym_tab(sym_tab * rm_tab);

int main()
{
	scanner_drive();
	init_global_tab(); //do some init before the lr1
	init_drop_vector();
	lr1_drive();

	//destroy(); //destroy the symbol_table
	destroy_strings(); //destroy the const_tables
	destroy_ints();
	destroy_reals();
	destroy_all_vector();
	destroy_sym_tab(global_tab);

	return 0; //ignore this sentance will report a error 160
}

