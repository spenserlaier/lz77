#ifndef ARRAYLIST_H
#define ARRAYLIST_H

#include <stddef.h> // For size_t

// Forward declaration of the ArrayList struct
typedef struct ArrayList ArrayList;

// Function prototypes
ArrayList *createArrayList(size_t initialCapacity);
void destroyArrayList(ArrayList *list);

#endif // ARRAYLIST_H
