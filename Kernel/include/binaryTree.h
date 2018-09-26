#ifndef BINARYTREE_H
#define BINARYTREE_H

#include <stddef.h>

typedef struct binaryTreeCDT
{
    void * data;
    struct binaryTreeCDT * left;
    struct binaryTreeCDT * right;

} binaryTreeCDT;

typedef binaryTreeCDT * binaryTree;
typedef void * (* allocFn) (size_t);
typedef void (* freeFn) (void *);

/** crea un bTree
 * recibe la data a guardar y la funcion para alocar memoria
 */
binaryTree createBTree (void * data, void * (* allocFn) (size_t));

/** elimina un bTree dado, con su funcion para liberar memoria */
void freeBTree (binaryTree bTree, void (* freeFn) (void *));

#endif
