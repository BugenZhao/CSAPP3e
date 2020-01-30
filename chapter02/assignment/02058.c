#include <assert.h>

int is_little_endian() {
    unsigned x = 0x01;
    return ((unsigned char *) &x)[0] == 0x01;
}

int main() {
    assert(is_little_endian());
}
