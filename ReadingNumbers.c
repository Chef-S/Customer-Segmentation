#include <stdlib.h>
#include <stdio.h>

int main(){

    FILE *myFile;
    myFile = fopen("somenumbers.txt", "r");
    int arr[10];
    //read file into array
    int numberArray[10];
    int i;

    if (myFile == NULL){
        printf("Error Reading File\n");
        exit (0);
    }

    for (i = 0; i < 10; i++){
       fscanf(myFile, "%d,", &arr[i] );
    }

    for (i = 0; i < 10; i++){
        printf("Number is: %d\n\n", arr[i]);
    }

    fclose(myFile);

    return 0;
}
