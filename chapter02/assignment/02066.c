#include <assert.h>
#include <stdio.h>

int leftmost_one(unsigned x) {
    // Turn x into vector like "0...011...1"
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    // Then turn x into        "0...010...0"
    return (x >> 1) + 1;
}

int main() {
    assert(leftmost_one(0xFF000000) == 0x80000000);
}
