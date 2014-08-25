#ifndef __LLIST_H
#define __LLIST_H 1

#include <stdint.h>
#include <sys/types.h>

struct _xLList_Node_ {
    struct _xLList_Node_ *next;
    struct _xLList_Node_ *prev;
    void* data;
}; typedef struct _xLList_Node_ xLList_Node_t;

typedef struct {
    xLList_Node_t* root;
    int count;
    size_t node_size;
} xLList_t;


/** Create a new linked list                                **
 ** Space is managed by the llist code. All you have to do  **
 ** is tell it the size of the structure you are managing.  **
 **                                                         **
 ** Arguments:                                              **
 **  node_size - The size of the structure you are managing **
 **                                                         **
 ** Returns:                                                **
 **  A pointer to a xLList_t struct.                        **/

xLList_t* xLList_Create(size_t node_size);


/** Destroy a linked list.                                  **
 ** Release the space used by a list. This includes all the **
 ** user data.                                              **
 **                                                         **
 ** Arguments:                                              **
 **  list      - A refrence to the list returned by Create. **
 **                                                         **
 ** Returns:                                                **
 **  None.                                                  **/

void xLList_Destroy(xLList_t *list);


/** Remove an element from a Linked List.                   **
 ** Release the space used by This element. This includes   **
 ** The user data for the node.                             **
 **                                                         **
 ** Arguments:                                              **
 **  list      - A refrence to the list returned by Create. **
 **  node      - A refrence to the Node you want to remove. **
 **                                                         **
 ** Returns:                                                **
 **  None.                                                  **/

void xLList_Remove(xLList_t *list, xLList_Node_t *node);


/** Add a new structure to the linked list.                 **
 ** Copy a new structure into the list. Data is allocated   **
 ** for the data so your old refrence should be removed.    **
 **                                                         **
 ** Arguments:                                              **
 **  list      - A refrence to the list returned by Create. **
 **  node      - A refrence to the user structure to copy.  **
 **                                                         **
 ** Returns:                                                **
 **  A refrence to the new xLList_Node_t object in the list **/

xLList_Node_t* xLList_Append(xLList_t *list, void *node);

#endif /** __LLIST_H **/
