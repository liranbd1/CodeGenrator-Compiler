#include <stdio.h>

void main() {
    int a;
    int b[5][3][1];
    b[0][0][0] = 3;
    b[0][0][0]++;
    a = b[0][0][0]++;
    printf("%d\n",a);
    return;
}
