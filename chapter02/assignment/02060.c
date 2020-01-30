#include <assert.h>
#include <stdio.h>

unsigned replace_byte(unsigned x, int i, unsigned char b) {
    return x & ~(0xff << i * 8) | (b << i * 8);
}

int main() {
    assert(replace_byte(0x12345678, 2, 0xAB) == 0x12AB5678);
    assert(replace_byte(0x12345678, 0, 0xAB) == 0x123456AB);
}
