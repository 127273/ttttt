#include<stdio.h>
void size(char *p)
{
	int a=sizeof(p);
	printf("%d",a);
}

void ss(int &a, int &b)
{
	printf("%d",a);
}
int main()
{
		printf("hello world");
		char k;
		size(k);
		int a=3,b;
	//	ss(a,b);
		char z[2]={'%s','s'};
		printf(z[0]);
		printf("ss");
}
