#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "extmem.h"

#define R_C 112
#define S_C 224

struct rs_ele
{
	int e1;
	int e2;
};
struct new_ele
{
	struct rs_ele r;
	struct rs_ele s;
};

int gen_core(int d1, int u1, int d2, int u2, char * flag, int rs_c, char * tar_flag)
{
	Buffer rs_buf;
	unsigned char * blkptr, * blkep;
	int i, j, up1, up2, down1, down2, pn;
	struct rs_ele para, tmp_para;
	
	if(!initBuffer(2*((u2-d2+1)*sizeof(para)+1), (u2-d2+1)*sizeof(para), &rs_buf))
		return 0;

	for(i = d1; i <= u1; ++i)
	{
		blkptr = getNewBlockInBuffer(&rs_buf);
		for(j = d2; j <= u2; ++j)
		{
			para.e1 = i;
			para.e2 = j;
			memcpy(blkptr+sizeof(para)*(j-d2), &para, sizeof(para));
		}
		writeBlockToDisk(blkptr, flag, i, &rs_buf);
	}

	down1 = d1; down2 = d2; up1 = u1; up2 = u2;
	for(i = 0; i < rs_c; ++i)
	{
		para.e1 = rand() % (up1-down1+1) + down1;
		para.e2 = rand() % (up2-down2+1) + down2;
		up2--;
		if(!up2)
		{
			up2 = u2;
			up1--;
		}
		pn = u1 - (i/(u2-d2+1));
		if(pn == para.e1)
		{
			blkptr = readBlockFromDisk(flag, pn, &rs_buf);
			memcpy(&tmp_para, blkptr+sizeof(para)*(para.e2-d2), sizeof(para));
			memcpy(blkptr+sizeof(para)*(para.e2-d2), \
					blkptr+sizeof(para)*(u2-(i%(u2-d2+1))-d2), sizeof(para));
			memcpy(blkptr+sizeof(para)*(u2-(i%(u2-d2+1))-d2), \
					&tmp_para, sizeof(para));
			writeBlockToDisk(blkptr, flag, pn, &rs_buf);
		}
		else
		{
			blkptr = readBlockFromDisk(flag, pn, &rs_buf);
			blkep = readBlockFromDisk(flag, para.e1, &rs_buf);
			memcpy(&tmp_para, blkep+sizeof(para)*(para.e2-d2), sizeof(para));
			memcpy(blkep+sizeof(para)*(para.e2-d2), \
					blkptr+sizeof(para)*(u2-(i%(u2-d2+1))-d2), sizeof(para));
			memcpy(blkptr+sizeof(para)*(u2-(i%(u2-d2+1))-d2), \
					&tmp_para, sizeof(para));
			writeBlockToDisk(blkptr, flag, pn, &rs_buf);
			writeBlockToDisk(blkep, flag, para.e1, &rs_buf);
		}
	}

	Buffer d_buf;
	if(!initBuffer(64+1, 64, &d_buf))
		return 0;
	blkep = NULL;
	for(i = 0; i < rs_c; ++i)
	{
		pn = u1 - (i/(u2-d2+1));
		blkptr = readBlockFromDisk(flag, pn, &rs_buf);
		memcpy(&para, blkptr+sizeof(para)*(u2-(i%(u2-d2+1))-d2), sizeof(para));
		if((0 == i % 7) || (i == rs_c - 1))
		{
			if(blkep)
			{
				memset(blkep+sizeof(para)*7, 0, sizeof(para));
				if(i != rs_c - 1)
				{
					j = i/7+1;
					memcpy(blkep+sizeof(para)*7+4, &j, sizeof(int));
					writeBlockToDisk(blkep, tar_flag, i/7, &d_buf);
				}
				else
				{
					memcpy(blkep+sizeof(para)*(i%7), &para, sizeof(para));
					writeBlockToDisk(blkep, tar_flag, (i+1)/7, &d_buf);
					freeBlockInBuffer(blkptr, &rs_buf);
					break;
				}
			}
			blkep = getNewBlockInBuffer(&d_buf);
		}
		memcpy(blkep+sizeof(para)*(i%7), &para, sizeof(para));
		freeBlockInBuffer(blkptr, &rs_buf);
	}

	freeBuffer(&rs_buf);
	freeBuffer(&d_buf);
	
	for(i = d1; i <= u1; ++i)
		dropBlockOnDisk(flag, i);

	return 1;
}

