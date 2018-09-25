//
// Created by Emilio Basualdo on 9/14/18.
//

#ifndef ANTEOJOS_QUEUE_H
#define ANTEOJOS_QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <myAlloc.h>

#define EMPTY_QUEUE -1


// A structure to represent a queue
typedef struct Queue
{
    int front, rear, size;
    unsigned capacity;
    int* array;
}Queue;

struct Queue* createQueue(unsigned capacity);
// Queue is full when size becomes equal to the capacity
int isFull(struct Queue* queue);
// Queue is empty when size is 0
int isEmpty(struct Queue* queue);
// Function to add an item to the queue.
// It changes rear and size
void enqueue(struct Queue* queue, int item);
// Function to remove an item from queue.
// It changes front and size
int dequeue(struct Queue* queue);
// Function to get front of queue
int front(struct Queue* queue);
// Function to get rear of queue
int rear(struct Queue* queue);

#endif //ANTEOJOS_QUEUE_H
