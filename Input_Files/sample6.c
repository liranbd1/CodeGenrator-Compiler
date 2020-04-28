#include <stdio.h>
struct B
{
    int a;
    int b;
};
void main() {
    struct B k;
    int t;
    k.a = 5;
    k->b = 3;
    t = k->a + k.b;
    printf("%d\n",t);
    return;
}
