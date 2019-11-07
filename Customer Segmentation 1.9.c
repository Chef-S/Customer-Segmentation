#include<stdio.h>
#include<math.h>

int main(){
int i,i1,i2,i3,i4, centroid1, centroid2,centroid3,temp1, temp2,temp3,prev_centroid1,prev_centroid2,prev_centroid3,f1,f2;
int data_set[10];
char product[20];
int cluster1[10], cluster2[10],cluster3[10];

printf("\n");
printf("**************************************************************Welcome to Customer Segmentation Application**************************************************************");

//User is asked for the name of the product
printf("\nEnter the Name of the product\n");
gets(product);
printf("\n");
printf("\n");
printf("Sales Strategies for selling of %s is to divide the customer base into 3 groups on basis of no of products bought and taget market the three groups seperately",product);
printf("\n");
printf("\n");
/*  Initial version of the program collected data from the user , calculated and discarded values.
	//user is asked for the number of products bought by 10 customers

    for(i=0;i<10;i++)
    {

		printf("\nEnter number of %s bought by customer %d \n", &product, i+1);
	    scanf("%d",&data_set[i]);

    }*/

//writing to a file
//writing mode can be changed to append mode when software given to the client
printf("\n");
printf("\n");
     FILE *fptr1;
     fptr1 = fopen("customer_data.txt","w");
     
     if(fptr1 == NULL)
     {
     printf("Error!"); 
     }
     
     for(f1=0;f1<10;f1++){
     printf("Enter Number of products bought by customer %d :",f1+1);
     scanf("%d",&data_set[f1]);
     
     fprintf(fptr1,"%d",data_set[f1]); 
     fprintf(fptr1," "," "); 
    }
    
	fclose(fptr1); 
printf("\n");
printf("Customer data entered into the file");
printf("\n");
//till here writing

//reading from the file
 FILE *fptr2;
    fptr2 = fopen("customer_data.txt", "r");

    //read file into array
   
     if (fptr2 == NULL){
        printf("Error Reading File\n");
       
    }

    for (f2 = 0; f2 < 10; f2++){
    fscanf(fptr2, "%d,", &data_set[f2] );
    }
    
	printf("\n");         
    
	for (f2 = 0; f2< 10; f2++){
        printf("Number of product bought by customer %d is : %d\n\n",f2+1 , data_set[f2]);
    }

    fclose(fptr2);
printf("\n");  
printf("Customer data fetched from the file");
printf("\n");
//instead of collecting data from user, program can be cutomised to fetch from files created by client.
//till here reading
 
 
//Ask user for intital centroids

printf("\n Enter initial centroid1\t");
       scanf("%d",&centroid1);
printf("\n Enter initial centroid2\t");
       scanf("%d",&centroid2);
printf("\n Enter initial centroid3\t");
       scanf("%d",&centroid3);
  
do{
   i=i2=i3=i4=0;

//saving previous mean so we can update centroid later

prev_centroid1= centroid1;
prev_centroid2= centroid2;
prev_centroid3= centroid3;


//decision made as to which cluster this data will go to
 for(i1=0;i1<10;i1++)
    {
        
        temp1=data_set[i1]-centroid1;
        if(temp1<0){
		temp1=-temp1;
		}
        
        temp2=data_set[i1]-centroid2;
        if(temp2<0){
		temp2=-temp2;
		}

        temp3=data_set[i1]-centroid3;
        if(temp3<0){
		temp3=-temp3;
		}
       
	   //calculating distance to means
	    if(temp1<=temp2&&temp1<=temp3)
        {
            //near to first mean
            cluster1[i2]=data_set[i1];
            i2++;
        }
        else if(temp2<temp1&&temp2<=temp3)
        {
            //near to second mean

            cluster2[i3]=data_set[i1];
            i3++;
        }
       else 
        {
            //near to third mean

            cluster3[i4]=data_set[i1];
            i4++;
        }


    }

  
  
//Updating centroids by calculating mean 
     
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
    
    temp2=0;
    for(temp1=0;temp1<i4;temp1++)
    {
        temp2=temp2+cluster3[temp1];
    }
    centroid3=temp2/i4;    

//Printing clusters 
  
printf("\n");
printf("Type 1 customers in cluster 1\t");
printf("\n");

 for(temp1=0;temp1<i2;temp1++)
    {    printf("\t");
         printf("%d",cluster1[temp1]);
    }
     printf("\tCentroid 1=%d",centroid1);


printf("\n");
printf("\n");

printf("Type 2 customers in cluster 2\t");


printf("\n");


 for(temp1=0;temp1<i3;temp1++)
    {    printf("\t");
        printf("%d",cluster2[temp1]);
    }
    printf("\tcentroid 2=%d",centroid2);
printf("\n");
printf("\n");

printf("Type 3 customers in cluster 3\t");


printf("\n");


 for(temp1=0;temp1<i4;temp1++)
    {    printf("\t");
        printf("%d",cluster3[temp1]);
        printf("\t");
    }
    printf("\tcentroid 3=%d",centroid3);
printf("\n");
printf("\n");

// Loop terminates after the next calculated mean is equal to the previous mean
}
while( (centroid1!=prev_centroid1) || (centroid2!=prev_centroid2) || (centroid3!=prev_centroid3) );

return 0;

}
