#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "global_define.h"

FILE *syn_fp, *parseli_fp;
char le[20], ri[100];
char syms[maxs][20];
int synn[maxs], sn, st; //sn:the counter of symbols, st:the counter of the terminal
pro prods[max_pro];
int cp; //the counter of prductions
state all_stat[max_stat];
int cs; //the counter of states

//the objective of lr
int goto_tab[max_stat][maxs];
int action_tab[max_stat][maxs];

typedef struct fset
{
	int ter[maxs];
	int ct;
} fs;
fs first_sets[maxs];

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

display_a_pro(int i)
{
	int j;

	printf("%d: %d %s->", i, prods[i].var, syms[get_p(prods[i].var)]);
	for(j = 0; j < prods[i].vl; ++j)
	{
		printf("%s", syms[get_p(prods[i].pv[j])]);
	}
	printf(" ==");
	for(j = 0; j < prods[i].vl; ++j)
	{
		printf(" %d", prods[i].pv[j]);
	}
	putchar('\n');
}

void display_pros()
{
	int i;
	for(i = 0; i < cp; ++i)
	{
		display_a_pro(i);
	}
}

void display_stat(state stat)
{
	jpro *p = stat;
	int i;

	while(p)
	{
		printf("%d %d    ", p->np, p->pp, p->exp_len);
		printf("%s->", syms[get_p(prods[p->np].var)]);
		for(i = 0; i < prods[p->np].vl; ++i)
		{
			if(i == p->pp)
				printf(".");
			printf("%s", syms[get_p(prods[p->np].pv[i])]);
		}
		if(p->pp == prods[p->np].vl)
			printf(".");
		printf("    (%d):", p->exp_len);
		for(i = 0; i < p->exp_len; ++i)
		{
			if(p->exp_sb[i] == 0)
				//printf( " 0-#");
				printf(" #");
			else
				//printf(" %d-%s", p->exp_sb[i], syms[get_p(p->exp_sb[i])]);
				printf(" %s", syms[get_p(p->exp_sb[i])]);
		}
		printf("\n");
		p = p->nj;
	}
}

void display_first_sets()
{
	int i, j;

	printf("@@@@@@@@@@@@@ first set @@@@@@@@@@@@@@@@@@@@@@\n");

	for(i = 0; i < sn; ++i)
	{
		printf("%d: %s    =>    ", i, syms[i]);
		for(j = 0; j < first_sets[i].ct; ++j)
		{
			printf("%s ", syms[get_p(first_sets[i].ter[j])]);
			//printf("%d-%s ", first_sets[i].ter[j], syms[get_p(first_sets[i].ter[j])]);
		}
		putchar('\n');
	}
}

void gen_first_sets()
{
	int i, j, fl, flag = 1, ar, h, k, pvi, cfl;

	for(i = 0; i < sn; ++i) //set first_sets all empty
		first_sets[i].ct = 0;

	for(i = 0; i < sn; ++i)
	{
		fl = 0;
		if(!is_var(synn[i]))
		{
			first_sets[i].ter[first_sets[i].ct] = synn[i];
			(first_sets[i].ct)++;
		}
		else
		{
			for(j = 0; j < cp; ++j)
			{
				if(prods[j].var == synn[i] && !is_var(prods[j].pv[0]))
				{
					for(h = 0; h < first_sets[i].ct; ++h) //remove the duplication
					{
						if(first_sets[i].ter[h] == prods[j].pv[0])
							break;
					}
					if(h == first_sets[i].ct)
					{
						first_sets[i].ter[first_sets[i].ct] = prods[j].pv[0];
						(first_sets[i].ct)++;
					}
				}
			}
		}
	}

	while(flag) //has change?
	{
		flag = 0;
		for(i = 0; i < sn; ++i)
		{
			for(j = 0; j < cp; ++j)
			{
				pvi = 0;
				if(prods[j].var==synn[i] && prods[j].pv[0]!=synn[i] && is_var(prods[j].pv[0]))
				{
					ar = get_p(prods[j].pv[0]);
					for(h = 0; h < first_sets[ar].ct; ++h)
					{
						if(first_sets[ar].ter[h] != -1) //the empty production
						{
							for(k = 0; k < first_sets[i].ct; ++k)
							{
								if(first_sets[i].ter[k] == first_sets[ar].ter[h])
									break;
							}
							if(k == first_sets[i].ct)
							{
								flag = 1; //has changed
								first_sets[i].ter[first_sets[i].ct] = first_sets[ar].ter[h];
								(first_sets[i].ct)++;
							}
						}
						else
							pvi = 1;
					}
				}
				if(pvi>0)
				{
					cfl = 1; //cfl=1 stand for the current variable has empty production
					while(cfl && pvi < prods[j].vl)
					{
						cfl = 0;
						if(!is_var(prods[j].pv[pvi]))
						{
							for(k = 0; k < first_sets[i].ct; ++k) //remove the duplication
							{
								if(first_sets[i].ter[k] == prods[j].pv[pvi])
									break;
							}
							if(k == first_sets[i].ct)
							{
								flag = 1;
								first_sets[i].ter[first_sets[i].ct] = prods[j].pv[0];
								(first_sets[i].ct)++;
							}
							break;
						}
						else
						{
							ar = get_p(prods[j].pv[pvi]);
							for(h = 0; h < first_sets[ar].ct; ++h)
							{
								if(first_sets[ar].ter[h] != -1) //the empty production
								{
									for(k = 0; k < first_sets[i].ct; ++k)
									{
										if(first_sets[i].ter[k] == first_sets[ar].ter[h])
											break;
									}
									if(k == first_sets[i].ct)
									{
										flag = 1; //has changed
										first_sets[i].ter[first_sets[i].ct] = first_sets[ar].ter[h];
										(first_sets[i].ct)++;
									}
								}
								else
								{
									cfl = 1;
									pvi++;
								}
							}
						}
					}
					if(pvi == prods[j].vl)
					{
						for(k = 0; k < first_sets[i].ct; ++k)
						{
							if(first_sets[i].ter[k] == -1)
								break;
						}
						if(k == first_sets[i].ct)
						{
							first_sets[i].ter[first_sets[i].ct] = -1; //add the empty prodution
							(first_sets[i].ct)++;
						}
					}
				}
			}
		}
	}
}

