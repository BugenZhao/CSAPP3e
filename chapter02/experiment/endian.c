#include <stdio.h>
int main() {
    int a = 0x01234567;
    for (int i = 0; i < 4; i++) {
        printf("%.2x\n", ((unsigned char *) (&a))[i]);
    } // little endian
}
