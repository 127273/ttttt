#include<stdarg.h>
#include<stdio.h>
void my_putchar(char *str,...)
{
		va_list ap;
		va_start(ap,str);
		int i = 0;
		int j = 0; 
		char *p=str;
	// get % 
		//while(str++)putchar('x');
		//if(str[4] == '\0')
		while(str[i])
		{
			if(str[i] == '%' && str[i+1])
			{
				// %d 
				int d;
				char s;
				i++;
				switch(str[i])
				{
					case 'd':
					d=va_arg(ap,int);
					d+=48;
					putchar(d);				             	                                                                                    
					default:;

				}
				
			}
					
			else
				putchar(str[i]);
			i++;
		}
		putchar('z');
	
		va_end(ap);
			
			
} 
int main()
{
	int c=5;

	my_putchar("Îa%d%s", c, s);

	putchar('e');
	while(1);
}
