#include <stdio.h>

int global_init_var = 84;
int global_uninit_var;
char * str1 = "hello world!";

void func1(int i)
{
	printf("%d\n", i);
}

int main(void)
{
	static int static_var = 85;
	static int static_var2;
	char * str2 = "22222";

	int a = 1;
	int b;

	func1(static_var+static_var2+a+b);

	return a;
}
