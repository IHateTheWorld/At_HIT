#include <stdio.h>
#include <stdlib.h>
#include "words.h"
#include "variables.h"

//join the macros to become the identifier of the struct
#define struct_(para) _struct_(para)
#define _struct_(para) struct_ ## para
#define struct_fun_(para) _struct_fun_(para)
#define _struct_fun_(para) struct_fun_ ## para
#define map_item(para) {para, struct_fun_(para)}

stack offset, tblptr, tmp_offset; //the stacks using for deviding regions
stack val_stack;
extern sym_tab * mktable(sym_tab * previous);
extern id_no * enter(sym_tab * table, char * name, int type, int offset, void * pt);
extern int addwidth(sym_tab * table, int width, int tmp_widht);
extern int enterproc(sym_tab * table, char * name, sym_tab * newtable);
extern id_no * lookup_sym_tab(char * name);
extern tmp_struct * newtemp(sym_tab * table, int type, int offset, void * pt);
extern drop_vector * new_drop_vector();
extern int insert_dim(drop_vector * vec, int len);
extern int add_dims(drop_vector * vec, int dims, int width);
extern int limit(drop_vector * vec, int m); //m start from 0
//extern string * lookup_string(char * s);
//extern in * lookup_int(int v);
//extern rn * lookup_real(float r);

FILE * ic_fp = NULL; //the file store intermediate codes

void gencode(char * ic) //generate a intermediate code
{
	fprintf(ic_fp, "%s\n", ic);
}

//define the struct of the symbol or variable that can be pushed into the stack
typedef struct
{
	void * addr;
} struct_(ID);

typedef struct
{
	void * addr;
} struct_(CONST_INT);

typedef struct
{
	void * addr;
} struct_(CONST_REAL);

typedef struct
{
	void * addr;
} struct_(CONST_CHAR);

typedef struct
{
	void * addr;
} struct_(CONST_STRING);

typedef struct
{
	void * addr;
} struct_(CONST_BOOL);

typedef struct
{
	;
} struct_(SS);

typedef struct
{
	int type;
	int width;
	void * addr;
} struct_(E);

typedef struct
{
	int type;
	int width;
	void * addr;
} struct_(T);

typedef struct
{
	int type;
	int width;
	void * addr;
} struct_(F);

typedef struct
{
	int type;
	int width; //the length of the type
} struct_(TY);

typedef struct
{
	int type;
	int width;
} struct_(D);

typedef struct
{
	;
} struct_(A);

typedef struct
{
	id_no * addr;
	void * offset; //there should be attention
} struct_(LEFT);

typedef struct
{
	int ndim;
	id_no * array;
	void * addr;
} struct_(ELIST);

typedef struct
{
	;
} struct_(P);

typedef struct
{
	;
} struct_(PA);

typedef struct
{
	;
} struct_(B);

typedef struct
{
	;
} struct_(Y);

typedef struct
{
	;
} struct_(Z);

typedef struct
{
	;
} struct_(R);

typedef struct
{
	;
} struct_(S);

typedef struct
{
	;
} struct_(L);

typedef struct
{
	;
} struct_(PL);

typedef struct
{
	;
} struct_(PO);

typedef struct
{
	;
} struct_(G);

typedef struct
{
	;
} struct_(CC);

typedef struct
{
	char * name;
	int ndim;
	int width;
	drop_vector * addr;
} struct_(CLIST);

typedef struct
{
	;
} struct_(M);

typedef struct
{
	;
} struct_(M1);

typedef struct
{
	;
} struct_(M2);

typedef struct
{
	;
} struct_(M3);

typedef struct
{
	;
} struct_(M4);

