#include<stdio.h>
void main(){
int i,i1,i2,k,j,fix=0;
int arr[10]={1,2,3,4,5,6,7,8,9,10};
int c1[10];
int c2[10];
int centroid1;
int centroid2;
printf("enter c 1 c 2");

scanf("%d %d",&centroid1, &centroid2 );
   printf("\n");


for(i=0;i<10;i++){
if((abs(arr[i]-centroid1))<=(abs(arr[i]- centroid2)))
{
 
   // printf("cluster 1-");
    //printf("%d\n",arr[i]);
    j=i;
    c1[j]=arr[i];
//printf("%d\t",i);
//printf("%d\n",c1[j]);


}

else 
{

  //  printf("cluster 2-");
    //printf("%d\n",arr[i]); 

   
    c2[fix]=arr[i];
    //printf("%d\t",i);
    //printf("%d\n",c2[fix]);
    fix++;
  //printf("%d",fix);  
}
}

for(i1=0;i1<=j;i1++)
{

printf("%d \t",c1[i1]);	
}


printf("\n");
for(i2=0;i2<=fix-1;i2++)
{

printf("%d \t",c2[i2]);	
}


}


