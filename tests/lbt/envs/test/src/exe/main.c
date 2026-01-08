#include <stdio.h>

int TestFunc();
int testos();

int main() {
    printf("Hello, World!\nLib: %i\nOS: %i\n", TestFunc(), testos());
    return 0;
}