#define add1(dlen, pcount, off, name) \
	memcpy(tar_ptr+dlen*(count%pcount), &name, dlen); \
	count++; \
	if(0 == count % pcount) \
	{ \
		memset(tar_ptr+dlen*pcount, 0, dlen); \
		unsigned int t = count / pcount + 1; \
		memcpy(tar_ptr+dlen*pcount + off, &t, sizeof(int)); \
		writeBlockToDisk(tar_ptr, tar_flag, count/pcount, &tar_buf); \
		tar_ptr = getNewBlockInBuffer(&tar_buf); \
	}

#define deal_last(dlen, pcount, name) \
	if(count % pcount) { \
		int ti = -1; \
		int * pname; \
		for(pname = (int *)(&name); pname < (int *)((&name)+1); ++pname) \
			memcpy(pname, &ti, sizeof(int)); \
		memcpy(tar_ptr+dlen*(count%pcount), &name, dlen); \
		memset(tar_ptr+60, 0, sizeof(int)); \
		writeBlockToDisk(tar_ptr, tar_flag, count/pcount+1, &tar_buf); \
	} \
	else if(count > 0) { \
		freeBlockInBuffer(tar_ptr, &tar_buf); \
		readBlockFromDisk(tar_flag, count/pcount, &tar_buf); \
		memset(tar_ptr+60, 0, sizeof(int)); \
		writeBlockToDisk(tar_ptr, tar_flag, count/pcount, &tar_buf); \
	} \

int linear_search(char * flag, unsigned int addr, int val, char * tar_flag)
{
	unsigned char * blkptr, * tar_ptr;
	Buffer d_buf, tar_buf;
	unsigned int next_addr;
	struct rs_ele para;
	int i, count;
	
	if((!initBuffer(64+1, 64, &d_buf)) || (!initBuffer(64+1, 64, &tar_buf)))
		return 0;

	next_addr = addr;
	count = 0;
	tar_ptr = getNewBlockInBuffer(&tar_buf);
	do
	{
		blkptr = readBlockFromDisk(flag, next_addr, &d_buf);
		memcpy(&next_addr, blkptr+sizeof(para)*7+4, sizeof(int));
		for(i = 0; i < 7; ++i)
		{
			memcpy(&para, blkptr+sizeof(para)*i, sizeof(para));
			if(para.e1 == val)
			{
				//add1 is not just a sentance, so should us '{' and '}'
				add1(8, 7, 4, para)
			}
		}
		freeBlockInBuffer(blkptr, &d_buf);
	}
	while(next_addr);

	deal_last(8, 7, para)
	
	freeBuffer(&d_buf);
	freeBuffer(&tar_buf);

	return 1;
}

int cmp(const void * a, const void * b)
{
	return ((((struct rs_ele *)a)->e1 > ((struct rs_ele *)b)->e1) || \
			((((struct rs_ele *)a)->e1 == ((struct rs_ele *)b)->e1) && \
			 (((struct rs_ele *)a)->e2 > ((struct rs_ele *)b)->e2))) ? 1 : 0;
}

