#include <stdio.h>
#include "lockFreeStack.h"

int main() {
    LockFreeStack stack;
    init_stack(&stack);

    push(&stack, 10);
    push(&stack, 20);
    push(&stack, 30);

    int value;
    while (pop(&stack, &value)) {
        printf("Popped: %d\n", value);
    }

    return 0;
}