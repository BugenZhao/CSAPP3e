#include <assert.h>
#include <stdio.h>

int add(int a, int b);

int main() {
    int a, b;
    scanf("%d%d", &a, &b);
    printf("%d", add(a, b));
}