int sort_relation(char * flag, unsigned int addr, char * tar_flag, int K)
{
	Buffer bufs[K], tar_buf;
	int i;
	unsigned char * blkptr;
	struct rs_ele para, min_para, max_para;

	for(i = 0; i < K; ++i)
	{
		if(!initBuffer((64+1)*K, 64, &bufs[i]))
			return 0;
	}
	
	if(!initBuffer(64+1, 64, &tar_buf))
		return 0;

	void * buf_starts[K+1];
	int ls[K], buf_pos[K+1], j;
	for(i = 0; i < K; ++i)
	{
		blkptr = readBlockFromDisk(flag, (unsigned int)(i+1), &bufs[0]);
		qsort(blkptr, 7, sizeof(para), cmp); //should not use the quick sort
		for(j = 0; j < 7; ++j)
			memcpy(&para, blkptr+sizeof(para)*j, sizeof(para));

		buf_starts[i] = blkptr;
		buf_pos[i] = 0;
		ls[i] = K;
	}
	min_para.e1 = -1; //stand for the minest element
	min_para.e2 = -1;
	max_para.e1 = 65535;
	max_para.e2 = 65535;
	buf_starts[K] = &min_para;
	buf_pos[K] = 0;

	int t, tmp;
	struct rs_ele * p1, * p2;
	for(i = K-1; i >= 0; --i) //build the tree
	{
		j = i;
		for(t = (j+K)/2; t > 0; t /= 2) //adjust(j)
		{
			p1 = (struct rs_ele *)(buf_starts[j]+sizeof(para)*buf_pos[j]); //the son
			p2 = (struct rs_ele *)(buf_starts[ls[t]]+sizeof(para)*buf_pos[ls[t]]); //the parent
			if((p1->e1 > p2->e1) || ((p1->e1 == p2->e1) && (p1->e2 > p2->e2)))
			{
				tmp = j;
				j = ls[t];
				ls[t] = tmp;
			}
		}
		ls[0] = j;
	}

	int count = 0;
	blkptr = getNewBlockInBuffer(&tar_buf);
	while(buf_starts[ls[0]] != &max_para) //k_merge, attention the condition
	{
		i = ls[0];
		//output a element to output_buffer
		memcpy(blkptr+sizeof(para)*(count%7), buf_starts[i]+sizeof(para)*buf_pos[i], sizeof(para));
		count++;
		if(0 == count % 7)
		{
			writeBlockToDisk(blkptr, tar_flag, count/7, &tar_buf);
			blkptr = getNewBlockInBuffer(&tar_buf); //there may be attention
		}
		buf_pos[i]++; //input a element from input_buffer
		if(buf_pos[i] >= 7) //
		{
			buf_starts[i] = &max_para;
			buf_pos[i] = 0;
		}
		j = i;
		for(t = (j+K)/2; t > 0; t /= 2) //adjust(j)
		{
			p1 = (struct rs_ele *)(buf_starts[j]+sizeof(para)*buf_pos[j]); //the son
			p2 = (struct rs_ele *)(buf_starts[ls[t]]+sizeof(para)*buf_pos[ls[t]]); //the parent
			if(p1->e1 > p2->e1 || ((p1->e1 == p2->e1) && (p1->e2 > p2->e2)))
			{
				tmp = j;
				j = ls[t];
				ls[t] = tmp;
			}
		}
		ls[0] = j;
	}

	//test the result
	Buffer test_buf;
	initBuffer(65, 64, &test_buf);
	printf("================= %s ========================\n", tar_flag);
	for(i = 1; i <= K; ++i)
	{
		blkptr = readBlockFromDisk(tar_flag, i, &test_buf);
		for(j = 0; j < 7; ++j)
		{
			memcpy(&para, blkptr+sizeof(para)*j, sizeof(para));
			printf(">>> %d %d\n", para.e1, para.e2);
		}
		freeBlockInBuffer(blkptr, &test_buf);
	}
	putchar('\n');

	for(i = 0; i < K; ++i)
		freeBuffer(&bufs[i]);
	freeBuffer(&tar_buf);
	
	return 1;
}

int binary_search(char * flag, int addr_count, char * tar_flag, int rs_c, int key)
{
	Buffer read_buf, tar_buf;
	int low = 0, up = rs_c - 1, mid, pn, pos, store_pos, tmpkey, spn;
	struct rs_ele para;
	unsigned char * blkptr, * tar_ptr, * eptr;
	int count = 0;

	if((!initBuffer(2*65, 64, &read_buf)) || (!initBuffer(65, 64, &tar_buf)))
		return 0;
	tar_ptr = getNewBlockInBuffer(&tar_buf);
	while(up > low-1)
	{
		mid = (low + up) >> 1;
		pn = mid / 7 + 1; //the number of the packet
		pos = mid % 7; //the offset in the packet
		blkptr = readBlockFromDisk(flag, pn, &read_buf);
		memcpy(&para, blkptr+sizeof(para)*pos, sizeof(para));
		tmpkey = para.e1;
		if(key > tmpkey)
			low = mid + 1;
		else if(key < tmpkey)
			up = mid - 1;
		else //find the target
		{
			store_pos = pos;
			eptr = blkptr;
			spn = pn;
			for(; pos < 7; ++pos) //the right part
			{
				memcpy(&para, eptr+sizeof(para)*pos, sizeof(para));
				if(para.e1 == key)
				{
					add1(8, 7, 4, para)
					if(pos == 6)
					{
						spn++;
						if(eptr != blkptr)
							freeBlockInBuffer(eptr, &read_buf);
						if(spn <= addr_count)
							eptr = readBlockFromDisk(flag, spn, &read_buf);
						else
							break;
						pos = -1; //pos will +1 in a moment, due to the 'for'
					}
				}
				else
				{
					if(eptr != blkptr)
						freeBlockInBuffer(eptr, &read_buf);
					break;
				}
			}
			eptr = blkptr;
			spn = pn;
			if(store_pos == 0)
				goto  here;
			for(pos = store_pos - 1; pos >= 0; --pos) //the left part
			{
				memcpy(&para, eptr+sizeof(para)*pos, sizeof(para));
				if(para.e1 == key)
				{
					add1(8, 7, 4, para)
					if(pos == 0)
					{
here:		            spn--;
						if(eptr != blkptr)
							freeBlockInBuffer(eptr, &read_buf);
						if(spn > 0)
							eptr = readBlockFromDisk(flag, spn, &read_buf);
						else
							break;
						pos = 7; //pos will -1 in a moment, due to the 'for'
					}
				}
				else
				{
					if(eptr != blkptr)
						freeBlockInBuffer(eptr, &read_buf);
					break;
				}

			}
			break;
		}
		freeBlockInBuffer(blkptr, &read_buf);
	}

	deal_last(8, 7, para)
	
	freeBuffer(&read_buf);
	freeBuffer(&tar_buf);

	return 1;
}

