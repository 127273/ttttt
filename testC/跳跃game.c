#include<stdio.h>
struct Skip{
    unsigned int skipPos;
    unsigned int skipNum;
};
/*
int array_length( unsigned int a[])
{
    int i = 0;
    while(a[i]) {i++;printf("%d", a[i]);}
    return i;
}*/
int main()
{
    Skip skip;

    const unsigned int length = 7;// array_length(a);
   
    unsigned int a[length + 1] = {2, 3, 2, 2, 1, 2, 3, 4};

   
  
    skip.skipPos = length ;
    skip.skipNum = 0;

   
    int i = skip.skipPos, j = 0, temp = 0;
    while( i != 0)
    {
        for(j = i - 1; j >= 0; j--)
        {
            if(skip.skipPos - j <= a[j])
                temp = j;
        }
        skip.skipPos = temp;
        skip.skipNum++;
        i = temp; // printf("what");
        printf("%d", temp);
    }
    printf("what");
    printf("ok %d",skip.skipNum);
    //getchar();
//    system("PAUSE");
}
