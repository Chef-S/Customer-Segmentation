    #include <stdio.h>
    int main()
    {
     int arr[10];
     int i;
     FILE *fptr1;
     fptr = fopen("customer_data.txt","w");
     
     if(fptr == NULL)
     {
     printf("Error!"); 
     }
     
     for(i=0;i<10;i++){
     printf("Enter data for customer %d :",i);
     scanf("%d",&arr[i]);
     printf("%d",arr[i]);
     fprintf(fptr,"%d",arr[i]); 
     fprintf(fptr," "," "); 
    }
    
	fclose(fptr1); 
     
    
    }
    
