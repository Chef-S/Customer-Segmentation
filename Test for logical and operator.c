#include<stdio.h>
#include<math.h>
int main(){
	int a=2;
	int b=3;
    int c=4;
    
    do
    {printf("I'm still in the loop");
    a++;
    b++;
    c++;
	}while(a!=4||b!=6||c!=8);
}


/*

output:
I'm still in the loop
I'm still in the loop


*/
