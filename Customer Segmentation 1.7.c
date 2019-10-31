#include<stdio.h>

#include<math.h>

int main(){
int i,i1,i2, centroid1, centroid2,temp1, temp2,i3,prev_centroid1,prev_centroid2;
int data_set[10];
char product[20];
int cluster1[10], cluster2[10];

printf("\n");
printf("**************************************************************Welcome to Customer Segmentation Application**************************************************************");

printf("\nEnter the Name of the product\n");
gets(product);



    for(i=0;i<10;i++)
    {

		printf("\nEnter number of %s bought by customer %d \n", &product, i+1);
	    scanf("%d",&data_set[i]);

    }


printf("\n Enter initial centroid1\t");
       scanf("%d",&centroid1);
printf("\n Enter initial centroid2\t");
       scanf("%d",&centroid2);
  
do{
   i=i2=i3=0;

prev_centroid1= centroid1;
prev_centroid2= centroid2;


 for(i1=0;i1<10;i1++)
    {
        
        temp1=data_set[i1]-centroid1;
        if(temp1<0){temp1=-temp1;}
        
        temp2=data_set[i1]-centroid2;
        if(temp2<0){temp2=-temp2;}

        if(temp1<temp2)
        {
            cluster1[i2]=data_set[i1];
            i2++;
        }
        else
        {
           cluster2[i3]=data_set[i1];
            i3++;
        }

    }

  
    temp2=0;
   
   
    for(temp1=0;temp1<i2;temp1++)
    {
        temp2=temp2+cluster1[temp1];
    }
    centroid1=temp2/i2;

    temp2=0;
   
    for(temp1=0;temp1<i3;temp1++)
    {
        temp2=temp2+cluster2[temp1];
    }
    centroid2=temp2/i3;

  
printf("\n");
printf("Type 1 customers in cluster 1\t");
printf("\n");

 for(temp1=0;temp1<i2;temp1++)
    {    printf("\t");
         printf("%d",cluster1[temp1]);
    }
     printf("\n");
	 printf("\tCentroid 1=%d",centroid1);


printf("\n");
printf("\n");

printf("Type 2 customers in cluster 2\t");


printf("\n");


 for(temp1=0;temp1<i3;temp1++)
    {    printf("\t");
        printf("%d",cluster2[temp1]);
    }

printf("\n");    printf("\tcentroid 2=%d",centroid2);
printf("\n");
printf("\n");

}while(centroid1!=prev_centroid1&&centroid2!=prev_centroid2);

return 0;

}
