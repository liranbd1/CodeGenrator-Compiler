#include <stdio.h>
struct B
{
    int a;
    int b;
};

struct C{
    struct B k;
};

void main() {
    struct C cobj;
    struct B b;
    b.a = 5;
    cobj.k.a = 20;
    printf("%d\n",cobj.k.a);
    return;
}
