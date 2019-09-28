#include<stdio.h>
void main(){
int i;
int arr[10]={1,2,3,4,5,6,7,8,9,10};
for(i=0;i<10;i++){
if((abs(arr[i]-1))<(abs(arr[i]- 10)))
{

    printf("cluster 1-");
    printf("%d\n",arr[i]);
}
else
{

    printf("cluster 2-");
    printf("%d\n",arr[i]);
}
}
}
