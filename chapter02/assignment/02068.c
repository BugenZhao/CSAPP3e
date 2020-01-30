#include <assert.h>
#include <stdio.h>

int lower_one_mask(int n) {
    // Assume 1 <= n <= w
    int w = sizeof(int) << 3;
    return (1U << 1 << (n - 1)) - 1;
}

int main() {
    assert(lower_one_mask(6) == 0x3F);
    assert(lower_one_mask(17) == 0x1FFFF);
    assert(lower_one_mask(32) == 0xFFFFFFFF);
}
