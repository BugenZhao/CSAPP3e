#include <assert.h>
#include <stdio.h>

unsigned sra(int x, int k) {
    int xsrl = (unsigned) x >> k;
    int w    = sizeof(int) << 3;
    int mask = ~0 << (w - k);
    int m    = x & (1 << w - 1);
    // If x >= 0, then m = 0, let mask = 0
    // If x < 0, then !m = 0, let mask remain unchanged
    mask &= !m - 1;
    return xsrl | mask;
}

int main() {
    assert(sra(1, 1) == 0);
    assert(sra(~0, 1) == ~0);
}
