#include <stdio.h>

void main() {
    int a;
    int *b;
    int **c;
    int k;
    a = 24;
    b = &a;
    c = &b;
    k = **c + *b;
    printf("%d\n",a);
    printf("%d\n", **c);
    printf("%d\n", *b);
    printf("%d\n", b);
    printf("%d\n", c);
    printf("%d\n", k);
    return;
}
