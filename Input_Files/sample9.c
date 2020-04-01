//
// Created by rotem on 3/31/2020.
//

#include <stdio.h>

void main() {
    double i;
    double j;

    j = 5;

    for (i=10;i >= 0;i--) {
        if (j > i) {
            j=j
        }
        else {
            j = j - 1;
        }
        printf("%.2f\n", i);
    }
    return;
}