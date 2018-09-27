#include "binaryTree.h"

binaryTree createBTree (void * data, allocFn alloc)
{
    binaryTree newBTree = alloc(sizeof(binaryTreeCDT));

    // TODO creo que deberiamos chequear que newBTree NO sea null con algun testeo

    newBTree->data = data;
    newBTree->left=NULL;
    newBTree->right=NULL;

    return newBTree;
}

void freeBTree (binaryTree bTree, freeFn free)
{
    if (bTree != NULL)
    {
        free(bTree->data);              //TODO chequear si hace falta
        free(bTree);
    }
    else
    {
        // TODO error?
    }
}