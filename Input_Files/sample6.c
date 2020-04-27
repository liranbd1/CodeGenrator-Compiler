#include <stdio.h>
struct B{
    int a;
    int d;
    double l;
    struct B* next;
};

struct C{
    struct B* k;
};
void main() {
    struct C a;
    a.k.a = 5;
    printf("&d\n", a.k.a);
    return;
}