int display(char * flag, unsigned int addr, unsigned int a_len, int data_count)
{
	Buffer d_buf;
	unsigned int next_addr = addr;
	unsigned char * blkptr;
	void * a_data;
	int i, j, result = 0;

	a_data = malloc(a_len);
	initBuffer(65, 64, &d_buf);
	printf("============================ %s ===============================\n", flag);
	do
	{
		blkptr = readBlockFromDisk(flag, next_addr, &d_buf);
		if(!blkptr) //the file is not exist, that is has no record
			return 0;
		memcpy(&next_addr, blkptr+60, sizeof(int));
		for(i = 0; i < data_count; ++i)
		{
			memcpy(a_data, blkptr+a_len*i, a_len);
			if(-1 == *((int *)a_data))
				break;
			printf(">>> ");
			for(j = 0; j < a_len / sizeof(int); ++j)
				printf("%d ", *((int *)(a_data+sizeof(int)*j)));
			putchar('\n');
			result++;
		}
		freeBlockInBuffer(blkptr, &d_buf);
	}
	while(next_addr);

	free(a_data);
	freeBuffer(&d_buf);

	return result;
}

int projection(char * flag, int addr_count, char * tar_flag)
{
	Buffer d_buf, tar_buf;
	int i, j, new=-1, next, count = 0;
	unsigned char * blkptr, * tar_ptr;
	struct rs_ele para;

	if((!initBuffer(65, 64, &d_buf)) || (!initBuffer(65, 64, &tar_buf)))
		return 0;
	tar_ptr = getNewBlockInBuffer(&tar_buf);
	for(i = 1; i <= addr_count; ++i)
	{
		blkptr = readBlockFromDisk(flag, i, &d_buf);
		for(j = 0; j < 7; ++j)
		{
			memcpy(&para, blkptr+sizeof(para)*j, sizeof(para));
			next = para.e1;
			if(next != new)
			{
				new = next;
				add1(4, 15, 0, new);
				new = next;
			}
		}
		freeBlockInBuffer(blkptr, &d_buf);
	}
	
	deal_last(4, 15, new)
	
	freeBuffer(&d_buf);
	freeBuffer(&tar_buf);

	return 1;;
}

int nest_loop_join(char * rflag, char * sflag, char * tar_flag, int rc, int sc)
{
	Buffer rb, sb, tar_buf;
	unsigned char * rptr, * sptr, * rtmp, * stmp, * tar_ptr;
	int i, j, count = 0;
	struct rs_ele rpara, spara;
	struct new_ele new;

	if((!initBuffer(65, 64, &rb)) || (!initBuffer(65, 64, &sb)) || (!initBuffer(65, 64, &tar_buf)))
		return 0;
	
	tar_ptr = getNewBlockInBuffer(&tar_buf);
	for(i = 1; i <= rc; ++i)
	{
		rptr = readBlockFromDisk(rflag, i, &rb);
		for(j = 1; j <= sc; ++j)
		{
			sptr = readBlockFromDisk(sflag, j, &sb);
			for(rtmp = rptr; rtmp < rptr + sizeof(rpara) * 7; rtmp += sizeof(rpara))
			{
				memcpy(&rpara, rtmp, sizeof(rpara));
				for(stmp = sptr; stmp < sptr + sizeof(spara) * 7; stmp += sizeof(spara))
				{
					memcpy(&spara, stmp, sizeof(spara));
					if(rpara.e1 == spara.e1) //should write a record
					{
						memcpy(&new, &rpara, sizeof(rpara));
						memcpy((unsigned char *)((&new))+sizeof(rpara), &spara, sizeof(spara)); //attention the addtion of the pointer
						add1(16, 3, 12, new);
					}
				}
			}
			freeBlockInBuffer(sptr, &sb);
		}
		freeBlockInBuffer(rptr, &rb);
	}
	
	deal_last(16, 3, new)

	freeBuffer(&rb);
	freeBuffer(&sb);
	freeBuffer(&tar_buf);

	return 1;
}