//define the new_struct function through macro, can't add remark in the macro
#define sfdef_(para) \
	void * struct_fun_(para) (void) \
	{ \
		struct_(para) * newtmp; \
		newtmp = (struct_(para) *)malloc(sizeof(struct_(para))); \
		printf("newing..................................... %s\n", #para); \
		return newtmp; \
	}
sfdef_(ID);
sfdef_(CONST_INT);
sfdef_(CONST_REAL);
sfdef_(CONST_CHAR);
sfdef_(CONST_STRING);
sfdef_(CONST_BOOL);
sfdef_(SS);
sfdef_(E);
sfdef_(T);
sfdef_(F);
sfdef_(TY);
sfdef_(D);
sfdef_(A);
sfdef_(LEFT);
sfdef_(ELIST);
sfdef_(P);
sfdef_(PA);
sfdef_(B);
sfdef_(Y);
sfdef_(Z);
sfdef_(R);
sfdef_(S);
sfdef_(L);
sfdef_(PL);
sfdef_(PO);
sfdef_(G);
sfdef_(CC);
sfdef_(CLIST);
sfdef_(M);
sfdef_(M1);
sfdef_(M2);
sfdef_(M3);
sfdef_(M4);

//the map of symbol_variable => new_struct
struct map_node
{
	int symvar_no;
	void * (*struct_fun) (void); //just the pointer of the new_struct function
};
struct map_node maps[] = 
{
	map_item(ID),
	map_item(CONST_INT),
	map_item(CONST_REAL),
	map_item(CONST_CHAR),
	map_item(CONST_STRING),
	map_item(CONST_BOOL),
	map_item(SS),
	map_item(E),
	map_item(T),
	map_item(F),
	map_item(TY),
	map_item(D),
	map_item(A),
	map_item(LEFT),
	map_item(ELIST),
	map_item(P),
	map_item(PA),
	map_item(B),
	map_item(Y),
	map_item(Z),
	map_item(R),
	map_item(S),
	map_item(L),
	map_item(PL),
	map_item(PO),
	map_item(G),
	map_item(CC),
	map_item(CLIST),
	map_item(M),
	map_item(M1),
	map_item(M2),
	map_item(M3),
	map_item(M4)
};

//the function to search and return the new struct of the symbol or variable
void * new_struct(int pa)
{
	int i;

	for(i = 0; i < (sizeof(maps)/sizeof(struct map_node)); ++i)
	{
		if(pa == maps[i].symvar_no)
		{
			return (*maps[i].struct_fun)();
		}
	}
	return NULL;
}

//join the macros to become the entrance of the sub_function
#define sub_fun_(para) _sub_fun_(para)
#define _sub_fun_(para) sub_fun_ ## para

void type_trans(void * res, void * a, void * b)
{
	struct_(E) * ta = NULL, * tb = NULL, * tr = NULL;
	ta = (struct_(E) *)a;
	tb = (struct_(E) *)b;
	tr = (struct_(E) *)res;

	if(ta->type == tb->type)
	{
		if(ta->type == INT || ta->type == REAL)
			tr->type = ta->type;
		else if(ta->type == CONST_INT)
			tr->type = INT;
		else if(ta->type == CONST_REAL)
			tr->type = REAL;
		else
			goto error;
		tr->width = ta->width;
		return ;
	}
	else if((ta->type == INT && tb->type == CONST_INT) || (ta->type == CONST_INT && tb->type == CONST_REAL))
	{
		tr->type = INT;
		tr->width = 4;
		return ;
	}
	else if((ta->type == INT && tb->type == REAL) || (ta->type == REAL && tb->type == INT) || \
			(ta->type == CONST_INT && tb->type == CONST_REAL) || \
			(ta->type == CONST_REAL && tb->type == CONST_INT) || \
			(ta->type == INT && tb->type == CONST_REAL) || \
			(ta->type == REAL && tb->type == CONST_INT) || \
			(tb->type == INT && ta->type == CONST_REAL) || \
			(tb->type == REAL && ta->type == CONST_INT) \
		   )

	{
		tr->type = REAL;
		tr->width = 8;
		return ;
	}
	error:
		printf(">>>>>>>>>>>>>>>>>>> res->type: %d res->width: %d\n", tr->type, tr->width);
		printf(">>>>>>>>>>>>>>>>>>> a->type: %d b->type: %d\n", ta->type, tb->type);
		printf("the two types should not be calculated!\n");
}

void output_format(char * code, void* newe, void * te, void * tt, char c1, char c2, char c3, char c4)
{
	struct_(E) * new_E = NULL, * e = NULL, * t = NULL;
	new_E = (struct_(E) *) newe;
	e = (struct_(E) *) te;
	t = (struct_(E) *)tt;
	if(e->type != CONST_INT && e->type != CONST_REAL && \
			t->type != CONST_INT && t->type != CONST_REAL)
		sprintf(code, "%p(%c) := %p(%c) %c %p(%c)", new_E->addr, c1, e->addr, c2, c3, t->addr, c4);
	else if(e->type == CONST_INT && t->type == CONST_INT)
		sprintf(code, "%p(%c) := %p(%d) %c %p(%d)", new_E->addr, c1, e->addr, \
				((in*)(e->addr))->val, c3, t->addr, ((in *)(t->addr))->val);
	else if(e->type == CONST_INT && t->type == CONST_REAL)
		sprintf(code, "%p(%c) := %p(%d) %c %p(%f)", new_E->addr, c1, e->addr, \
				((in*)(e->addr))->val, c3, t->addr, ((rn *)(t->addr))->val);
	else if(e->type == CONST_REAL && t->type == CONST_REAL)
		sprintf(code, "%p(%c) := %p(%f) %c %p(%f)", new_E->addr, c1, e->addr, \
				((rn*)(e->addr))->val, c3, t->addr, ((rn *)(t->addr))->val);
	else if(e->type == CONST_REAL && t->type == CONST_INT)
		sprintf(code, "%p(%c) := %p(%f) %c %p(%d)", new_E->addr, c1, e->addr, \
				((rn*)(e->addr))->val, c3, t->addr, ((in *)(t->addr))->val);
	else if(e->type == CONST_INT && t->type == INT)
		sprintf(code, "%p(%c) := %p(%d) %c %p(%c)", new_E->addr, c1, e->addr, \
				((in*)(e->addr))->val, c3, t->addr, c4);
	else if(e->type == CONST_INT && t->type == REAL)
		sprintf(code, "%p(%c) := %p(%d) %c %p(%c)", new_E->addr, c1, e->addr, \
				((in*)(e->addr))->val, c3, t->addr, c4);
	else if(e->type == CONST_REAL && t->type == INT)
		sprintf(code, "%p(%c) := %p(%f) %c %p(%c)", new_E->addr, c1, e->addr, \
				((rn*)(e->addr))->val, c3, t->addr, c4);
	else if(e->type == CONST_REAL && t->type == REAL)
		sprintf(code, "%p(%c) := %p(%f) %c %p(%c)", new_E->addr, c1, e->addr, \
				((rn*)(e->addr))->val, c3, t->addr, c4);
	else if(t->type == CONST_INT && e->type == INT)
		sprintf(code, "%p(%c) := %p(%c) %c %p(%d)", new_E->addr, c1, e->addr, \
				c2, c3, t->addr, ((in*)(t->addr))->val);
	else if(t->type == CONST_INT && e->type == REAL)
		sprintf(code, "%p(%c) := %p(%c) %c %p(%d)", new_E->addr, c1, e->addr, \
				c2, c3, t->addr, ((in*)(t->addr))->val);
	else if(t->type == CONST_REAL && e->type == INT)
		sprintf(code, "%p(%c) := %p(%c) %c %p(%f)", new_E->addr, c1, e->addr, \
				c2, c3, t->addr, ((rn*)(t->addr))->val);
	else if(t->type == CONST_REAL && e->type == REAL)
		sprintf(code, "%p(%c) := %p(%c) %c %p(%f)", new_E->addr, c1, e->addr, \
				c2, c3, t->addr, ((rn*)(t->addr))->val);
}

//define the sub_functios
void * sub_fun_(0) (void * p) //SS->CC
{
	return NULL;
}

void * sub_fun_(1) (void * p) //E->E+T
{
	printf("################################################################ 1 E->E+T...\n");
	struct_(E) * new_E = NULL, * e = NULL;
	struct_(T) * t = NULL;
	get_val(&val_stack, 0, &t);
	get_val(&val_stack, 2, &e);
	new_E = (struct_(E) *)malloc(sizeof(struct_(E)));
	if(!new_E)
		return NULL;
	
	type_trans(new_E, t, e);
	/*
	if(t->type == e->type)
	{
		new_E->type = t->type;
		new_E->width = t->width;
	}
	else if((t->type == INT && e->type == REAL) || (t->type == REAL && e->type == INT))
	{
		new_E->type = REAL;
		new_E->width = 8;
	}
	else
		printf("the two types should not be calculated!\n");
	*/

	sym_tab * top_tab;
	int top_offset;
	get_head(&tblptr, &top_tab);
	pop(&tmp_offset, &top_offset);
	printf("&&& tmp_offset: %d\n", top_offset);
	new_E->addr = newtemp(top_tab, new_E->type, top_offset, NULL);
	top_offset += new_E->width;
	push(&tmp_offset, &top_offset);

	char code[100]; //generate the intermediate code
	//sprintf(code, ">>> e->type: %d t->type: %d", e->type, t->type);
	//gencode(code);
	output_format(code, new_E, e, t, 'E', 'E', '+', 'T');
	/*
	if(e->type != CONST_INT && e->type != CONST_REAL && \
			t->type != CONST_INT && t->type != CONST_REAL)
		sprintf(code, "%p(E) := %p(E) + %p(T)", new_E->addr, e->addr, t->addr);
	else if(e->type == CONST_INT && t->type == CONST_INT)
		sprintf(code, "%p(E) := %p(%d) + %p(%d)", new_E->addr, e->addr, \
				((in*)(e->addr))->val, t->addr, ((in *)(t->addr))->val);
	else if(e->type == CONST_INT && t->type == CONST_REAL)
		sprintf(code, "%p(E) := %p(%d) + %p(%f)", new_E->addr, e->addr, \
				((in*)(e->addr))->val, t->addr, ((rn *)(t->addr))->val);
	else if(e->type == CONST_REAL && t->type == CONST_REAL)
		sprintf(code, "%p(E) := %p(%f) + %p(%f)", new_E->addr, e->addr, \
				((rn*)(e->addr))->val, t->addr, ((rn *)(t->addr))->val);
	else if(e->type == CONST_REAL && t->type == CONST_INT)
		sprintf(code, "%p(E) := %p(%f) + %p(%d)", new_E->addr, e->addr, \
				((rn*)(e->addr))->val, t->addr, ((in *)(t->addr))->val);
	else if(e->type == CONST_INT && t->type == INT)
		sprintf(code, "%p(E) := %p(%d) + %p(T)", new_E->addr, e->addr, \
				((in*)(e->addr))->val, t->addr);
	else if(e->type == CONST_INT && t->type == REAL)
		sprintf(code, "%p(E) := %p(%d) + %p(T)", new_E->addr, e->addr, \
				((in*)(e->addr))->val, t->addr);
	else if(e->type == CONST_REAL && t->type == INT)
		sprintf(code, "%p(E) := %p(%f) + %p(T)", new_E->addr, e->addr, \
				((rn*)(e->addr))->val, t->addr);
	else if(e->type == CONST_REAL && t->type == REAL)
		sprintf(code, "%p(E) := %p(%f) + %p(T)", new_E->addr, e->addr, \
				((rn*)(e->addr))->val, t->addr);
	else if(t->type == CONST_INT && e->type == INT)
		sprintf(code, "%p(E) := %p(E) + %p(%d)", new_E->addr, e->addr, \
				t->addr, ((in*)(t->addr))->val);
	else if(t->type == CONST_INT && e->type == REAL)
		sprintf(code, "%p(E) := %p(E) + %p(%d)", new_E->addr, e->addr, \
				t->addr, ((in*)(t->addr))->val);
	else if(t->type == CONST_REAL && e->type == INT)
		sprintf(code, "%p(E) := %p(E) + %p(%f)", new_E->addr, e->addr, \
				t->addr, ((rn*)(t->addr))->val);
	else if(t->type == CONST_REAL && e->type == REAL)
		sprintf(code, "%p(E) := %p(E) + %p(%f)", new_E->addr, e->addr, \
				t->addr, ((rn*)(t->addr))->val);
	*/

	//sprintf(code, "%p(E) := %p(E) + %p(T)", new_E->addr, e->addr, t->addr);
	gencode(code);

	return new_E;
}

void * sub_fun_(2) (void * p) //E->E-T
{
	printf("################################################################ 2 E->E-T...\n");
	struct_(E) * new_E = NULL, * e = NULL;
	struct_(T) * t = NULL;
	get_val(&val_stack, 0, &t);
	get_val(&val_stack, 2, &e);
	new_E = (struct_(E) *)malloc(sizeof(struct_(E)));
	if(!new_E)
		return NULL;
	
	type_trans(new_E, t, e);
	/*
	if(t->type == e->type)
	{
		new_E->type = t->type;
		new_E->width = t->width;
	}
	else if((t->type == INT && e->type == REAL) || (t->type == REAL && e->type == INT))
	{
		new_E->type = REAL;
		new_E->width = 8;
	}
	else
		printf("the two types should not be calculated!\n");
	*/

	sym_tab * top_tab;
	int top_offset;
	get_head(&tblptr, &top_tab);
	pop(&tmp_offset, &top_offset);
	printf("&&& tmp_offset: %d\n", top_offset);
	new_E->addr = newtemp(top_tab, new_E->type, top_offset, NULL);
	top_offset += new_E->width;
	push(&tmp_offset, &top_offset);

	char code[100]; //generate the intermediate code
	output_format(code, new_E, e, t, 'E', 'E', '-', 'T');
	//sprintf(code, "%p(E) := %p(E) - %p(T)", new_E->addr, e->addr, t->addr);
	gencode(code);

	return new_E;
}

void * sub_fun_(3) (void * p) //E->T
{
	printf("################################################################ 3 E->T...\n");
	struct_(T) * t = NULL;
	get_val(&val_stack, 0, &t);

	struct_(E) * new_E = NULL;
	new_E = (struct_(E) *)malloc(sizeof(struct_(E)));
	if(!new_E)
		return NULL;
	new_E->addr = t->addr;
	new_E->type = t->type;
	new_E->width = t->width;

	return new_E;
}

void * sub_fun_(4) (void * p) //T->T*F
{
	printf("################################################################ 4 T->T*F...\n");
	struct_(T) * new_T = NULL, * t = NULL;
	struct_(F) * f = NULL;
	get_val(&val_stack, 0, &f);
	get_val(&val_stack, 2, &t);
	new_T = (struct_(T) *)malloc(sizeof(struct_(T)));
	if(!new_T)
		return NULL;

	type_trans(new_T, t, f);
	/*
	if(t->type == f->type)
	{
		new_T->type = t->type;
		new_T->width = t->width;
	}
	else if((t->type == INT && f->type == REAL) || (t->type == REAL && f->type == INT))
	{
		new_T->type = REAL;
		new_T->width = 8;
	}
	else
		printf("the two types should not be calculated!\n");
	*/

	sym_tab * top_tab;
	int top_offset;
	get_head(&tblptr, &top_tab);
	pop(&tmp_offset, &top_offset);
	printf("&&& tmp_offset: %d\n", top_offset);
	new_T->addr = newtemp(top_tab, new_T->type, top_offset, NULL);
	top_offset += new_T->width;
	push(&tmp_offset, &top_offset);

	char code[100]; //generate the intermediate code
	output_format(code, new_T, t, f, 'T', 'T', '*', 'F');
	//sprintf(code, "%p(T) := %p(T) * %p(F)", new_T->addr, t->addr, f->addr);
	gencode(code);

	return new_T;
}

void * sub_fun_(5) (void * p) //T->T/F
{
	printf("################################################################ 4 T->T*F...\n");
	struct_(T) * new_T = NULL, * t = NULL;
	struct_(F) * f = NULL;
	get_val(&val_stack, 0, &f);
	get_val(&val_stack, 2, &t);
	new_T = (struct_(T) *)malloc(sizeof(struct_(T)));
	if(!new_T)
		return NULL;

	type_trans(new_T, t, f);
	/*
	if(t->type == f->type)
	{
		new_T->type = t->type;
		new_T->width = t->width;
	}
	else if((t->type == INT && f->type == REAL) || (t->type == REAL && f->type == INT))
	{
		new_T->type = REAL;
		new_T->width = 8;
	}
	else
		printf("the two types should not be calculated!\n");
	*/

	sym_tab * top_tab;
	int top_offset;
	get_head(&tblptr, &top_tab);
	pop(&tmp_offset, &top_offset);
	printf("&&& tmp_offset: %d\n", top_offset);
	new_T->addr = newtemp(top_tab, new_T->type, top_offset, NULL);
	top_offset += new_T->width;
	push(&tmp_offset, &top_offset);

	char code[100]; //generate the intermediate code
	output_format(code, new_T, t, f, 'T', 'T', '/', 'F');
	//sprintf(code, "%p(T) := %p(T) / %p(F)", new_T->addr, t->addr, f->addr);
	gencode(code);

	return new_T;
}

void * sub_fun_(6) (void * p) //T->F
{
	printf("################################################################ 6 T->F...\n");
	struct_(F) * f = NULL;
	get_val(&val_stack, 0, &f);

	struct_(T) * new_T = NULL;
	new_T = (struct_(T) *)malloc(sizeof(struct_(T)));
	if(!new_T)
		return NULL;
	new_T->type = f->type;
	new_T->width = f->width;
	new_T->addr = f->addr;

	return new_T;
}

void * sub_fun_(7) (void * p) //F->id
{
	printf("################################################################ 7 F->id...\n");
	struct_(ID) * var_id = NULL;
	get_val(&val_stack, 0, &var_id);
	if(var_id)
		printf("name=%s\n", ((id*)(var_id->addr))->name);
	else
		printf("should not...\n");
	struct_(F) * new_F = NULL;
	new_F = (struct_(F) *)malloc(sizeof(struct_(F)));
	if(!new_F)
		return NULL;
	id_no * id_pt = lookup_sym_tab(((id*)(var_id->addr))->name);
	if(!id_pt)
		printf("%s has not define!\n", ((id*)(var_id->addr))->name);
	new_F->type = id_pt->type;
	if(new_F->type == INT) //attention: there may be add something in the future
		new_F->width = 4;
	else if(new_F->type == REAL)
		new_F->width = 8;
	else
		printf("the type of %s can't be given to F!\n", ((id*)(var_id->addr))->name);
	new_F->addr = id_pt;

	return new_F;
}

void * sub_fun_(8) (void * p) //F->const_int
{
	printf("############################################################# 8 F->const_int...\n");
	struct_(CONST_INT) * c_in = NULL;
	get_val(&val_stack, 0, &c_in);

	struct_(F) * new_F;
	new_F = (struct_(F) *)malloc(sizeof(struct_(F)));
	if(!new_F)
		return NULL;
	new_F->type = CONST_INT;
	new_F->width = 4;
	new_F->addr = c_in->addr;
	
	return new_F;
}

void * sub_fun_(9) (void * p) //F->const_real
{
	printf("################################################################ 9 F->const_real...\n");
	struct_(CONST_REAL) * c_rl = NULL;
	get_val(&val_stack, 0, &c_rl);

	struct_(F) * new_F;
	new_F = (struct_(F) *)malloc(sizeof(struct_(F)));
	if(!new_F)
		return NULL;
	new_F->type = CONST_REAL;
	new_F->width = 8;
	new_F->addr = c_rl->addr;
	
	return new_F;
}

void * sub_fun_(10) (void * p) //F->(E)
{
	printf("#####EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE################################# 10 F->(E)...\n");
	struct_(E) * e = NULL;
	get_val(&val_stack, 0, &e);

	struct_(F) * new_F;
	new_F = (struct_(F) *)malloc(sizeof(struct_(F)));
	if(!new_F)
		return NULL;
	new_F->type = e->type;
	new_F->width = e->width;
	new_F->addr = e->addr;

	return new_F;
}

void * sub_fun_(11) (void * p) //TY->char
{
	printf("###################################################################### 30 TY->char...\n");
	struct_(TY) * new_TY = NULL;
	new_TY = (struct_(TY) *)malloc(sizeof(struct_(TY)));
	if(!new_TY)
		return NULL;
	new_TY->type = CHAR;
	new_TY->width = 1;
	return new_TY;
}

void * sub_fun_(12) (void * p) //TY->int
{
	printf("####################################################################### 31 TY->int...\n");
	struct_(TY) * new_TY = NULL;
	new_TY = (struct_(TY) *)malloc(sizeof(struct_(TY)));
	if(!new_TY)
	{
		printf("newing TY has error...\n");
		return NULL;
	}
	new_TY->type = INT;
	new_TY->width = 4;
	return new_TY;
}

void * sub_fun_(13) (void * p) //TY->real
{
	printf("####################################################################### 32 TY->real...\n");
	struct_(TY) * new_TY = NULL;
	new_TY = (struct_(TY) *)malloc(sizeof(struct_(TY)));
	if(!new_TY)
		return NULL;
	new_TY->type = REAL;
	new_TY->width = 8;
	return new_TY;
}

void * sub_fun_(14) (void * p) //TY->bool
{
	printf("###################################################################### 33 TY->bool...\n");
	struct_(TY) * new_TY = NULL;
	new_TY = (struct_(TY) *)malloc(sizeof(struct_(TY)));
	if(!new_TY)
		return NULL;
	new_TY->type = BOOL;
	new_TY->width = 1;
	return new_TY;
}

void * sub_fun_(15) (void * p) //D->TY id
{
	printf("######################################################################## 24 D->TY id\n");
	struct_(ID) * var_id = NULL;
	struct_(TY) * ty = NULL;
	get_val(&val_stack, 0, &var_id);
	if(var_id)
		printf("name=%s\n", ((id*)(var_id->addr))->name);
	else
		printf("should not...\n");
	get_val(&val_stack, 1, &ty);
	if(ty)
		printf("type: %d  width: %d\n", ty->type, ty->width);
	else
		printf("should not...\n");

	sym_tab * top_tab;
	int top_offset;
	get_head(&tblptr, &top_tab);
	pop(&offset, &top_offset);
	printf("&&&& offset: %d\n", top_offset);
	enter(top_tab, ((id*)(var_id->addr))->name, ty->type, top_offset, NULL);
	top_offset += ty->width;
	push(&offset, &top_offset);

	struct_(D) * new_D = NULL;
	new_D = (struct_(D) *)malloc(sizeof(struct_(D)));
	if(!new_D)
		return NULL;
	new_D->type = ty->type;
	new_D->width = ty->width;

	return new_D;
}

void * sub_fun_(16) (void * p) //D->TY id=E
{
	printf("##################################################################### 25 D->TY id=E\n");
	struct_(ID) * var_id = NULL;
	struct_(TY) * ty = NULL;
	id_no * new_id = NULL;
	get_val(&val_stack, 2, &var_id);
	if(var_id)
		printf("name=%s\n", ((id*)(var_id->addr))->name);
	else
		printf("should not...\n");
	get_val(&val_stack, 3, &ty);
	if(ty)
		printf("type: %d  width: %d\n", ty->type, ty->width);
	else
		printf("should not...\n");

	sym_tab * top_tab;
	int top_offset;
	get_head(&tblptr, &top_tab);
	pop(&offset, &top_offset);
	printf("&&& offset: %d\n", top_offset);
	new_id = enter(top_tab, ((id*)(var_id->addr))->name, ty->type, top_offset, NULL);
	top_offset += ty->width;
	push(&offset, &top_offset);

	struct_(E) * e = NULL;
	get_val(&val_stack, 0, &e); //store the address of temp variable

	char code[100];
	if(e->type == CONST_INT)
		sprintf(code, "%p(%s) := %p(%d)", new_id, new_id->name, e->addr, ((in*)(e->addr))->val);
	else if(e->type == CONST_REAL)
		sprintf(code, "%p(%s) := %p(%f)", new_id, new_id->name, e->addr, ((rn*)e->addr)->val);
	else
		sprintf(code, "%p(%s) := %p(E)", new_id, new_id->name, e->addr);
	gencode(code);

	printf("exit .... 25\n");

	struct_(D) * new_D = NULL;
	new_D = (struct_(D) *)malloc(sizeof(struct_(D)));
	if(!new_D)
		return NULL;
	new_D->type = ty->type;
	new_D->width = ty->width;

	return new_D;
}

void * sub_fun_(17) (void * p) //D->D,id
{
	printf("######################################################################## 17 D->D,id\n");
	struct_(ID) * var_id = NULL;
	struct_(D) * d = NULL;
	get_val(&val_stack, 0, &var_id);
	if(var_id)
		printf("name=%s\n", ((id*)(var_id->addr))->name);
	else
		printf("should not...\n");
	get_val(&val_stack, 2, &d);
	if(d)
		printf("type: %d  width: %d\n", d->type, d->width);
	else
		printf("should not...\n");

	sym_tab * top_tab;
	int top_offset;
	get_head(&tblptr, &top_tab);
	pop(&offset, &top_offset);
	printf("&&&& offset: %d\n", top_offset);
	enter(top_tab, ((id*)(var_id->addr))->name, d->type, top_offset, NULL);
	top_offset += d->width;
	push(&offset, &top_offset);

	struct_(D) * new_D = NULL;
	new_D = (struct_(D) *)malloc(sizeof(struct_(D)));
	if(!new_D)
		return NULL;
	new_D->type = d->type;
	new_D->width = d->width;

	return new_D;
}

void * sub_fun_(18) (void * p) //D->D,id=E
{
	printf("##################################################################### 25 D->D,id=E\n");
	struct_(ID) * var_id = NULL;
	struct_(TY) * d = NULL;
	id_no * new_id = NULL;
	get_val(&val_stack, 2, &var_id);
	if(var_id)
		printf("name=%s\n", ((id*)(var_id->addr))->name);
	else
		printf("should not...\n");
	get_val(&val_stack, 4, &d);
	if(d)
		printf("type: %d  width: %d\n", d->type, d->width);
	else
		printf("should not...\n");

	sym_tab * top_tab;
	int top_offset;
	get_head(&tblptr, &top_tab);
	pop(&offset, &top_offset);
	printf("&&& offset: %d\n", top_offset);
	new_id = enter(top_tab, ((id*)(var_id->addr))->name, d->type, top_offset, NULL);
	top_offset += d->width;
	push(&offset, &top_offset);

	struct_(E) * e = NULL;
	get_val(&val_stack, 0, &e); //store the address of temp variable

	char code[100];
	if(e->type == CONST_INT)
		sprintf(code, "%p(%s) := %p(%d)", new_id, new_id->name, e->addr, ((in*)(e->addr))->val);
	else if(e->type == CONST_REAL)
		sprintf(code, "%p(%s) := %p(%f)", new_id, new_id->name, e->addr, ((rn*)e->addr)->val);
	else
		sprintf(code, "%p(%s) := %p(E)", new_id, new_id->name, e->addr);
	//sprintf(code, "%p(%s) := %p(E)", new_id, new_id->name, e->addr);
	gencode(code);

	printf("exit .... 25\n");

	struct_(D) * new_D = NULL;
	new_D = (struct_(D) *)malloc(sizeof(struct_(D)));
	if(!new_D)
		return NULL;
	new_D->type = d->type;
	new_D->width = d->width;

	return new_D;
}

void * sub_fun_(19) (void * p) //D->TY CLIST]
{
	printf("##################################################################### 19 D->TY CLIST]\n");
	struct_(CLIST) * clist = NULL;
	struct_(TY) * ty = NULL;
	get_val(&val_stack, 1, &clist);
	get_val(&val_stack, 2, &ty);

	sym_tab * top_tab;
	int top_offset;
	get_head(&tblptr, &top_tab);
	pop(&offset, &top_offset);
	add_dims(clist->addr, clist->ndim, ty->width);
	enter(top_tab, clist->name, ty->type, top_offset, clist->addr);
	printf(">>> array: %s offset: %d length: %d ndim: %d\n", clist->name, top_offset, clist->width * ty->width, clist->ndim);
	top_offset += clist->width * ty->width;
	push(&offset, &top_offset);

	struct_(D) * new_D = NULL;
	new_D = (struct_(D) *)malloc(sizeof(struct_(D)));
	if(!new_D)
		return NULL;
	new_D->type = ty->type;
	new_D->width = ty->width;

	return new_D;
}

void * sub_fun_(20) (void * p) //D->D,CLIST]
{
	printf("##################################################################### 20 D->D,CILST]\n");
	struct_(CLIST) * clist = NULL;
	struct_(D) * d = NULL;
	get_val(&val_stack, 1, &clist);
	get_val(&val_stack, 3, &d);

	sym_tab * top_tab;
	int top_offset;
	get_head(&tblptr, &top_tab);
	pop(&offset, &top_offset);
	add_dims(clist->addr, clist->ndim, d->width);
	enter(top_tab, clist->name, d->type, top_offset, clist->addr);
	printf(">>> array: %s offset: %d length: %d ndim: %d\n", clist->name, top_offset, clist->width * d->width, clist->ndim);
	top_offset += clist->width * d->width;
	push(&offset, &top_offset);

	struct_(D) * new_D = NULL;
	new_D = (struct_(D) *)malloc(sizeof(struct_(D)));
	if(!new_D)
		return NULL;
	new_D->type = d->type;
	new_D->width = d->width;

	return new_D;
}

void * sub_fun_(21) (void * p) //A->LEFT=E
{
	printf("######################################################################## 21 A->LEFT=E\n");
	struct_(E) * e = NULL;
	struct_(LEFT) * left = NULL;
	get_val(&val_stack, 0, &e);
	get_val(&val_stack, 2, &left);
	
	char code[100];
	if(!left->offset) //left is simple variable id*
	{
		if(e->type == CONST_INT)
			sprintf(code, "%p(%s) := %p(%d)", left->addr, ((id_no*)(left->addr))->name, e->addr, ((in*)(e->addr))->val);
		else if(e->type == CONST_REAL)
			sprintf(code, "%p(%s) := %p(%f)", left->addr, ((id_no*)(left->addr))->name, e->addr, ((rn*)e->addr)->val);
		else
			sprintf(code, "%p(%s) := %p(E)", left->addr, ((id_no*)(left->addr))->name, e->addr);

		//sprintf(code, "%p(%s) := %p(E)", left->addr, ((id_no*)(left->addr))->name, e->addr);
		gencode(code);
	}
	else //left is a array
	{
		sprintf(code, "%p(%s)[%p] := %p", left->addr, left->addr->name, left->offset, e->addr);
		gencode(code);
	}
	
	return NULL;
}

void * sub_fun_(22) (void * p) //LEFT->ELIST]
{
	printf("######################################################################## 22 LEFT->ELIST]\n");
	struct_(ELIST) * elist = NULL;
	get_val(&val_stack, 1, &elist);
	struct_(LEFT) * new_LEFT = NULL;
	new_LEFT = (struct_(LEFT) *)malloc(sizeof(struct_(LEFT)));
	if(!new_LEFT)
		return NULL;

	sym_tab * top_tab;
	int top_offset;
	get_head(&tblptr, &top_tab);
	pop(&tmp_offset, &top_offset);
	printf("&&& tmp_offset: %d\n", top_offset);
	new_LEFT->addr = elist->array; //attention: use INT?
	new_LEFT->offset = newtemp(top_tab, INT, top_offset, NULL);
	top_offset += 4;
	push(&tmp_offset, &top_offset);

	char code[100];
	//beacause the lows is 0, so don't need to caculate the new_LEFT->addr
	//sprintf(code, "%p := %p", new_LEFT->addr, elist->array);
	//gencode(code);
	sprintf(code, "%p := %p * %d", new_LEFT->offset, elist->addr, ((drop_vector *)(elist->array->pt))->width);
	gencode(code);

	return new_LEFT;
}

void * sub_fun_(23) (void * p) //LEFT->id
{
	printf("######################################################################## 36 LEFT->id\n");
	struct_(ID) * var_id = NULL;
	get_val(&val_stack, 0, &var_id);
	if(var_id)
		printf("name=%s\n", ((id*)(var_id->addr))->name);
	else
		printf("should not...\n");
	struct_(LEFT) * new_LEFT = NULL;
	new_LEFT = (struct_(LEFT) *)malloc(sizeof(struct_(LEFT)));
	if(!new_LEFT)
	{
		printf("newing LEFT has error...\n");
		return NULL;
	}
	new_LEFT->offset = NULL;
	new_LEFT->addr = lookup_sym_tab(((id*)(var_id->addr))->name);
	if(new_LEFT->addr)
		printf("name: %s type: %d offset: %d\n", new_LEFT->addr->name, new_LEFT->addr->type, new_LEFT->addr->offset);
	else
		printf("%s has not defined!\n", ((id*)(var_id->addr))->name);
	return new_LEFT;
}

void * sub_fun_(24) (void * p) //ELIST->ELIST,E
{
	printf("################################################################ 24 ELIST->ELIST,E\n");
	struct_(ELIST) * elist = NULL, * new_ELIST = NULL;
	struct_(E) * e = NULL;
	get_val(&val_stack, 0, &e);
	get_val(&val_stack, 2, &elist);
	new_ELIST = (struct_(ELIST) *)malloc(sizeof(struct_(ELIST)));
	if(!new_ELIST)
		return NULL;

	tmp_struct * t = NULL;
	sym_tab * top_tab;
	int top_offset;
	get_head(&tblptr, &top_tab);
	pop(&tmp_offset, &top_offset);
	printf("&&& tmp_offset: %d\n", top_offset);
	t = newtemp(top_tab, INT, top_offset, NULL);
	top_offset += 4;
	push(&tmp_offset, &top_offset);
	new_ELIST->array = elist->array;
	new_ELIST->addr = t;
	new_ELIST->ndim = elist->ndim + 1;

	char code[100];
	printf("~~~~~~~~~limit: %d\n", limit(elist->array->pt, new_ELIST->ndim - 1));
	sprintf(code, "%p := %p * %d", t, elist->addr, limit(elist->array->pt, new_ELIST->ndim - 1));
	gencode(code);
	sprintf(code, "%p := %p + %p", t, t, e->addr);
	gencode(code);

	return new_ELIST;
}

void * sub_fun_(25) (void * p) //ELIST->id[E
{
	printf("################################################################ 25 ELIST->id[E\n");
	struct_(ID) * var_id = NULL;
	struct_(E) * e = NULL;
	get_val(&val_stack, 0, &e);
	get_val(&val_stack, 2, &var_id);

	struct_(ELIST) * new_ELIST = NULL;
	new_ELIST = (struct_(ELIST) *)malloc(sizeof(struct_(ELIST)));
	if(!new_ELIST)
		return NULL;
	new_ELIST->ndim = 1;
	new_ELIST->array = lookup_sym_tab(((id_no *)(var_id->addr))->name);
	if(new_ELIST->array->pt)
	{
		id_no * tpt = new_ELIST->array;
		printf(">>>>>>>> is a array!\n");
		printf("name: %s type: %d offset: %d dims:%d\n", tpt->name, tpt->type, tpt->offset, ((drop_vector *)(tpt->pt))->dims);
		int i;
		printf("limit: ");
		for(i = 0; i < ((drop_vector *)(tpt->pt))->dims; ++i)
		{
			printf("%d ", limit(tpt->pt, i));
		}
		printf("\n");
	}
	new_ELIST->addr = e->addr;
	printf("<<<<<<<<<<<<< e->type: %d\n", e->type);

	return new_ELIST;
}

void * sub_fun_(26) (void * p) //P->TY id(PA)M{L}
{
	printf("############################################################## 46 P->TY id(PA)M{L}\n");
	sym_tab * top_tab, * out_tab;
	int top_offset, top_tmp_offset;
	struct_(ID) * proc_id = NULL; //proc_id is a pointer pointing to id*
	pop(&tblptr, &top_tab);
	pop(&offset, &top_offset);
	pop(&offset, &top_tmp_offset);
	addwidth(top_tab, top_offset, top_tmp_offset);
	get_head(&tblptr, &out_tab);
	get_val(&val_stack, 7, &proc_id);
	if(proc_id)
		printf("name=%s\n", ((id*)(proc_id->addr))->name);
	else
		printf("should not...\n");
	enterproc(out_tab, ((id*)(proc_id->addr))->name, top_tab);
	return NULL;
}

void * sub_fun_(27) (void * p) //PA->TY id
{
	return NULL;
}

void * sub_fun_(28) (void * p) //PA->ɛ
{
	return NULL;
}

void * sub_fun_(29) (void * p) //PA->TY id, PA
{
	return NULL;
}

void * sub_fun_(30) (void * p) //B->B or Y
{
	return NULL;
}

void * sub_fun_(31) (void * p) //B->Y
{
	return NULL;
}

void * sub_fun_(32) (void * p) //B->E R E
{
	return NULL;
}

void * sub_fun_(33) (void * p) //Y->Y and Z
{
	return NULL;
}

void * sub_fun_(34) (void * p) //Y->Z
{
	return NULL;
}

void * sub_fun_(35) (void * p) //Z->const_bool
{
	return NULL;
}

void * sub_fun_(36) (void * p) //Z->(B)
{
	return NULL;
}

void * sub_fun_(37) (void * p) //R-> >
{
	return NULL;
}

void * sub_fun_(38) (void * p) //R-> <
{
	return NULL;
}

void * sub_fun_(39) (void * p) //R-> ==
{
	return NULL;
}

void * sub_fun_(40) (void * p) //R-> >=
{
	return NULL;
}

void * sub_fun_(41) (void * p) //R-> <=
{
	return NULL;
}

void * sub_fun_(42) (void * p) //R->!=
{
	return NULL;
}

void * sub_fun_(43) (void * p) //S->if(B)M1{L}
{
	return NULL;
}

void * sub_fun_(44) (void * p) //S->if(B)M1{L}elseM2{L}
{
	return NULL;
}

void * sub_fun_(45) (void * p) //S->while(B)M4{L}
{
	return NULL;
}

void * sub_fun_(46) (void * p) //S->A;
{
	return NULL;
}

void * sub_fun_(47) (void * p) //S->D;
{
	return NULL;
}

void * sub_fun_(48) (void * p) //S->call id(PL)
{
	return NULL;
}

void * sub_fun_(49) (void * p) //S->return E;
{
	return NULL;
}

void * sub_fun_(50) (void * p) //L->L S
{
	return NULL;
}

void * sub_fun_(51) (void * p) //L->S
{
	return NULL;
}

void * sub_fun_(52) (void * p) //PL->PL,PO
{
	return NULL;
}

void * sub_fun_(53) (void * p) //PL->PO
{
	return NULL;
}

void * sub_fun_(54) (void * p) //PO->E
{
	return NULL;
}

void * sub_fun_(55) (void * p) //PO->ɛ
{
	return NULL;
}

void * sub_fun_(56) (void * p) //G->D
{
	return NULL;
}

void * sub_fun_(57) (void * p) //G->P
{
	return NULL;
}

void * sub_fun_(58) (void * p) //CC->G
{
	return NULL;
}

void * sub_fun_(59) (void * p) //CC->G CC
{
	return NULL;
}

void * sub_fun_(60) (void * p) //M->ɛ
{
	int tmp_ofs = 0;
	printf("######################################################################## 56 M->ɛ\n");
	sym_tab * t = NULL;
	t = mktable(global_tab);
	if(!t)
	{
		return NULL;
	}
	push(&tblptr, &t);
	push(&offset, &tmp_ofs);
	push(&tmp_offset, &tmp_ofs);
	return t;
}

void * sub_fun_(61) (void * p) //M1->ɛ
{
	return NULL;
}

void * sub_fun_(62) (void * p) //M2->ɛ
{
	return NULL;
}

void * sub_fun_(63) (void * p) //M3->ɛ
{
	return NULL;
}

void * sub_fun_(64) (void * p) //M4->ɛ
{
	return NULL;
}

void * sub_fun_(65) (void * p) //CLIST->CLIST,const_int
{
	printf("########################################################### 65 CLIST->CLIST,const_int\n");
	struct_(CONST_INT) * c_in = NULL;
	struct_(CLIST) * clist = NULL;
	get_val(&val_stack, 0, &c_in);
	get_val(&val_stack, 2, &clist);

	struct_(CLIST) * new_CLIST = NULL;
	new_CLIST = (struct_(CLIST) *)malloc(sizeof(struct_(CLIST)));
	if(!new_CLIST)
		return NULL;
	new_CLIST->name = clist->name; //just point, don't need to copy the cont
	new_CLIST->ndim = clist->ndim + 1;
	new_CLIST->addr = clist->addr; //new a drop vector
	insert_dim(new_CLIST->addr, ((in *)(c_in->addr))->val); //insert a dim
	new_CLIST->width = clist->width * ((in *)(c_in->addr))->val;

	return new_CLIST;
}

void * sub_fun_(66) (void * p) //CLIST->id[const_int
{
	printf("########################################################### 66 CLIST->id[const_int\n");
	struct_(CONST_INT) * c_in = NULL;
	struct_(ID) * var_id = NULL;
	get_val(&val_stack, 0, &c_in);
	get_val(&val_stack, 2, &var_id);

	struct_(CLIST) * new_CLIST = NULL;
	new_CLIST = (struct_(CLIST) *)malloc(sizeof(struct_(CLIST)));
	if(!new_CLIST)
		return NULL;
	new_CLIST->name = ((id*)(var_id->addr))->name; //just point, don't need to copy the cont
	new_CLIST->ndim = 1;
	new_CLIST->addr = new_drop_vector(); //new a drop vector
	insert_dim(new_CLIST->addr, ((in *)(c_in->addr))->val); //insert a dim
	new_CLIST->width = ((in *)(c_in->addr))->val;
	printf(">>> name: %s\n", new_CLIST->name);

	return new_CLIST;
}

//define the pointer of sub_function
typedef void * (*sub_fun_ptr)(void*); //can't write as "...(void *)(*sub_fun_ptr)..."
#define sub_fun_counter 67 //the counter of the sub_fun equal to the counter of the productions
//define the array that store the entrances of the sub_fuctions
sub_fun_ptr sub_fun_table[sub_fun_counter] = 
{
sub_fun_(0), sub_fun_(1), sub_fun_(2), sub_fun_(3), sub_fun_(4), sub_fun_(5),
sub_fun_(6), sub_fun_(7), sub_fun_(8), sub_fun_(9), sub_fun_(10), sub_fun_(11),
sub_fun_(12), sub_fun_(13), sub_fun_(14), sub_fun_(15), sub_fun_(16), sub_fun_(17),
sub_fun_(18), sub_fun_(19), sub_fun_(20), sub_fun_(21), sub_fun_(22), sub_fun_(23),
sub_fun_(24), sub_fun_(25), sub_fun_(26), sub_fun_(27), sub_fun_(28), sub_fun_(29),
sub_fun_(30), sub_fun_(31), sub_fun_(32), sub_fun_(33), sub_fun_(34), sub_fun_(35),
sub_fun_(36), sub_fun_(37), sub_fun_(38), sub_fun_(39), sub_fun_(40), sub_fun_(41),
sub_fun_(42), sub_fun_(43), sub_fun_(44), sub_fun_(45), sub_fun_(46), sub_fun_(47),
sub_fun_(48), sub_fun_(49), sub_fun_(50), sub_fun_(51), sub_fun_(52), sub_fun_(53),
sub_fun_(54), sub_fun_(55), sub_fun_(56), sub_fun_(57), sub_fun_(58), sub_fun_(59),
sub_fun_(60), sub_fun_(61), sub_fun_(62), sub_fun_(63), sub_fun_(64), sub_fun_(65),
sub_fun_(66)
};

