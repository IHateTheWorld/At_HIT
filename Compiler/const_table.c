#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "global_define.h"

#define NUM_ZERO 1.0e-10
extern int hashpjw(char* s);

void init_string_table()
{
	int i;

	for(i = 0; i < STRING_BUCKETS; ++i)
	{
		string_table[i] = NULL;
	}
}

string* lookup_string(char * s)
{
	int num_bucket;
	string * p;

	num_bucket = hashpjw(s);
	p = string_table[num_bucket];
	while(p != NULL)
	{
		if(strcmp(p->str, s) == 0)
			return p;
		p = p->next_str;
	}
	return NULL;
}

string* insert_string(char * s)
{
	int num_bucket;
	string * head, *p;

	num_bucket = hashpjw(s);
	head = string_table[num_bucket];
	p = lookup_string(s);

	if(!p)
	{
		p = (string*)malloc(sizeof(string));
		p->str = (char*)malloc((strlen(s)+1)*sizeof(char));
		strcpy(p->str, s);
		p->next_str = head;
		string_table[num_bucket] = p;
	}

	return p;
}

void destroy_strings()
{
	int i;
	string *p, *np;

	for(i = 0; i < STRING_BUCKETS; ++i)
	{
		p = string_table[i];
		while(p)
		{
			np = p->next_str;
			if(p->str)
				free(p->str);
			free(p);
			p = np;
		}
		string_table[i] = NULL;
	}
}

void init_int_table()
{
	int i;

	for(i = 0; i < 10; ++i)
		int_table[i] = NULL;
}

int maxv(int v) //int's hash function
{
	int i;

	i = v;
	if(i / 100000000) i = i / 100000000;
	if(i / 10000) i = i / 10000;
	if(i / 100) i = i / 100;
	if(i / 10) i = i / 10;

	return i;
}

in* lookup_int(int v)
{
	int n;
	in *p;

	n = maxv(v);
	p = int_table[n];
	while(p != NULL)
	{
		if(p->val == v)
			return p;
		p = p->next_int;
	}

	return NULL;
}

in* insert_int(int v)
{
	int n;
	in *p, *head;

	p = lookup_int(v);
	if(!p)
	{
		n = maxv(v);
		head = int_table[n];
		p = (in*)malloc(sizeof(in));
		p->val = v;
		p->next_int = head;
		int_table[n] = p;
	}

	return p;
}

void destroy_ints()
{
	int i;
	in *p, *np;

	for(i = 0; i < 10; ++i)
	{
		p = int_table[i];
		while(p)
		{
			np = p->next_int;
			free(p);
			p = np;
		}
		int_table[i] = NULL;
	}
}

void init_real_table()
{
	real_table = NULL;
}

rn * lookup_real(float r)
{
	rn *p;
	
	p = real_table;
	while(p)
	{
		if(fabs(p->val-r) <= NUM_ZERO) //judge the equal of two floats
			return p;
	}

	return NULL;
}

rn * insert_real(float r)
{
	rn *p;

	p = lookup_real(r);
	if(!p)
	{
		p = (rn*)malloc(sizeof(rn));
		p->val = r;
		p->next_real = real_table;
		real_table = p;
	}

	return p;
}

void destroy_reals()
{
	rn *p, *np;

	p = real_table;
	while(p)
	{
		np = p->next_real;
		free(p);
		p = np;
	}
}

