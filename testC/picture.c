#include<stdio.h>
int main()
{
	FILE *fp=NULL ;
	fp = open("1.dat","r");
	char data[2014];
	fread(fp,data);
}
