#include <assert.h>
#include <stdio.h>

long rfact(long n);

int main() {
    long n;
    scanf("%ld", &n);
    printf("%ld", rfact(n));
}
