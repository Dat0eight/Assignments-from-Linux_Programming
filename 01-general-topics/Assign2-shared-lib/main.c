#include <stdio.h>
#include "inc/multi-math.h"
#include "inc/quote-msg.h"

int main()
{
    msg_func();
    
    printf("4 x 5 = %d", multi_func(4 , 5));
    
    return 0;
}