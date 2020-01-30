#include <assert.h>
#include <stdio.h>

unsigned worker(unsigned x, unsigned y) {
    unsigned mask = 0xff;
    return (x & mask) | (y & ~mask);
}

int main() {
    assert(worker(0x89ABCDEFu, 0x76543210u) == 0x765432EFu);
}

