//
// Created by Bugen Zhao on 2020/2/4.
//

#include <stdio.h>

long d[] = {6, 7, 3, 5, 2, 4, 1};

void bubble_a(long *data, long count) {
    for (long *last = data + count - 1; last != data; last--) {
        for (long *p = data; p < last; p++)
            if (*(p + 1) < *p) {
                long t = *(p + 1);
                *(p + 1) = *p;
                *p = t;
            }
    }
}

int main() {
    bubble_a(d, 7);
    for (int i = 0; i < 7; ++i) {
        printf("%ld ", d[i]);
    }
}