#include<stdio.h>
#include<math.h>


int main(){
int i,i1,i2, centroid1, centroid2, k=2,j, fix=0,i3;
int data_set[10];
char product[20], cluster1[10], cluster2[10];

printf("\n");
printf("**************************************************************Welcome to Customer Segmentation Application**************************************************************");

//User is asked for the name of the product
printf("\nEnter the Name of the product\n");
gets(product);

	//user is asked for the number of products bought by 10 customers

    for(i1=0;i1<10;i1++)
    {

		printf("\nEnter number of %s bought by customer %d \n", &product, i+1);
	    scanf("%d",&data_set[i1]);

    }

 //Ask user for intital centroids
printf("\n Enter initial centroid1\t");
       scanf("%d",&centroid1);
printf("\n Enter initial centroid2\t");
       scanf("%d",&centroid2);





 //descion made as to which cluster this data will go to
for(i=0;i<10;i++)
  {
     if( (  abs(centroid1-data_set[i])  ) <= ( abs( centroid2-data_set[i]) ) )
      { j=i;
        cluster1[j]=data_set[i];
      }

     else{
     	
        cluster2[fix]=data_set[i];
        fix++;
         }

  }
  
  

  
  
  
  
printf("\n");
printf("Type 1 customers in cluster 1\t");
printf("\n");

for(i2=0;i2<=j;i2++){
printf("%d \t",cluster1[i2]);
}

printf("\n");
printf("\n");

printf("Type 2 customers in cluster 2\t");


printf("\n");


for(i3=0;i3<=fix-1;i3++){
printf("%d \t",cluster2[i3]);
}

printf("\n");
printf("\n");

return 0;

}
