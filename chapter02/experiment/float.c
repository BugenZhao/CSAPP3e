#include <stdio.h>
int main() {
    unsigned x = (1U << 24U) + 1U;
    printf("%d %f %f\n", x, (float) x, (double) x);

    unsigned long long y = (1UL << 53U) + 1U;
    printf("%lld %f\n", y, (double) y);
}
