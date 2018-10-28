#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define maxs 100
FILE *syn_fp, *parseli_fp;
char syms[maxs][10];
int synn[maxs];
int sn;
int st;
char le[10];
char ri[100];

typedef struct pro_n
{
	int var; //the value of variable
	int pv[50]; //save the value of production
	int vl; //len of production
} pro;

#define max_pro 200 //the max counter of productions
pro prods[max_pro];
int cp; //the counter of prductions

typedef struct j_pro_n
{
	int np; //the NO of production
	int pp; //the position of point '.'
	int exp_sb[20]; //the expect symbol
	struct j_pro_n * nj; //the next j_pro_n
} jpro;

typedef jpro* state; //the production in closure

#define max_stat 2000 //the max counter of states
state all_stat[max_stat];
int cs; //the counter of states

int goto_tab[max_stat][maxs];
int action_tab[max_stat][maxs];

int chle(char ch)
{
	if(ch >= 'a' && ch <= 'z')
		return 0;
	else if(ch >= 'A' && ch <= 'Z')
		return 1;
}

int is_var(int num)
{
	if(num > 99) //the variable is greater than 99
		return 1;
	return 0;
}

int get_no(char * s)
{
	int i;

	for(i = 0; i < sn; ++i)
	{
		if(strcmp(syms[i], s) == 0)
			return synn[i];
	}
	return -1;
}

int get_p(int tors)
{
	int i;

	for(i = 0; i < sn; ++i)
	{
		if(synn[i] == tors)
			return i;
	}
	return -1;
}

void read_pros()
{
	int sw = 0, lc, rc, tv, ns;
	char tcs[10];

	cp = 0; //init the counter of productions
	st = -1;

	while(!feof(syn_fp))
	{
		if((fscanf(syn_fp,"%s",le)!=EOF)&&(fscanf(syn_fp,"%s",ri)!=EOF))
		{
			if(strcmp(le, "###") == 0 && strcmp(ri, "###") == 0)
			{
				sn = sw;
				sw = -1;
				continue;
			}
			if(sw >= 0)
			{
				strcpy(syms[sw], le);
				synn[sw] = atoi(ri);
				if(st < 0 && synn[sw] > 99) //the first variable
					st = sw;
				sw++;
			}
			else
			{
				tv = get_no(le);
				lc = rc = 0;
				while(rc < strlen(ri))
				{
					prods[cp].var = tv;
					lc = rc;
					while(rc < strlen(ri) && ri[rc] != '|' &&ri[rc] != '@')
					{
						rc++;
					}
					memcpy(tcs, ri+lc, rc-lc);
					tcs[rc-lc] = '\0';
					prods[cp].pv[ns++] = get_no(tcs);
					prods[cp].vl = ns;
					if(ri[rc] == '|' || rc >= strlen(ri))
					{
						cp++;
						ns = 0;
					}
					rc++;
				}
			}
		}
	}
}

void display_pros()
{
	int i, j;
	for(i = 0; i < cp; ++i)
	{
		printf("%d: %d %d    ", i, prods[i].var, prods[i].vl);
		for(j = 0; j < prods[i].vl; ++j)
		{
			printf(" %d", prods[i].pv[j]);
		}
		putchar('\n');
	}
}

void display_stat(state stat)
{
	jpro *p = stat;

	while(p)
	{
		printf("%d %d\n", p->np, p->pp);
		p = p->nj;
	}
}

int isij(state stat, int np, int pp) //is in the closure j
{
	jpro* p = stat;

	while(p)
	{
		if(p->np == np && p->pp == pp)
			return 1;
		p = p->nj;
	}

	return 0;
}

state closure(state stat)
{
	jpro *p, *new;
	int flag = 1, tmp_sym, i;

	while(flag) //flag stand for "is changed?"
	{
		flag = 0;
		p = stat; //while flag not while p
		while(p)
		{
			if(p->pp < prods[p->np].vl) //not reduction
			{
				tmp_sym = prods[p->np].pv[p->pp];
				if(is_var(tmp_sym)) //the next_symbol is a variable
				{
					for(i = 0; i < cp; ++i)
					{
						if(prods[i].var == tmp_sym && (!isij(stat, i, 0)))
						{
							new = (jpro*)malloc(sizeof(jpro));
							new->np = i;
							new->pp = 0;
							new->nj = p->nj;
							p->nj = new;
							flag = 1;
							break;
						}
					}
				}
			}
			p = p->nj;
		}
	}

	return stat;
}

state go(state stat, int nsym)
{
	jpro *p = stat, *q;
	state new_stat = NULL;
	int tmp_sym;

	while(p)
	{
		if(p->pp < prods[p->np].vl) //not reduction
		{
			tmp_sym = prods[p->np].pv[p->pp];
			//printf("nsym = %d\n", nsym);
			if(tmp_sym == nsym)
			{
				if(!new_stat) //need to add one state
				{
					new_stat = (state)malloc(sizeof(jpro));
					new_stat->nj = NULL;
					new_stat->np = p->np;
					new_stat->pp = p->pp + 1;
				}
				else
				{
					q = (jpro*)malloc(sizeof(jpro));
					q->np = p->np;
					q->pp = p->pp + 1;
					q->nj = new_stat; //link the nodes
					new_stat = q; //not new_stat->nj, that can cause dead circulation
				}	
			}
		}
		p = p->nj;
	}

	if(new_stat)
		return closure(new_stat);
	else
		return NULL;
}

