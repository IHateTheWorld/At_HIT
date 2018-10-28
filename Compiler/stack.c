#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "global_define.h"

void init_stack(stack * st, int si)
{
	st->head = NULL;
	st->size = si;
}

int is_empty(stack * st)
{
	if(st->head == NULL)
		return 1;
	return 0;
}

void push(stack * st, const void * puc)
{
	stack_node * nn;

	nn = (stack_node *)malloc(sizeof(stack_node));
	nn->cont = malloc(st->size);
	memcpy(nn->cont, puc, st->size);
	nn->next = st->head;
	st->head = nn;
}

int pop(stack * st, void * poc)
{
	stack_node * tmp;
	
	if(!is_empty(st))
	{
		memcpy(poc, st->head->cont, st->size);
		tmp = st->head;
		st->head = st->head->next;
		free(tmp);
		return 1;
	}
	else
		return 0;
}

int get_head(stack * st, void * gec)
{
	if(!is_empty(st))
	{
		memcpy(gec, st->head->cont, st->size);
		return 1;
	}
	gec = NULL;
	return 0;
}

int get_val(stack * st, int no, void * gec) //fetch the no(th) element, attention: no start from 0
{
	int i;
	stack_node * tmp;

	tmp = st->head;
	for(i = 0; i < no; ++i)
	{
		if(!tmp)
			return 0;
		tmp = tmp->next;
	}
	memcpy(gec, tmp->cont, st->size);
	return 1;
}

void destroy_stack(stack * st)
{
	stack_node * tmp;

	while(st->head)
	{
		tmp = st->head->next;
		free(st->head);
		st->head = tmp;
	}
}

