#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "global_define.h"

//Attention, maybe have problem, should change the makefile.
extern int hashpjw(char* s); //declare, define in hash.c
extern stack tblptr;

void init_symbol_table()
{
	int i;

	for(i = 0; i < BUCKETS; ++i)
	{
		symbol_table[i] = NULL;
	}
}

id* lookup(char * name)
{
	int num_bucket;
	id * p;

	num_bucket = hashpjw(name);
	p = symbol_table[num_bucket];

	while(p != NULL)
	{
		if(strcmp(p->name, name) == 0)
			return p;
		p = p->next_hash;
	}
	return NULL; //not in the symbol_table
}

id* insert(char * name)
{
	int num_bucket;
	id *head, *p;

	num_bucket = hashpjw(name);
	head = symbol_table[num_bucket];
	p = lookup(name);

	if(!p)
	{
		p = (id*)malloc(sizeof(id));
		p->name = (char*)malloc((strlen(name)+1)*sizeof(char));
		strcpy(p->name, name); //copy the string
		p->next_hash = head;
		symbol_table[num_bucket] = p;
	}

	return p;
}

void destroy()
{
	int i;
	id *p, *np;

	for(i = 0; i < BUCKETS; ++i)
	{
		p = symbol_table[i];
		while(p != NULL)
		{
			np = p->next_hash;
			if(p->name != NULL)
				free(p->name); //recycle space
			free(p); //recycle space
			p = np;
		}
		symbol_table[i] = NULL;
	}
}

void init_global_tab(void)
{
	global_tab = (sym_tab *)malloc(sizeof(sym_tab));

	global_tab->width = 0;
	global_tab->tmp_width = 0;
	global_tab->previous = NULL;
	global_tab->tail = NULL;
	global_tab->tail_tmp = NULL;
	global_tab->procs = NULL;
}

sym_tab * mktable(sym_tab * previous)
{
	sym_tab * new_tab;
	new_tab = (sym_tab *)malloc(sizeof(sym_tab));
	if(!new_tab)
		return NULL;
	new_tab->previous = previous;
	new_tab->width = 0;
	new_tab->tmp_width = 0;
	new_tab->tail = NULL;
	new_tab->tail_tmp = NULL;
	new_tab->procs = NULL;
	return new_tab;
}

id_no * enter(sym_tab * table, char * name, int type, int offset, void * pt)
{
	printf("enter enter...\n");
	id_no * new_no = (id_no *)malloc(sizeof(id_no));
	if(!new_no)
		return NULL;
	new_no->name = (char *)malloc((strlen(name)+1)*sizeof(char));
	memcpy(new_no->name, name, strlen(name)*sizeof(char));
	new_no->name[strlen(name)] = '\0';
	new_no->type = type;
	new_no->offset = offset;
	new_no->pt = pt;
	//attention: the all symbol generate a cycle
	if(!table->tail) //the table is null
	{
		table->tail = new_no;
		table->tail->next_id = new_no;
	}
	else
	{
		new_no->next_id = table->tail->next_id;
		table->tail->next_id = new_no;
		table->tail = new_no;
	}
	printf("exit enter...\n");

	return new_no;
}

int addwidth(sym_tab * table, int width, int tmp_width)
{
	table->width = width;
	table->tmp_width = tmp_width;
	return 1;
}

int enterproc(sym_tab * table, char * name, sym_tab * newtable)
{ //if new_table is a function name not null, else if new_table is a region name is null
	proc_no * new_proc;

	new_proc = (proc_no *)malloc(sizeof(proc_no));
	if(name)
	{
		new_proc->name = (char*)malloc((strlen(name)+1)*sizeof(char));
		memcpy(new_proc->name, name, strlen(name));
		new_proc->name[strlen(name)] = '\0';
	}
	else
		new_proc->name = NULL;
	new_proc->proc_pt = newtable;
	if(!table->procs)
	{
		table->procs = new_proc;
		table->procs->next_proc = new_proc; //the cycle consist itself
	}
	else
	{
		new_proc->next_proc = table->procs->next_proc;
		table->procs->next_proc = new_proc;
		table->procs = new_proc;
	}

	return 1;
}

