#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"

FILE *syn_fp, *nsyn_fp;
char pro_left[10], pro_right[20];

typedef struct pro_line
{
	char * ap;
	struct pro_line * np;
} pro_l;

typedef struct pro_node
{
	char * var;
	pro_l * pro1; //right start with left, left recursive
	pro_l * pro2; //not the left recursive
	struct pro_node * nn;
} pro_n;

pro_n * pro_head;
pro_n * npro_head;

void init_pro()
{
	pro_head = NULL;
	npro_head = NULL;
}

pro_l* lookup_pro(pro_n * pro_head, char * left, char * right, int ispre)
{
	pro_n * p;
	pro_l * l;

	p = pro_head;
	while(p)
	{
		if(strcmp(left, p->var) == 0)
		{
			if(ispre)
				l = p->pro1;
			else
				l = p->pro2;
			while(l)
			{
				if(strcmp(l->ap, right) == 0)
					return l;
				l = l->np;
			}
			if(ispre)
				return p->pro1;
			else
				return p->pro2;
		}
		p = p->nn;
	}

	return NULL;
}

void insert_pro(pro_n * pro_head, char * left, char * right, int ispre)
{
	pro_n *p;
	pro_l *l, *nl;

	l = lookup_pro(pro_head, left, right, ispre);
	if(l)
	{
		if(strcmp(l->ap, right) == 0)
			return ; //pass
		else
		{
			nl = (pro_l*)malloc(sizeof(pro_l));
			nl->ap = (char*)malloc((strlen(right)+1)*sizeof(char));
			strcpy(nl->ap, right);
			nl->np = l->np;
			l->np = nl;
		}
	}
	else
	{
		p = pro_head;
		while(p)
		{
			if(strcmp(left, p->var) == 0)
			{
				nl = (pro_l*)malloc(sizeof(pro_l));
				nl->ap = (char*)malloc((strlen(right)+1)*sizeof(char));
				strcpy(nl->ap, right);
				nl->np = NULL;
				if(ispre)
					p->pro1 = nl;
				else
					p->pro2 = nl;
				return ;
			}
			p = p->nn;
		}
		p = (pro_n*)malloc(sizeof(pro_n));
		p->var = (char*)malloc((strlen(left)+1)*sizeof(char));
		strcpy(p->var, left);
		p->nn = pro_head;
		pro_head = p;
		nl = (pro_l*)malloc(sizeof(pro_l));
		nl->ap = (char*)malloc((strlen(right)+1)*sizeof(char));
		strcpy(nl->ap, right);
		nl->np = NULL;
		if(ispre)
		{
			p->pro1 = nl;
			p->pro2 = NULL;
		}
		else
		{
			p->pro1 = NULL;
			p->pro2 = nl;
		}
	}
}

void read_syn()
{
	int i, lp, rp, pp, ispre;
	char v[20];

	while(!feof(syn_fp))
	{
		if((fscanf(syn_fp, "%s", pro_left)!=EOF)&&(fscanf(syn_fp, "%s", pro_right)!=EOF))
		{
			printf("%s %s %zd\n", pro_left, pro_right, strlen(pro_right));
			lp = rp = 0;
			while(rp < strlen(pro_right))
			{
				lp = rp;
				pp = 0;
				ispre = 1;
				while(rp < strlen(pro_right) && pro_right[rp] != '|')
				{
					if(ispre && pp < strlen(pro_left) && pro_left[pp] == pro_right[rp])
						pp++;
					else if(pp < strlen(pro_left))
						ispre = 0;
					rp++;
				}
				if(ispre)
				{
					memcpy(v, pro_right+strlen(pro_left)*sizeof(char)+lp, rp-lp-strlen(pro_left)*sizeof(char));
					v[rp-lp-strlen(pro_left)*sizeof(char)] = '\0';
				}
				else
				{
					memcpy(v, pro_right+lp, rp-lp);
					v[rp-lp] = '\0';
				}
				printf("@@@ %s %d\n", v, ispre);
				insert_pro(pro_head, pro_left, v, ispre);
				rp++;
			}
		}
	}
}


void rm_lr() //remove the left recursive
{
	pro_n *p;
	pro_l *l;
	char tmp[20], tl[10];

	p = pro_head;
	while(p != NULL)
	{
		l = p->pro1; //the lr part
		memcpy(tl, p->var, strlen(p->var)*sizeof(char));
		tl[strlen(p->var)] = '1';
		tl[strlen(p->var)+1] = '\0';
		while(l)
		{
			memcpy(tmp, p->var, strlen(p->var)*sizeof(char));
			tmp[strlen(p->var)] = '1';
			memcpy(tmp+(strlen(p->var)+1)*sizeof(char), l->ap, strlen(l->ap)*sizeof(char));
			tmp[strlen(p->var)+strlen(l->ap)+1] = '\0';
			insert_pro(npro_head, tl, tmp, 0); //is not lr
			l = l->np;
		}
		strcpy(tmp, "ε"); //the ε production
		insert_pro(npro_head, tl, tmp, 0);
		l = p->pro2; //not the lr part
		while(l)
		{
			memcpy(tmp, l->ap, strlen(l->ap)*sizeof(char));
			memcpy(tmp+strlen(l->ap)*sizeof(char), p->var, strlen(p->var)*sizeof(char));
			tmp[strlen(l->ap)+strlen(p->var)+1] = '1';
			tmp[strlen(l->ap)+strlen(p->var)+2] = '\0';
			insert_pro(npro_head, p->var, tmp, 0);
			l = l->np;
		}
		p = p->nn;
	}
}

int main(void)
{
	syn_fp = fopen("syntax.s", "rb");
	nsyn_fp = fopen("rmlr_syntax.s", "wb");
	init_pro(); //init the list of syntax
	read_syn();
	fclose(syn_fp);
	fclose(nsyn_fp);

	return 0;
}

