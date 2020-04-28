#include <stdio.h>
struct B
{
    int a;
    int b;
};
void main() {
    int a;
    a = 5;
    a += 1;
    a*= 2;
    a/= 3;
    a-= 9;
    printf("&d\n", a);
    return;
}