void destroy_sym_tab(sym_tab * rm_tab) //recycle the memory
{
	sym_tab * tab;
	id_no * it, * itt;
	proc_no * pt, * ptt;
	tmp_struct * tmp_tmp, * ttt;

	tab = rm_tab;
	if(tab->tail)
	{
		it = tab->tail->next_id;
		tab->tail->next_id = NULL; //destroy the cycle
		while(it)
		{
			itt = it->next_id;
			free(it->name);
			if(it->pt)
				free(it->pt);
			free(it);
			it = itt;
		}
		tab->tail = NULL;
	}
	if(tab->tail_tmp)
	{
		tmp_tmp = tab->tail_tmp->next_tmp;
		tab->tail_tmp->next_tmp = NULL;
		while(tmp_tmp)
		{
			ttt = tmp_tmp->next_tmp;
			if(tmp_tmp->pt)
				free(tmp_tmp->pt);
			free(tmp_tmp);
			tmp_tmp = ttt;
		}
		tab->tail_tmp = NULL;
	}
	if(tab->procs)
	{
		pt = tab->procs->next_proc; //destroy the cycle
		tab->procs->next_proc = NULL;
		while(pt)
		{
			ptt = pt->next_proc;
			if(pt->name) //pt is a funciton in global region
				free(pt->name);
			destroy_sym_tab((sym_tab*)(pt->proc_pt)); //recurse
			free(pt);
			pt = ptt;
		}
		tab->procs = NULL;
	}
}

id_no * lookup_sym_tab(char * name)
{
	sym_tab * corrent = NULL;
	id_no * tmp_id = NULL, * saved_tail;
	get_head(&tblptr, &corrent);

	while(corrent)
	{
		if(corrent->tail) //sym_table is not null
		{
			saved_tail = corrent->tail;
			tmp_id = corrent->tail;
			do
			{
				if(!strcmp(tmp_id->name, name))
					return tmp_id;
				tmp_id = tmp_id->next_id;
			}
			while(tmp_id != saved_tail);
		}
		corrent = corrent->previous; //the outer layer
	}

	return NULL; //should report a error
}

tmp_struct * newtemp(sym_tab * table, int type, int offset, void * pt)
{
	printf("enter newtemp...\n");
	tmp_struct * new_no = (tmp_struct *)malloc(sizeof(tmp_struct));
	if(!new_no)
		return NULL;
	new_no->type = type;
	new_no->offset = offset;
	new_no->pt = pt;
	//attention: the all symbol generate a cycle
	if(!table->tail_tmp) //the table is null
	{
		table->tail_tmp = new_no;
		table->tail_tmp->next_tmp = new_no;
	}
	else
	{
		new_no->next_tmp = table->tail_tmp->next_tmp;
		table->tail_tmp->next_tmp = new_no;
		table->tail_tmp = new_no;
	}
	printf("exit newtemp...\n");

	return new_no;
}

void init_drop_vector()
{
	all_vector = NULL;
}

drop_vector * new_drop_vector()
{
	drop_vector * new_vec = NULL;

	new_vec = (drop_vector *)malloc(sizeof(drop_vector));
	if(!new_vec)
		return NULL;
	new_vec->dims = 0;
	new_vec->head_dim = NULL;
	
	new_vec->next_vec = all_vector;
	all_vector = new_vec;
	
	return new_vec;
}

int insert_dim(drop_vector * vec, int len) //high in left, low in right, like 4 3 2 1 0
{
	dim_len * new_dim = NULL;
	new_dim = (dim_len *)malloc(sizeof(dim_len));
	if(!new_dim)
		return 0;
	new_dim->len = len;
	new_dim->next_len = vec->head_dim;
	vec->head_dim = new_dim;
	
	return 1;
}

int add_dims(drop_vector * vec, int dims, int width)
{
	vec->dims = dims;
	vec->width = width;
	return 1;
}

int limit(drop_vector * vec, int m) //m start from 0
{
	int i;
	dim_len * dl = NULL;
	
	printf("~~~~~~~ dims:%d m:%d\n", vec->dims, m);

	if(m >= vec->dims)
	{
		printf("error: array is out of bound!\n");
		return -1;
	}
	
	dl = vec->head_dim;
	printf("~~~~~~~ %d\n", dl->len);
	for(i = 1; i < vec->dims - m; ++i)
	{
		printf("~~~~~~~ %d\n", dl->len);
		dl = dl->next_len;
	}

	return dl->len;
}

void destroy_all_vector()
{
	drop_vector * dv = NULL, * tdv = NULL;
	dim_len * dl = NULL, * tdl = NULL;

	dv = all_vector;
	while(dv)
	{
		tdv = dv->next_vec;
		dl = dv->head_dim;
		while(dl)
		{
			tdl = dl->next_len;
			free(dl);
			dl = tdl;
		}
		free(dv);
		dv = tdv;
	}

	all_vector = NULL;
}