int isij(state stat, jpro* new) //is in the closure j
{
	jpro* p = stat;
	int i, j;

	while(p)
	{
		if(p->np == new->np && p->pp == new->pp) //need to combine
		{
			for(i = 0; i < new->exp_len; ++i)
			{
				for(j = 0; j < p->exp_len; ++j)
				{
					if(new->exp_sb[i] == p->exp_sb[j])
						break;
				}
				if(j == p->exp_len) //can't find, need to add
				{
					p->exp_sb[p->exp_len] = new->exp_sb[i];
					p->exp_len++;
				}
			}
			return 1;
		}
		p = p->nj;
	}

	return 0;
}

int is_exp(jpro* p, int ex)
{
	int i;

	for(i = 0; i < p->exp_len; ++i)
	{
		if(p->exp_sb[i] == ex)
			return 1;
	}

	return 0;
}

state closure(state stat)
{
	jpro *p, *new=NULL;
	int flag = 1, tmp_sym, i, nsym, y, tte, fef, k;
	fs cufs;

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
						if(prods[i].var == tmp_sym)
						{
							new = (jpro*)malloc(sizeof(jpro));
							new->np = i;
							if(prods[i].vl == 1 && prods[i].pv[0] == -1) //just reduce the empty production
								new->pp = 1;
							else
								new->pp = 0;
							new->exp_len = 0;
							if(p->pp+1 == prods[p->np].vl)
							{
								for(k = 0; k < p->exp_len; ++k)
								{
									if(!is_exp(new, p->exp_sb[k]))
									{
										new->exp_sb[new->exp_len] = p->exp_sb[k];
										new->exp_len++;
									}
								}
							}
							for(y = p->pp+1; y < prods[p->np].vl; ++y) //generate expect symbol
							{
								fef = 0;
								nsym = prods[p->np].pv[y];
								cufs = first_sets[get_p(nsym)];
								for(tte = 0; tte < cufs.ct; ++tte)
								{
									if(cufs.ter[tte] == -1)
									{
										fef = 1;
									}
									else
									{
										if(!is_exp(new, cufs.ter[tte]))
										{
											new->exp_sb[new->exp_len] = cufs.ter[tte];
											new->exp_len++;
										}
									}
								}
								if(!fef) //the current symbol'first_set has empty production
									break;
								else if(y == prods[p->np].vl-1) //add the expect symbols
								{
									for(k = 0; k < p->exp_len; ++k)
									{
										if(!is_exp(new, p->exp_sb[k]))
										{
											new->exp_sb[new->exp_len] = p->exp_sb[k];
											new->exp_len++;
										}
									}
								}
							}
						}
						if(new && !isij(stat, new))
						{
							if(!new->exp_len)
							{
								printf("*** error: the length of except symbol should not be 0");
								free(new);
								new = NULL;
							}
							new->nj = p->nj;
							p->nj = new;
							new = NULL; //that is important
							flag = 1;
							break;
						}
						else if(new)
						{
							free(new);
							new = NULL;
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
	int tmp_sym, i;

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
					new_stat->exp_len = p->exp_len;
					for(i = 0; i < p->exp_len; ++i) //just copy the expect symbols
						new_stat->exp_sb[i] = p->exp_sb[i];
				}
				else
				{
					q = (jpro*)malloc(sizeof(jpro));
					q->np = p->np;
					q->pp = p->pp + 1;
					q->exp_len = p->exp_len;
					for(i = 0; i < p->exp_len; ++i) //just copy the expect symbols
						q->exp_sb[i] = p->exp_sb[i];
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

void display_all_stats()
{
	int i, j, nno;
	state nstat;

	printf("@@@@@@@@@@@@@@ all states @@@@@@@@@@@@@@@@@@@@\n");
	for(i = 0; i < cs; ++i)
	{
		printf(">>>>>>>>>>>>>>>>>>>>> %d\n", i);
		display_stat(all_stat[i]);
		for(j = 0; j < sn; ++j)
		{
			nstat = go(all_stat[i], synn[j]);
			if(nstat)
			{
				nno = is_exist_stat(nstat);
				printf("********* %s %d\n", syms[j], nno);
				display_stat(all_stat[nno]);
			}
		}
	}
}

int is_exist_stat(state stat)
{
	state p;
	jpro *q, *h;
	int i, n1, n2=0, j, k;

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
					if(q->np == h->np && q->pp == h->pp && q->exp_len == h->exp_len)
					{
						for(j = 0; j < q->exp_len; ++j)
						{
							for(k = 0; k < h->exp_len; ++k)
							{
								if(q->exp_sb[j] == h->exp_sb[k])
									break;
							}
							if(k == h->exp_len)
								break;
						}
						if(j == q->exp_len)
							break;
					}
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
			//printf("&&& %d %d %s\n", i, j, syms[j]);
			//display_stat(all_stat[i]);
			if(tmp_stat && (is_exist_stat(tmp_stat) < 0))
			{
				//printf("$$$$$$$\n");
				//display_stat(tmp_stat);
				all_stat[cs] = tmp_stat;
				cs++; //cs is also in the for
			}
		}
	}
}

