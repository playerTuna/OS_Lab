#include <stdatomic.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct Node {
    int value;
    struct Node *next;
} Node;

typedef struct LockFreeStack {
    _Atomic(Node *) head; // atomic pointer to head
} LockFreeStack;

void init_stack(LockFreeStack *stack) {
    atomic_store(&stack->head, NULL);
}

bool push(LockFreeStack *stack, int value) {
    Node *new_node = malloc(sizeof(Node));
    if (!new_node) return false;

    new_node->value = value;

    Node *old_head;
    do {
        old_head = atomic_load(&stack->head);
        new_node->next = old_head;
        // Try to replace old_head with new_node atomically
    } while (!atomic_compare_exchange_weak(&stack->head, &old_head, new_node));

    return true;
}

bool pop(LockFreeStack *stack, int *value) {
    Node *old_head;
    Node *next_node;

    do {
        old_head = atomic_load(&stack->head);
        if (old_head == NULL) return false;
        next_node = old_head->next;
        // Try to replace head with next_node
    } while (!atomic_compare_exchange_weak(&stack->head, &old_head, next_node));

    *value = old_head->value;
    free(old_head);
    return true;
}
