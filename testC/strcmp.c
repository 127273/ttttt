#include<stdio.h>
int strcmpg(char *a, char *b)
{
	int i = 0;
	while(a[i] && b[i] && a[i] == b[i])i++;
	if(a[i] || b[i]) return 1;
	return 0;
}
int main()
{
	char *a="as",*b="ass";
	char c[20];
	c[0]='a';
	c[1]='s';
	c[2]='\0';
	int result = strcmpg(a,c);
//	if(result) 
	printf("%d",result);
}
