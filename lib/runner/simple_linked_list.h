#pragma once
#include <stdlib.h>

typedef struct simple_linked_list_node {
    void * val;
    struct simple_linked_list_node * next;
} simple_linked_list_node_t;

void simple_linked_list_push(simple_linked_list_node_t * head, void * val) {
    simple_linked_list_node_t * current = head;
    while (current->next != NULL) {
        current = current->next;
    }

    current->next = (simple_linked_list_node_t *) malloc(sizeof(simple_linked_list_node_t));
    current->next->val = val;
    current->next->next = NULL;
}