#define get1(iorj, pre1, pre2) \
	if(0 == iorj%7) { \
		if(pre1##ptr) \
			freeBlockInBuffer(pre1##ptr, &pre2##b); \
		pre1##ptr = readBlockFromDisk(pre2##flag, iorj/7+1, &pre2##b); \
	} \
	memcpy(&pre2##para, pre1##ptr+sizeof(pre2##para)*(iorj%7), sizeof(pre2##para));

#define is_over(iorj) \
	iorj++;

int sort_merge_join(char * rflag, char * sflag, char * tar_flag, int rc, int sc)
{
	Buffer rb, sb, tar_buf;
	int i, j, rpn, spn, tmp_store, rt, st, count = 0;
	unsigned char * rptr = NULL, * sptr = NULL, * tar_ptr, * septr, * reptr;
	struct rs_ele rpara, spara;
	struct new_ele new;

	if((!initBuffer(2*65, 64, &rb)) || (!initBuffer(2*65, 64, &sb)) || \
			(!initBuffer(65, 64, &tar_buf)))
		return 0;

	i = j = 0;
	tar_ptr = getNewBlockInBuffer(&tar_buf); //remember...
	while(i < 7*rc && j < 7*sc)
	{
		reptr = septr = NULL;
		get1(i, r, r)
		i++;
		get1(j, s, s)
		j++;
		while(rpara.e1 != spara.e1 && !(i == 7*rc && rpara.e1 < spara.e1) && !(j == 7*sc && rpara.e1 > spara.e1))
		{
			while(rpara.e1 > spara.e1 && j < 7*sc)
			{
				get1(j, s, s)
				j++;
			}
			while(rpara.e1 < spara.e1 && i <7*rc)
			{
				get1(i, r, r)
				i++;
			}
		}
		if(rpara.e1 != spara.e1) //can't find this time
			break;
		rpn = i;
		spn = j;
		tmp_store = rpara.e1;
		while(tmp_store == rpara.e1 && i < 7*rc) //rpn-1 ~ i-2/1
		{
			get1(i, r, r)
			i++;
		}
		while(tmp_store == spara.e1 && j < 7*sc) //spn-1 ~ j-2/1
		{
			get1(j, s, s)
			j++;
		}
	
		if(rpara.e1 != tmp_store) i--;
		if(spara.e1 != tmp_store) j--;
		rt = rpn - 1;
		if(rt % 7)
			reptr = readBlockFromDisk(rflag, rt/7+1, &rb);
		for(; rt <= i-1; rt++)
		{
			get1(rt, re, r)
			st = spn - 1;
			if(st % 7)
				septr = readBlockFromDisk(sflag, st/7+1, &sb);
			for(; st <= j-1; st++)
			{
				get1(st, se, s)
				memcpy(&new, &rpara, sizeof(rpara));
				memcpy((unsigned char *)((&new))+sizeof(rpara), &spara, sizeof(spara));
				add1(16, 3, 12, new)
			}
			freeBlockInBuffer(septr, &sb);
		}
		freeBlockInBuffer(reptr, &rb);
	}

	deal_last(16, 3, new)
	
	freeBuffer(&rb);
	freeBuffer(&sb);
	freeBuffer(&tar_buf);

	return 1;
}

int hash_partition(char * flag, char * t_flag, int addr_count)
{
	Buffer buf, bufs[5], tar_buf;
	int counts[5] = {0}, i, j, count;
	unsigned char * blkptr, * tar_ptr, * tar_ptrs[5];
	struct rs_ele para;
	char tar_flag[20];

	if(!initBuffer(65, 64, &buf))
		return 0;
	for(i = 0; i < 5; ++i)
	{
		if(!initBuffer(65, 64, &bufs[i]))
			return 0;
		tar_ptrs[i] = getNewBlockInBuffer(&bufs[i]);
	}
	for(i = 1; i <= addr_count; ++i)
	{
		blkptr = readBlockFromDisk(flag, i, &buf);
		for(j = 0; j < 7; ++j)
		{
			memcpy(&para, blkptr+sizeof(para)*j, sizeof(para));
			memcpy(&tar_buf, &bufs[para.e1%5], sizeof(Buffer)); //hash is mod5
			sprintf(tar_flag, "%s%d", t_flag, para.e1%5);
			tar_ptr = tar_ptrs[para.e1%5];
			count = counts[para.e1%5];
			add1(8, 7, 4, para)
			tar_ptrs[para.e1%5] = tar_ptr;
			counts[para.e1%5] = count;
		}
		freeBlockInBuffer(blkptr, &buf);
	}

	for(i = 0; i < 5; ++i)
	{
		memcpy(&tar_buf, &bufs[i], sizeof(Buffer));
		sprintf(tar_flag, "%s%d", t_flag, i);
		tar_ptr = tar_ptrs[i];
		count = counts[i];
		deal_last(8, 7, para)
		freeBuffer(&bufs[i]);
	}

	freeBuffer(&buf);

	return 1;
}

int hash_join(char * rflag, char * sflag, char * tar_flag, int rc, int sc)
{
	Buffer rb, sb, tar_buf;
	unsigned char * tar_ptr, * rptr, * sptr;
	struct rs_ele rpara, spara;
	struct new_ele new;
	int count = 0, i, j, k;
	char f1[20], f2[20];
	unsigned int r_next, s_next;

	if((!initBuffer(65, 64, &rb)) || (!initBuffer(65, 64, &sb)) || (!initBuffer(65, 64, &tar_buf)))
		return 0;
	
	hash_partition(rflag, "hashR", 16); //generate the hash files
	hash_partition(sflag, "hashS", 32);
	
	tar_ptr = getNewBlockInBuffer(&tar_buf);
	for(i = 0; i < 5; ++i)
	{
		sprintf(f1, "hashR%d", i);
		sprintf(f2, "hashS%d", i);
		r_next = 1;
		do
		{
			rptr = readBlockFromDisk(f1, r_next, &rb);
			memcpy(&r_next, rptr+sizeof(rpara)*7+4, sizeof(int));
			for(j = 0; j < 7; ++j)
			{
				memcpy(&rpara, rptr+sizeof(rpara)*j, sizeof(rpara));
				if(rpara.e1 != -1 && rpara.e2 != -1)
				{
					s_next = 1;
					do
					{
						sptr = readBlockFromDisk(f2, s_next, &sb);
						memcpy(&s_next, sptr+sizeof(spara)*7+4, sizeof(int));
						for(k = 0; k < 7; ++k)
						{
							memcpy(&spara, sptr+sizeof(spara)*k, sizeof(spara));
							if(spara.e1 != -1 && spara.e2 != -1)
							{
								if(rpara.e1 == spara.e1) //should write a record
								{
									memcpy(&new, &rpara, sizeof(rpara));
									memcpy((unsigned char *)((&new))+sizeof(spara), &spara, sizeof(spara));
									add1(16, 3, 12, new);
								}
							}
							else
								break;
						}
						freeBlockInBuffer(sptr, &sb);
					}
					while(s_next);
				}
				else
					break;
			}
			freeBlockInBuffer(rptr, &rb);
		}
		while(r_next);
	}
	
	deal_last(16, 3, new)

	freeBuffer(&rb);
	freeBuffer(&sb);
	freeBuffer(&tar_buf);

	return 1;
}

int combine_op(char * rflag, char * sflag, char * tar_flag, int rc, int sc)
{
	Buffer rb, sb, tar_buf;
	int i, j, rpn, spn, tmp_store, rt, st, count = 0;
	unsigned char * rptr = NULL, * sptr = NULL, * tar_ptr, * septr, * reptr;
	struct rs_ele rpara, spara, tmp_para;

	if((!initBuffer(65, 64, &rb)) || (!initBuffer(65, 64, &sb)) || \
			(!initBuffer(65, 64, &tar_buf)))
		return 0;

	i = j = 0;
	tar_ptr = getNewBlockInBuffer(&tar_buf); //remember...
	while(i < 7*rc && j < 7*sc)
	{
		reptr = septr = NULL;
		get1(i, r, r)
		i++;
		get1(j, s, s)
		j++;
		if(rpara.e1 != spara.e1 || rpara.e2 != spara.e2) //the init time
		{
			add1(8, 7, 4, rpara)
			add1(8, 7, 4, spara)
		}
		else
		{
			add1(8, 7, 4, rpara)
		}
		while((rpara.e1 != spara.e1 || rpara.e2 != spara.e2) && \
				!(i == 7*rc && (rpara.e1 < spara.e1 || (rpara.e1 == spara.e1 && rpara.e2 < spara.e2))) && \
				!(j == 7*sc && (rpara.e1 > spara.e1 || (rpara.e1 == spara.e1 && rpara.e2 > spara.e2))))
		{
			while((rpara.e1 > spara.e1 || \
					(rpara.e1 == spara.e1 && rpara.e2 > spara.e2)) \
					&& j < 7*sc)
			{
				get1(j, s, s)
				j++;
				if(rpara.e1 != spara.e1 || rpara.e2 != spara.e2)
				{
					add1(8, 7, 4, spara)
				}
			}
			while((rpara.e1 < spara.e1 || \
					(rpara.e1 == spara.e1 && rpara.e2 < spara.e2)) \
					&& i < 7*rc)
			{
				get1(i, r, r)
				i++;
				if(rpara.e1 != spara.e1 || rpara.e2 != spara.e2)
				{
					add1(8, 7, 4, rpara)
				}
			}
		}
	}

	while(i < 7*rc) //attention
	{
		get1(i, r, r)
		i++;
		add1(8, 7, 4, rpara)
	}
	while(j < 7*sc)
	{
		get1(j, s, s)
		j++;
		add1(8, 7, 4, spara)
	}

	deal_last(8, 7, rpara)
	
	freeBuffer(&rb);
	freeBuffer(&sb);
	freeBuffer(&tar_buf);
	
	return 1;
}

int union_op(char * rflag, char * sflag, char * tar_flag, int rc, int sc)
{
	Buffer rb, sb, tar_buf;
	int i, j, rpn, spn, tmp_store, rt, st, count = 0;
	unsigned char * rptr = NULL, * sptr = NULL, * tar_ptr, * septr, * reptr;
	struct rs_ele rpara, spara;

	if((!initBuffer(65, 64, &rb)) || (!initBuffer(65, 64, &sb)) || \
			(!initBuffer(65, 64, &tar_buf)))
		return 0;

	i = j = 0;
	tar_ptr = getNewBlockInBuffer(&tar_buf); //remember...
	while(i < 7*rc && j < 7*sc)
	{
		reptr = septr = NULL;
		get1(i, r, r)
		i++;
		get1(j, s, s)
		j++;
		while((rpara.e1 != spara.e1 || rpara.e2 != spara.e2) && \
				!(i == 7*rc && (rpara.e1 < spara.e1 || (rpara.e1 == spara.e1 && rpara.e2 < spara.e2))) && \
				!(j == 7*sc && (rpara.e1 > spara.e1 || (rpara.e1 == spara.e1 && rpara.e2 > spara.e2))))
		{
			while((rpara.e1 > spara.e1 || \
					(rpara.e1 == spara.e1 && rpara.e2 > spara.e2)) \
					&& j < 7*sc)
			{
				get1(j, s, s)
				j++;
			}
			while((rpara.e1 < spara.e1 || \
					(rpara.e1 == spara.e1 && rpara.e2 < spara.e2)) \
					&& i < 7*rc)
			{
				get1(i, r, r)
				i++;
			}
		}
		if(rpara.e1 == spara.e1 && rpara.e2 == spara.e2)
		{
			add1(8, 7, 4, rpara)
		}
	}

	deal_last(8, 7, rpara)
	
	freeBuffer(&rb);
	freeBuffer(&sb);
	freeBuffer(&tar_buf);

	return 1;
}

int diff_op(char * rflag, char * sflag, char * tar_flag, int rc, int sc)
{
	Buffer rb, sb, tar_buf;
	int i, j, rpn, spn, tmp_store, rt, st, count = 0;
	unsigned char * rptr = NULL, * sptr = NULL, * tar_ptr, * septr, * reptr;
	struct rs_ele rpara, spara, tmp_para, old_rp;

	if((!initBuffer(65, 64, &rb)) || (!initBuffer(65, 64, &sb)) || \
			(!initBuffer(65, 64, &tar_buf)))
		return 0;

	i = j = 0;
	tar_ptr = getNewBlockInBuffer(&tar_buf); //remember...
	while(i < 7*rc && j < 7*sc)
	{
		reptr = septr = NULL;
		get1(i, r, r)
		i++;
		get1(j, s, s)
		j++;
		if(rpara.e1 != spara.e1 || rpara.e2 != spara.e2) //the init time
		{
			memcpy(&old_rp, &rpara, sizeof(rpara));
		}
		while((rpara.e1 != spara.e1 || rpara.e2 != spara.e2) && \
				!(i == 7*rc && (rpara.e1 < spara.e1 || (rpara.e1 == spara.e1 && rpara.e2 < spara.e2))) && \
				!(j == 7*sc && (rpara.e1 > spara.e1 || (rpara.e1 == spara.e1 && rpara.e2 > spara.e2))))
		{
			while((rpara.e1 > spara.e1 || \
					(rpara.e1 == spara.e1 && rpara.e2 > spara.e2)) \
					&& j < 7*sc)
			{
				get1(j, s, s)
				j++;
			}
			while((rpara.e1 < spara.e1 || \
					(rpara.e1 == spara.e1 && rpara.e2 < spara.e2)) \
					&& i < 7*rc)
			{
				get1(i, r, r)
				i++;
				add1(8, 7, 4, old_rp)
				if(rpara.e1 != spara.e1 || rpara.e2 != spara.e2)
					memcpy(&old_rp, &rpara, sizeof(rpara));
			}
		}
	}

	if(rpara.e1 != spara.e1 || rpara.e2 != spara.e2)
		add1(8, 7, 4, old_rp)
	while(i < 7*rc) //attention
	{
		get1(i, r, r)
		i++;
		add1(8, 7, 4, rpara)
	}

	deal_last(8, 7, rpara)
	
	freeBuffer(&rb);
	freeBuffer(&sb);
	freeBuffer(&tar_buf);
	
	return 1;
}

int main(int argc, char **argv)
{
	int lr, ls, br, bs, pr, ps, nlj, smj, hj, com, uni, dif;

	srand((unsigned int)time(NULL));

	gen_core(1, 40, 1, 1000, "randR", R_C, "rawR");
	gen_core(20, 60, 1, 1000, "randS", S_C, "rawS");

	sort_relation("rawR", 1, "sortR", 16);
	sort_relation("rawS", 1, "sortS", 32);

	linear_search("rawR", 1, 40, "linearR");
	linear_search("rawS", 1, 60, "linearS");

	lr = display("linearR", 1, 8, 7);
	ls = display("linearS", 1, 8, 7);

	binary_search("sortR", 16, "binR", R_C, 40);
	binary_search("sortS", 32, "binS", S_C, 60);

	br = display("binR", 1, 8, 7);
	bs = display("binS", 1, 8, 7);

	projection("sortR", 16, "proR");
	projection("sortS", 32, "proS");

	pr = display("proR", 1, 4, 15);
	ps = display("proS", 1, 4, 15);

	nest_loop_join("rawR", "rawS", "nljRS", 16, 32);
	nlj = display("nljRS", 1, 16, 3);

	sort_merge_join("sortR", "sortS", "smjRS", 16, 32);
	smj = display("smjRS", 1, 16, 3);

	hash_join("rawR", "rawS", "hjRS", 16, 32);
	hj = display("hjRS", 1, 16, 3);
	
	combine_op("sortR", "sortS", "combineRS", 16, 32);
	com = display("combineRS", 1, 8, 7);

	union_op("sortR", "sortS", "unionRS", 16, 32);
	uni = display("unionRS", 1, 8, 7);

	diff_op("sortR", "sortS", "diffRS", 16, 32);
	dif = display("diffRS", 1, 8, 7);

	printf("\nResult: \n\tlinearR: %d\n\tlinearS: %d\n\tbinR: %d\n\tbinS: %d\n\tproR: %d\n\tproS: %d\n\tnlj: %d\n\tsmj: %d\n\thj: %d\n\tcom: %d\n\tuni: %d\n\tdif:%d\n", lr, ls, br, bs, pr, ps, nlj, smj, hj, com, uni, dif);

    return 0;
}

