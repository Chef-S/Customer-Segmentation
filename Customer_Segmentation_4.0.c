#include<stdio.h>
#include<math.h>
#include <stdlib.h> 
#include<strings.h>
int main(){

int i,i1,i2,i3,i4, centroid1, centroid2,centroid3,temp1, temp2,temp3,prev_centroid1,prev_centroid2,prev_centroid3,f1,f2;
FILE *fptr1;
FILE *fptr2;
int data_set[10];
char product[20];
int cluster1[10], cluster2[10],cluster3[10];
int choice,choose_algo;
while(1){
printf("\n");
printf("\n");
printf("********************************************Welcome to Clustering Application, Which Algorithm would you like to work with?*********************************************");
printf("\n");
printf("\n");
printf("Enter 1 : for Kmeans Clustering\nEnter 2 : for Hierarchical Clustering\nEnter 3 : for DBSCAN\nEnter 0 : to exit application");	
printf("\n");
scanf("%d",&choose_algo);
//choose_algo switch case
switch(choose_algo){


case 1:

  while(1){

        printf("***************************************Welcome to Kmeans clustering Application, How many clusters would you like to work with?********************************");
        printf("\n");
        printf("\n");
        printf("Enter 2 : for 2 Clusters\nEnter 3 : for 3 Cluster\nEnter 0 : to exit kmeans clustering application and go to main menue ");	
        printf("\n");

        scanf("%d",&choice);

        //K means switch case
        switch(choice){

            case 2:

              /*int i,i1,i2, centroid1, centroid2,temp1, temp2,i3,prev_centroid1,prev_centroid2,f1,f2;
              int data_set[10];
              char product[20];
              int cluster1[10], cluster2[10];*/

              printf("\n");
              printf("**************************************************************Welcome to Customer Segmentation Application**************************************************************");
              printf("\n");
              //User is asked for the name of the product
              printf("\nEnter the Name of the product(20 Characters only)\n");
              scanf("%s",&product);
               //gets(product);
              printf("\n");
              printf("Sales Strategies for selling of %s is to divide the customer base into 2 groups on basis of no of products bought and taget market the two groups seperately",product);
               
			   /*
	           //user is asked for the number of products bought by 10 customers
               for(i=0;i<10;i++)
              {
               printf("\nEnter number of %s bought by customer %d \n", &product, i+1);
       	       scanf("%d",&data_set[i]);
			  }*/

              //writing to a file
              printf("\n");
              printf("\n");
              //FILE *fptr1;
              fptr1 = fopen("customer_data.txt","w");
     
              if(fptr1 == NULL)
              {
              printf("Error!"); 
              }
     
              for(f1=0;f1<10;f1++){
              printf("Enter Number of %s bought by customer %d :",product,f1+1);
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
              //FILE *fptr2;
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
              printf("Number of %s bought by customer %d is : %d\n\n",product, f2+1 , data_set[f2]);
              }

              fclose(fptr2);

 
              printf("Customer data fetched from the file");
              printf("\n");
              //till here reading
 
 
              //Ask user for intital centroids
              printf("\n Enter initial centroid1\t");
              scanf("%d",&centroid1);
              printf("\n Enter initial centroid2\t");
              scanf("%d",&centroid2);
  
              do{
              i=i2=i3=0;
              //saving previous mean so we can update centroid later

              prev_centroid1= centroid1;
              prev_centroid2= centroid2;

              //decision made as to which cluster this data will go to
              for(i1=0;i1<10;i1++)
              {
              //calculating distance to means
              temp1=data_set[i1]-centroid1;
              if(temp1<0){temp1=-temp1;}
         
              temp2=data_set[i1]-centroid2;
              if(temp2<0){temp2=-temp2;}

              if(temp1<temp2)
              {
              //near to first mean
              cluster1[i2]=data_set[i1];
              i2++;
              }
              else
              {
               //near to second mean

               cluster2[i3]=data_set[i1];
               i3++;
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

                //Printing clusters 
  
                printf("\n");
                printf("Type 1 customers in cluster 1\t");
                printf("\n");

                for(temp1=0;temp1<i2;temp1++)
                {   
				printf("\t");
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

                // Loop terminates after the next calculated mean is equal to the previous mean
                }

                while(centroid1!=prev_centroid1&&centroid2!=prev_centroid2);
                break;

         case 3:

               /*int i,i1,i2,i3,i4, centroid1, centroid2,centroid3,temp1, temp2,temp3,prev_centroid1,prev_centroid2,prev_centroid3,f1,f2;
               int data_set[10];
               char product[20];
               int cluster1[10], cluster2[10],cluster3[10];*/

               printf("\n");
               printf("**************************************************************Welcome to Customer Segmentation Application**************************************************************");

               //User is asked for the name of the product
               printf("\nEnter the Name of the product(20 Characters only)\n");
               scanf("%s",&product);
               printf("\n"); 
               printf("\n");
               printf("Sales Strategies for selling of %s is to divide the customer base into 3 groups on basis of no of products bought and taget market the three groups seperately",product);
               printf("\n");
               printf("\n");
               /*  Initial version of the program collected data from the user , calculated and discarded values.
	           // user is asked for the number of products bought by 10 customers

               for(i=0;i<10;i++) 
               {

		       printf("\nEnter number of %s bought by customer %d \n", &product, i+1);
	           scanf("%d",&data_set[i]);

               }*/

               //writing to a file
               //writing mode can be changed to append mode when software given to the client
               printf("\n");
               printf("\n");
               //  FILE *fptr1;
               fptr1 = fopen("customer_data.txt","w");
     
               if(fptr1 == NULL)
               {
                printf("Error!"); 
               }
     
               for(f1=0;f1<10;f1++){
               printf("Enter Number of %s bought by customer %d :",product,f1+1);
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
               // FILE *fptr2;
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
               printf("Number of %s bought by customer %d is : %d\n\n",product,f2+1 , data_set[f2]);
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
            
			 
               break;
        
		case 0:
			   goto out;
	           //break;
        
		
		default: 
	
	          printf("\n");
              printf("Enter a valid choice for 2 clusters choose 2 , for 3 clusters choose 3");	
              printf("\n");

             
			 
			 
			  }
              //end of switch case of k means 
       
	   
	   }//end of driver while loop for k means 

out: 
break; 

//break;//break out of choose_algo switch case's case 1 

case 2:
	
break;//break out of choose_algo switch case's case 2	

case 3:
break;//break out of choose_algo switch case's case 3

case 0:
	exit(0);
// exit program on user request 
	
	
default:
	
printf("\n");
printf("Enter a valid choice for Kmeans choose 1 , for Hierarchical Clustering choose 2 for DBSCAN choose 3");	
printf("\n");	

}//end of chose_algo switch case
}//end of main driver while loop 

return 0;

}
