#ifndef TOKEN_DEFINE
	#define TOKEN_DEFINE
	#define _token_type int
	struct _token
	{
		_token_type type;
		void* _point; //it can be id* or string*
	};
#endif

#ifndef BUFFER_DEFINE
	#define BUFFER_DEFINE
	#define _byte char
	#define _scan_pos int
	#define half_size 4096
#endif

#ifndef STACK_DEFINE
	#define STACK_DEFINE
	typedef struct node
	{
		void * cont;
		struct node * next;
	} stack_node;
	typedef struct
	{
		stack_node * head;
		int size;
	} stack;
	extern void init_stack(stack * st, int si);
	extern int is_empty(stack * st);
	extern void push(stack * st, const void * puc);
	extern int pop(stack * st, void * poc);
	extern int get_head(stack * st, void * gec);
	extern int get_val(stack * st, int no, void * gec);
	extern void destroy_stack(stack * st);
#endif

#ifndef ID_STRUCT
	#define ID_STRUCT
	typedef struct idnode //just store the name of the symbol during the lexical analysis
	{
		char * name;
		//int reg; //if use the multi_table this can ignore
		//int type;
		//int offset;
		struct idnode * next_hash;
	} id;
	#define BUCKETS 211
	extern id * symbol_table[BUCKETS];
	typedef struct idn
	{
		char * name;
		int type;
		int offset;
		void * pt; //that may be used in future
		struct idn * next_id;
	} id_no;
	typedef struct procn
	{
		char * name;
		void * proc_pt; //there should be attention
		struct procn * next_proc;
	} proc_no;
	typedef struct tmpn //equal to the symbol that has not name
	{
		int type;
		int offset;
		void * pt;
		struct tmpn * next_tmp;
	} tmp_struct;
	typedef struct stab_str
	{
		int width;
		int tmp_width;
		struct stab_str * previous;
		id_no * tail; //store the symbols
		tmp_struct * tail_tmp;
		proc_no * procs; //store the entrance of the procedures' sym_tab
	} sym_tab;
	typedef struct dimlen
	{
		int len;
		struct dimlen * next_len;
	} dim_len;
	typedef struct dropvec
	{
		int dims;
		int width; //the width of the type
		dim_len * head_dim;
		struct dropvec * next_vec;
	} drop_vector;
	extern sym_tab * global_tab; //the global sym_tab
	extern drop_vector * all_vector;
#endif

#ifndef STRING_STRUCT
	#define STRING_STRUCT
	typedef struct stringnode
	{
		char * str;
		struct stringnode * next_str;
	} string;
	#define STRING_BUCKETS 211
	extern string * string_table[STRING_BUCKETS];
#endif

#ifndef INT_STRUCT
	#define INT_STRUCT
	typedef struct int_node
	{
		int val;
		struct int_node * next_int;
	} in;
	extern in * int_table[10];
#endif

#ifndef REAL_STRUCT
	#define REAL_STRUCT
	typedef struct real_node
	{
		float val;
		struct real_node * next_real;
	} rn;
	extern rn * real_table;
#endif

#ifndef PROD_STAT_DEFINE
	#define PROD_STAT_DEFINE
	#define maxs 100 //the max counter of the symbols
	typedef struct pro_n
	{
		int var; //the value of variable
		int pv[50]; //save the value of production
		int vl; //len of production
	} pro;

	#define max_pro 200 //the max counter of productions

	typedef struct j_pro_n
	{
		int np; //the NO of production
		int pp; //the position of point '.'
		int exp_sb[50]; //the expect symbol
		int exp_len; //the counter of expect symbol
		struct j_pro_n * nj; //the next j_pro_n
	} jpro;

	typedef jpro* state; //the production in closure

	#define max_stat 2000 //the max counter of states
#endif

#ifndef SCAN_DEBUG
	#define SCAN_DEBUG 1
#endif

#if 0  //1 stand for close the output in screen
	#define lr1_parli_DEBUG(info) /*info*/
#else // 0 stand for open the output in screen
	#define lr1_parli_DEBUG(info) info
#endif

#if 0
	#define lr1_DEBUG(info) /*info*/
#else
	#define lr1_DEBUG(info) info
#endif
	
#if 1
	#define lr1_status_DEBUG(info) /*info*/
#else
	#define lr1_status_DEBUG(info) info
#endif

#define lr1_DEBUG2 1