void distroy_stats()
{
	int i;
	jpro *p, *q;

	for(i = 0; i < cs; ++i)
	{
		p = all_stat[i];
		while(p)
		{
			q = p->nj;
			free(p);
			p = q;
		}
		all_stat[i] = NULL;
	}
}

void cret_parli()
{
	int i, j, nno, nsb, tpp, esm;
	state nstat;
	jpro * p;

	for(i = 0; i < max_stat; ++i) //init the state_array
		all_stat[i] = NULL;

	all_stat[0] = (state)malloc(sizeof(jpro));
	cs++; //the counter of states is 1 now
	all_stat[0]->np = 0; //the first production is the S in the grammar
	all_stat[0]->pp = 0; //the point in the leftest position
	all_stat[0]->exp_sb[0] = 0; //0 stand for '#'
	all_stat[0]->exp_len = 1;
	all_stat[0]->nj = NULL;

	all_stat[0] = closure(all_stat[0]);
	gen_states(); //generate states

	lr1_status_DEBUG(display_all_stats();)

	lr1_parli_DEBUG(printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@ has conflict? @@@@\n");)

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
					for(j = 0; j < p->exp_len; ++j)
					{
						esm = p->exp_sb[j];
						if(esm != 0)
						{
							esm = get_p(esm);
							if(action_tab[i][esm] != -1 && action_tab[i][esm] != ((p->np<<1)+1))
							{
								printf("***3 state %d has conflict\n", i);
								break;
							}
							else
								action_tab[i][esm] = (p->np << 1) + 1;
						}
						else //is the terminal symbol
						{
							if(action_tab[i][st] != -1 && action_tab[i][st] != ((p->np<<1)+1))
							{
								printf("***4 state %d has conflict\n", i);
							}
							else
								action_tab[i][st] = (p->np << 1) + 1;

						}
					}
				}
			}
			p = p->nj;
		}
	}

	distroy_stats(); //recycle the space storing states

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

int start_create()
{
	syn_fp = fopen("syntax_cg.s", "rb");
	read_pros(); //read the productions
	fclose(syn_fp);
	lr1_parli_DEBUG(printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@ productions @@@@@@\n");)
	lr1_parli_DEBUG(display_pros();) //display the productions
	gen_first_sets(); //generate the first sets
	lr1_parli_DEBUG(display_first_sets();) //display the first sets
	cret_parli(); //create the lr1 parse list

	return 0;
}
