/**
 *
 *
*/

#include <math.h>
#include <stdio.h>

#define G_PI    3.14159265358979323846E0

/*求两个数之间的最大值*/
int max(a,b)
{
    if(a>=b)
       return (a);
    else
       return (b);
}

/*求最大公约数*/
int zdgys(int a, int b)
{
    int m=0;
    for(m=max(a,b);m>0;m--)
        if((a%m==0)&&b%m==0)
            break;
    return (m);
}


/*
 * 打印sinx的值，x的定义域:1/128-90/128度
 **/
int main()
{
    int i=0;
    int r = 0;
    double d = 0;
    int gys = 1;

    for(i=1;i<=128;i++)
    {
        d = sin(90*i/128*G_PI/180);
        r = d*10000;
        gys = zdgys(r,10000);
        printf("{%d,%d},",r/gys,10000/gys);
    }
    printf("\n");

/* 
    int a=18;
    int b=24;
    printf("%d and %d =:%d\n",a,b,zdgys(a,b));
*/
    return 0;
}
