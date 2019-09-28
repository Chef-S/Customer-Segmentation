#include<stdio.h>
#include<math.h>


int main(){
int i,i1,i2,i3, centroid1, centroid2, k=2;
int data_set[10];
char product[20], cluster1[10], cluster2[10];

printf("\n");
printf("**************************************************************Welcome to Customer Segmentation Application*****************************************************************");

//User is asked for the name of the product
printf("\nEnter the Name of the product\n");
gets(product);

	//user is asked for the number of products bought by 10 customers

    for(i=1;i<=10;i++)
    {

		printf("\nEnter number of %s bought by customer %d \n", &product, i);
	    scanf("%d",&data_set[i]);

    }

 //Ask user for intital centroids
printf("\n Enter initial centroid1 \t");
scanf("%d",&centroid1);
printf("\n Enter initial centroid2 \t");
scanf("%d",&centroid2);


 //descion made as to which cluster this data will go to
for(i1=0;i1<10;i1++){
     if( (  abs(centroid1-data_set[i1])  ) < ( abs( centroid2-data_set[i1]) ) )
      {
        cluster1[i1]=data_set[i1];
      }

     else{
        cluster2[i1]=data_set[i1];
         }

  }

printf("Type 1 customers in cluster 1");

for(i2=0;i2<10;i2++){
printf("%d \t",cluster1[i2]);
}

printf("\n");


printf("Type 2 customers in cluster 2");


printf("\n");


for(i3=0;i3<10;i3++){
printf("%d \t",cluster2[i3]);
}

return 0;

}
