#include <llist.h>
#include <stdlib.h>
#include <string.h>


xLList_t* xLList_Create(size_t node_size) {
    xLList_t *list = malloc(sizeof(xLList_t));
    list->root = NULL;
    list->node_size = node_size;
    return list;
}

void xLList_Destroy(xLList_t *list) {
    while(list->root != NULL) {
        xLList_Remove(list, list->root);
    }
    free(list);
}

void xLList_Remove(xLList_t *list, xLList_Node_t *node) {
    if(node == list->root)
        list->root = node->next;

    if(node->prev != NULL)
        node->prev->next = node->next;

    if(node->next != NULL)
        node->next->prev = node->prev;

    free(node->data);
    free(node);
}

xLList_Node_t* xLList_Append(xLList_t *list, void *node) {

    /* Special Case, An empty root node */
    if(list->root == NULL) {
        list->root = malloc(sizeof(xLList_Node_t));
        list->root->next = NULL;
        list->root->prev = NULL;
        list->root->data = malloc(list->node_size);
        memcpy(list->root->data, node, list->node_size);
        return list->root;
    }

    xLList_Node_t* c = list->root;
    while(c->next != NULL) { c = c->next; }
    c->next = malloc(sizeof(xLList_Node_t));
    c->next->next = NULL;
    c->next->prev = c;
    c->next->data = malloc(list->node_size);
    memcpy(c->next->data, node, list->node_size);
    return c->next;
}