int is_exist_stat(state stat)
{
	state p;
	jpro *q, *h;
	int i, n1, n2=0;

	h = stat;
	while(h) //calc the counter of productions in stat
	{
		n2++;
		h = h->nj;
	}

	for(i = 0; i < cs; ++i)
	{
		p = all_stat[i];
		//to judge that p is the same as stat ... it is so ugly ... fuck !!!
		q = p;
		n1 = 0; //should not be put outside
		while(q) //calc the counter of productions in all_stat[i]
		{
			n1++;
			q = q->nj;
		}
		if(n1 == n2)
		{
			q = p;
			while(q)
			{
				h = stat;
				while(h)
				{
					if(q->np == h->np && q->pp == h->pp)
						break;
					h = h->nj;
				}
				if(!h) //stand for is not same
					break;
				q = q->nj;
			}
			if(!q)
				return i; //the num of the next state
		}
		//else //stand for is not the same
		//	continue;
	}

	return -1;
}

void gen_states()
{
	int i, j;
	state tmp_stat;

	for(i = 0; i < cs; ++i)
	{
		for(j = 0; j < sn; ++j)
		{
			tmp_stat = go(all_stat[i], synn[j]);
			if(tmp_stat && (is_exist_stat(tmp_stat) < 0))
			{
				all_stat[cs] = tmp_stat;
				cs++; //cs is also in the for
			}
		}
	}
}

void cret_parli()
{
	int i, j, nno, nsb, tpp;
	state nstat;
	jpro * p;

	for(i = 0; i < max_stat; ++i) //init the state_array
		all_stat[i] = NULL;

	all_stat[0] = (state)malloc(sizeof(jpro));
	cs++; //the counter of states is 1 now
	all_stat[0]->np = 0; //the first production is the S in the grammar
	all_stat[0]->pp = 0; //the point in the leftest position
	all_stat[0]->nj = NULL;

	all_stat[0] = closure(all_stat[0]);
	//display_stat(all_stat[0]);
	gen_states(); //generate states

	//display all states
	for(i = 0; i < cs; ++i)
	{
		printf("@@@ %d\n", i);
		display_stat(all_stat[i]);
	}

	printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ 2th part...\n");

	//init the tables
	for(i = 0; i < cs; ++i)
	{
		for(j = 0; j < st+1; ++j)
			action_tab[i][j] = -1;
		for(j = 0; j < sn-st; ++j)
			goto_tab[i][j] = -1;
	}

	//create the parse table
	for(i = 0; i < cs; ++i)
	{
		p = all_stat[i];
		while(p)
		{
			if(p->pp < prods[p->np].vl)
			{
				nsb = prods[p->np].pv[p->pp];
				nstat = go(all_stat[i], nsb);
				nno = is_exist_stat(nstat);
				if(nstat && nno >= 0)
				{
					tpp = get_p(nsb);
					if(is_var(nsb)) //add goto_tab
					{
						if(goto_tab[i][tpp-st] != -1 && goto_tab[i][tpp-st] != nno)
							printf("***1 state %d has conflict\n", i);
						else
							goto_tab[i][tpp-st] = nno;
					}
					else //add action_tab
					{
						if(action_tab[i][tpp] != -1 && action_tab[i][tpp] != (nno << 1))
							printf("***2 state %d has conflict\n", i);
						else
							action_tab[i][tpp] = nno << 1; //lsb is a flag whether reduce or move_in
					}
				}
				else if(nstat && nno < 0)
					printf("*** error: the closure should be exist\n");
			}
			else //should be reduced or success
			{
				if(p->np == 0 && p->pp == 1) //ss->s.
					action_tab[i][st] = -2; //-2 stand for success, st stand for the line '#'
				else
				{
					for(j = 0; j < st+1; ++j)
					{
						if(action_tab[i][j] != -1 && action_tab[i][j] != ((p->np<<1)+1))
						{
							printf("***3 state %d has conflict\n", i);
							break;
						}
						else
							action_tab[i][j] = (p->np << 1) + 1;
					}
				}
			}
			p = p->nj;
		}
	}

	//write the parse tabel to the file
	parseli_fp = fopen("parli.l", "wb");
	for(i = 0; i < cs; ++i)
	{
		for(j = 0; j < st+1; ++j)
			fprintf(parseli_fp, "%d ", action_tab[i][j]);
		for(j = 0; j < sn-st; ++j)
			fprintf(parseli_fp, "%d ", goto_tab[i][j]);
		fprintf(parseli_fp, "\n");
	}
	fclose(parseli_fp);
}

int main(void)
{
	syn_fp = fopen("syntax_cg.s", "rb");
	read_pros(); //read the productions
	fclose(syn_fp);
	display_pros();//display the productions
	printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
	cret_parli();

	return 0;
}
