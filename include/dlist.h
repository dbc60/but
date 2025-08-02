#ifndef BUT_DLIST_H_
#define BUT_DLIST_H_

/**
 * @file dlist.h
 * @author Douglas Cuthbertson
 * @brief
 * @version 0.1
 * @date 2022-02-12
 *
 * See LICENSE.txt for copyright and licensing information about this file.
 */
// NOTE: This code is not thread safe. If you want thread-safety, wrap Dlist
// in a struct that contains a mutex to protect it.
#if defined(__cplusplus)
extern "C" {
#endif

// initialize a circular, doubly-linked list
#define DLIST_INIT(DL) ((DL)->right = (DL)->left = (DL))

// initialize a circular, doubly-linked list with an initial node
#define DLIST_INIT_ONE(DL, ND)           \
    do {                                 \
        (DL)->left = (DL)->right = (ND); \
        (ND)->left = (ND)->right = (DL); \
    } while (0)

// insert node to the left of the list
#define DLIST_INSERT_LEFT(DL, ND)       \
    do {                                \
        (ND)->left        = (DL)->left; \
        (ND)->right       = (DL);       \
        (ND)->left->right = (ND);       \
        (DL)->left        = (ND);       \
    } while (0)

// Insert a node at the end of the list
#define DLIST_INSERT_RIGHT(DL, ND)       \
    do {                                 \
        (ND)->right       = (DL)->right; \
        (ND)->left        = (DL);        \
        (DL)->right->left = (ND);        \
        (DL)->right       = (ND);        \
    } while (0)

// Insert a node at the head (to the right) of the list
#define DLIST_INSERT(DL, ND)             \
    do {                                 \
        (ND)->right       = (DL)->right; \
        (ND)->left        = (DL);        \
        (DL)->right->left = (ND);        \
        (DL)->right       = (ND);        \
    } while (0)

// Remove node from a list
#define DLIST_REMOVE(DL)                 \
    do {                                 \
        (DL)->left->right = (DL)->right; \
        (DL)->right->left = (DL)->left;  \
        (DL)->left        = (DL);        \
        (DL)->right       = (DL);        \
    } while (0)

// Remove node from a list, but don't reset the node's links
#define DLIST_REMOVE_SIMPLE(DL)          \
    do {                                 \
        (DL)->left->right = (DL)->right; \
        (DL)->right->left = (DL)->left;  \
    } while (0)

#define DLIST_IS_EMPTY(DL) ((DL)->left == (DL))

// directional macros
#define DLIST_RIGHT(DL)    ((DL)->right)
#define DLIST_LEFT(DL)     ((DL)->left)
#define DLIST_NEXT(DL)     ((DL)->right)
#define DLIST_PREVIOUS(DL) ((DL)->left)
#define DLIST_HEAD(DL)     ((DL)->right)
#define DLIST_FIRST(DL)    ((DL)->right)
#define DLIST_LAST(DL)     ((DL)->left)

// The node is its own tail
#define DLIST_TAIL(DL) ((DL))

struct DList {
    struct DList *left;
    struct DList *right;
};
typedef struct DList DList;

#if defined(__cplusplus)
}
#endif

#endif // BUT_DLIST_H_
