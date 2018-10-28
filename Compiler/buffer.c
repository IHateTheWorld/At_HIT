#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "global_define.h"

_byte scan_buf[half_size*2+2];
_scan_pos lexeme_beginning, forward;

void retract(int n)
{
	forward -= n;
}

void init_buf()
{
	memset(scan_buf, 0, sizeof(scan_buf)); //0
	scan_buf[half_size] = EOF;
	scan_buf[half_size*2+1] = EOF;
}

void get_half_buf(FILE* source_fp, int left_right) //0->left 1->right
{
	int read_bytes;

	read_bytes = fread(scan_buf+left_right*(half_size+1), 1, half_size, source_fp);
	if(read_bytes < half_size)
	{
		scan_buf[left_right*(half_size+1)+read_bytes] = EOF;
	}
}

char forward_a_pace(FILE* source_fp)
{
	char ch;

	ch = scan_buf[forward];

	if(ch == EOF) //only judge once
	{
		if(forward == half_size)
		{
			get_half_buf(source_fp, 1); //the right of buffer
			ch = scan_buf[++forward];
		}
		else if(forward == half_size*2 + 1)
		{
			get_half_buf(source_fp, 0); //the left of buffer
			forward = 0; // change to the beginning of buffer
			ch = scan_buf[forward];
		}
		else
			ch = EOF;
	}

	forward++;

	return ch;
}

void copytoken(char* token)
{
	if(lexeme_beginning < forward)
	{
		if(forward <= half_size || lexeme_beginning > half_size)
		{
			memcpy(token, scan_buf+lexeme_beginning, forward-lexeme_beginning);
			token[forward-lexeme_beginning] = '\0';
		}
		else
		{
			memcpy(token, scan_buf+lexeme_beginning, half_size-lexeme_beginning);
			memcpy(token+(half_size-lexeme_beginning), scan_buf+half_size+1, forward-half_size-1);
			token[forward-lexeme_beginning-1] = '\0'; //the eof in the middle
		}
	}
	else
	{
		if(lexeme_beginning < half_size)
		{
			memcpy(token, scan_buf+lexeme_beginning, half_size-lexeme_beginning);
			memcpy(token+(half_size-lexeme_beginning), scan_buf+half_size+1, half_size);
			memcpy(token+(2*half_size-lexeme_beginning), scan_buf, forward);
			token[2*half_size-lexeme_beginning+forward] = '\0';
		}
		else
		{
			memcpy(token, scan_buf+lexeme_beginning, 2*half_size+1-lexeme_beginning);
			if(forward < half_size)
			{
				memcpy(token+2*half_size+1-lexeme_beginning, scan_buf, forward);
				token[2*half_size+1-lexeme_beginning+forward] = '\0';
			}
			else
			{
				memcpy(token+2*half_size+1-lexeme_beginning, scan_buf, half_size);
				memcpy(token+3*half_size+1-lexeme_beginning, scan_buf+half_size+1, forward-half_size-1);
				token[2*half_size-lexeme_beginning+forward] = '\0';
			}
		}
	}
}
