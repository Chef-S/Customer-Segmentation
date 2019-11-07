#include<stdio.h>
#include<math.h>
#include <stdlib.h> 
#include<strings.h>
#include <limits.h>
#include <float.h>

//for DBSCAN 
#define UNCLASSIFIED -1
#define NOISE -2

#define CORE_POINT 1
#define NOT_CORE_POINT 0

#define SUCCESS 0
#define FAILURE -3
//till here for DBSCAN


//easy for nomenclature for DBSCAN
typedef struct point_s point_t;
struct point_s {
    double x, y, z;
    int cluster_id;
};

typedef struct node_s node_t;
struct node_s {
    unsigned int index;
    node_t *next;
};

typedef struct epsilon_neighbours_s epsilon_neighbours_t;
struct epsilon_neighbours_s {
    unsigned int num_members;
    node_t *head, *tail;
};

//till here easy for nomenclature for DBSCAN
// functions for DBSCAN 
node_t *create_node(unsigned int index);
int append_at_end(
     unsigned int index,
     epsilon_neighbours_t *en);
epsilon_neighbours_t *get_epsilon_neighbours(
    unsigned int index,
    point_t *points,
    unsigned int num_points,
    double epsilon,
    double (*dist)(point_t *a, point_t *b));
void print_epsilon_neighbours(
    point_t *points,
    epsilon_neighbours_t *en);
void destroy_epsilon_neighbours(epsilon_neighbours_t *en);
void dbscan(
    point_t *points,
    unsigned int num_points,
    double epsilon,
    unsigned int minpts,
    double (*dist)(point_t *a, point_t *b));
int expand(
    unsigned int index,
    unsigned int cluster_id,
    point_t *points,
    unsigned int num_points,
    double epsilon,
    unsigned int minpts,
    double (*dist)(point_t *a, point_t *b));
int spread(
    unsigned int index,
    epsilon_neighbours_t *seeds,
    unsigned int cluster_id,
    point_t *points,
    unsigned int num_points,
    double epsilon,
    unsigned int minpts,
    double (*dist)(point_t *a, point_t *b));
double euclidean_dist(point_t *a, point_t *b);
double adjacent_intensity_dist(point_t *a, point_t *b);
unsigned int parse_input(
    FILE *file,
    point_t **points,
    double *epsilon,
    unsigned int *minpts);
void print_points(
    point_t *points,
    unsigned int num_points);

node_t *create_node(unsigned int index)
{
    node_t *n = (node_t *) calloc(1, sizeof(node_t));
    if (n == NULL)
        perror("Failed to allocate node.");
    else {
        n->index = index;
        n->next = NULL;
    }
    return n;
}

int append_at_end(
     unsigned int index,
     epsilon_neighbours_t *en)
{
    node_t *n = create_node(index);
    if (n == NULL) {
        free(en);
        return FAILURE;
    }
    if (en->head == NULL) {
        en->head = n;
        en->tail = n;
    } else {
        en->tail->next = n;
        en->tail = n;
    }
    ++(en->num_members);
    return SUCCESS;
}

epsilon_neighbours_t *get_epsilon_neighbours(
    unsigned int index,
    point_t *points,
    unsigned int num_points,
    double epsilon,
    double (*dist)(point_t *a, point_t *b))
{
    epsilon_neighbours_t *en = (epsilon_neighbours_t *)
        calloc(1, sizeof(epsilon_neighbours_t));
    if (en == NULL) {
        perror("Failed to allocate epsilon neighbours.");
        return en;
    }
    for (int i = 0; i < num_points; ++i) {
        if (i == index)
            continue;
        if (dist(&points[index], &points[i]) > epsilon)
            continue;
        else {
            if (append_at_end(i, en) == FAILURE) {
                destroy_epsilon_neighbours(en);
                en = NULL;
                break;
            }
        }
    }
    return en;
}

void print_epsilon_neighbours(
    point_t *points,
    epsilon_neighbours_t *en)
{
    if (en) {
        node_t *h = en->head;
        while (h) {
            printf("(%lfm, %lf, %lf)\n",
                   points[h->index].x,
                   points[h->index].y,
                   points[h->index].z);
            h = h->next;
        }
    }
}

void destroy_epsilon_neighbours(epsilon_neighbours_t *en)
{
    if (en) {
        node_t *t, *h = en->head;
        while (h) {
            t = h->next;
            free(h);
            h = t;
        }
        free(en);
    }
}

void dbscan(
    point_t *points,
    unsigned int num_points,
    double epsilon,
    unsigned int minpts,
    double (*dist)(point_t *a, point_t *b))
{
    unsigned int i, cluster_id = 0;
    for (i = 0; i < num_points; ++i) {
        if (points[i].cluster_id == UNCLASSIFIED) {
            if (expand(i, cluster_id, points,
                       num_points, epsilon, minpts,
                       dist) == CORE_POINT)
                ++cluster_id;
        }
    }
}

