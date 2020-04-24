#include <stdio.h>

void main() {
    int x;
    int i;
    i = 0;
    x = 0;
    for (; i<10; i++)
    {
        x = i*i;
        printf("%d\n", x);
        if (x>50)
        {
            break;
        }
    }
    return;
}
