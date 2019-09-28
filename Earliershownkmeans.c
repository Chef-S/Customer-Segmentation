#include<stdio.h>
int main()
{
    int i1,i2,i3,t1,t2;

    int k0[10];
    int k1[10];
    int k2[10];

    printf("\nEnter sales data of 10 customers:\n");
    printf("\nEnter number of Gel pens bought in a year:\n");
    for(i1=0;i1<10;i1++)
    {
        printf("\nEnter for customer %d \n",i1);
		scanf("%d",&k0[i1]);

    }


    //initial means
    int m1;
    int m2;

    printf("\n Enter initial mean 1:");
    scanf("%d",&m1);
    printf("\n Enter initial mean 2:");
   scanf("%d",&m2);

    int om1,om2;    //old means

    do
    {

    //saving old means
    om1=m1;
    om2=m2;

    //creating clusters
    i1=i2=i3=0;
    for(i1=0;i1<10;i1++)
    {
        //calculating distance to means
        t1=k0[i1]-m1;
        if(t1<0){t1=-t1;}

        t2=k0[i1]-m2;
        if(t2<0){t2=-t2;}

        if(t1<t2)
        {
            //near to first mean
            k1[i2]=k0[i1];
            i2++;
        }
        else
        {
            //near to second mean

            k2[i3]=k0[i1];
            i3++;
        }

    }

    t2=0;
    //calculating new mean
    for(t1=0;t1<i2;t1++)
    {
        t2=t2+k1[t1];
    }
    m1=t2/i2;

    t2=0;
    for(t1=0;t1<i3;t1++)
    {
        t2=t2+k2[t1];
    }
    m2=t2/i3;

    //printing clusters
    printf("\nCluster 1:");
    for(t1=0;t1<i2;t1++)
    {    printf("\t");
         printf("%d",k1[t1]);
    }
     printf("\nm1=%d",m1);

    printf("\nCluster 2:");
    for(t1=0;t1<i3;t1++)
    {    printf("\t");
        printf("%d",k2[t1]);
    }
    printf("\nm2=%d",m2);

    printf("\n ----");
    }while(m1!=om1&&m2!=om2);

    printf("\n Clusters created");

    //ending
}