int expand(
    unsigned int index,
    unsigned int cluster_id,
    point_t *points,
    unsigned int num_points,
    double epsilon,
    unsigned int minpts,
    double (*dist)(point_t *a, point_t *b))
{
    int return_value = NOT_CORE_POINT;
    epsilon_neighbours_t *seeds =
        get_epsilon_neighbours(index, points,
                               num_points, epsilon,
                               dist);
    if (seeds == NULL)
        return FAILURE;

    if (seeds->num_members < minpts)
        points[index].cluster_id = NOISE;
    else {
        points[index].cluster_id = cluster_id;
        node_t *h = seeds->head;
        while (h) {
            points[h->index].cluster_id = cluster_id;
            h = h->next;
        }

        h = seeds->head;
        while (h) {
            spread(h->index, seeds, cluster_id, points,
                   num_points, epsilon, minpts, dist);
            h = h->next;
        }

        return_value = CORE_POINT;
    }
    destroy_epsilon_neighbours(seeds);
    return return_value;
}

int spread(
    unsigned int index,
    epsilon_neighbours_t *seeds,
    unsigned int cluster_id,
    point_t *points,
    unsigned int num_points,
    double epsilon,
    unsigned int minpts,
    double (*dist)(point_t *a, point_t *b))
{
    epsilon_neighbours_t *spread =
        get_epsilon_neighbours(index, points,
                       num_points, epsilon,
                       dist);
    if (spread == NULL)
        return FAILURE;
    if (spread->num_members >= minpts) {
        node_t *n = spread->head;
        point_t *d;
        while (n) {
            d = &points[n->index];
            if (d->cluster_id == NOISE ||
                d->cluster_id == UNCLASSIFIED) {
                if (d->cluster_id == UNCLASSIFIED) {
                    if (append_at_end(n->index, seeds)
                        == FAILURE) {
                        destroy_epsilon_neighbours(spread);
                        return FAILURE;
                    }
                }
                d->cluster_id = cluster_id;
            }
            n = n->next;
        }
    }

    destroy_epsilon_neighbours(spread);
    return SUCCESS;
}

double euclidean_dist(point_t *a, point_t *b)
{
    return sqrt(pow(a->x - b->x, 2) +
            pow(a->y - b->y, 2) +
            pow(a->z - b->z, 2));
}

unsigned int parse_input(
    FILE *file,
    point_t **points,
    double *epsilon,
    unsigned int *minpts)
{
    unsigned int num_points, i = 0;
    fscanf(file, "%lf %u %u\n",
           epsilon, minpts, &num_points);
    point_t *p = (point_t *)
        calloc(num_points, sizeof(point_t));
    if (p == NULL) {
        perror("Failed to allocate points array");
        return 0;
    }
    while (i < num_points) {
          fscanf(file, "%lf %lf %lf\n",
                 &(p[i].x), &(p[i].y), &(p[i].z));
          p[i].cluster_id = UNCLASSIFIED;
          ++i;
    }
    *points = p;
    return num_points;
}

void print_points(
    point_t *points,
    unsigned int num_points)
{
    unsigned int i = 0;
    printf("Number of points: %u\n"
        " x     y     z     cluster_id\n"
        "-----------------------------\n"
        , num_points);
    while (i < num_points) {
          printf("%5.2lf %5.2lf %5.2lf: %d\n",
                 points[i].x,
                 points[i].y, points[i].z,
                 points[i].cluster_id);
          ++i;
    }
}

// till here functions for DBSCAN 



int main(){
//variables for Kmeans
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







// variables for DBSCAN
double epsilon;
point_t *points;
   
unsigned int minpts;
FILE *dbfile;
    
unsigned int num_points;



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
	
 dbfile =fopen("example.dat", "r");

   // unsigned int 
   num_points =
        parse_input(dbfile, &points, &epsilon, &minpts);
        	
        	printf("%u",&num_points);
    if (num_points) {
    	
        dbscan(points, num_points, epsilon,
               minpts, euclidean_dist);
               
        printf("Epsilon: %lf\n", epsilon);
        printf("Minimum points: %u\n", minpts);
        print_points(points, num_points);
        	
    }
    free(points);





break;//break out of choose_algo switch case's case 3

case 0:
	exit(0);
// exit program on user request 
	
	
default:
	
printf("\n");
printf("Enter a valid choice for Kmeans choose 1 , for Hierarchical Clustering choose 2 for DBSCAN choose 3");	
printf("\n");	

}//end of choose_algo switch case
}//end of main driver while loop 

return 0;

}
