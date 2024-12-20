#include <stdlib.h>
#include <stdio.h>

typedef struct ArrayList ArrayList;

struct ArrayList {
    char * data;
    size_t size;
    size_t capacity;

    void (* append) (ArrayList *, char);
    void (* remove) (ArrayList *, size_t);
    char (* get) (ArrayList *, size_t);
    void (* print) (ArrayList * );
}; 

void addElement(ArrayList *list, char element);
void removeElement(ArrayList *list, size_t index);
char getElement(ArrayList *list, size_t index);
void printArrayList(ArrayList *list);
ArrayList *createArrayList(size_t initialCapacity);
void destroyArrayList(ArrayList *list);


ArrayList *createArrayList(size_t initialCapacity) {
    ArrayList *list = (ArrayList *)malloc(sizeof(ArrayList));
    if (!list) {
        fprintf(stderr, "Failed to allocate memory for ArrayList\n");
        exit(EXIT_FAILURE);
    }

    list->data = (char *)malloc(initialCapacity * sizeof(char));
    if (!list->data) {
        fprintf(stderr, "Failed to allocate memory for data\n");
        free(list);
        exit(EXIT_FAILURE);
    }

    list->size = 0;
    list->capacity = initialCapacity;

    // Assign function pointers
    list->append = addElement;
    list->remove = removeElement;
    list->get = getElement;
    list->print = printArrayList;

    return list;
}

void destroyArrayList(ArrayList *list) {
    if (list) {
        free(list->data);
        free(list);
    }
}


void removeElement(ArrayList *list, size_t index) {
    if (index >= list->size) {
        fprintf(stderr, "Index out of bounds\n");
        return;
    }
    for (size_t i = index; i < list->size - 1; i++) {
        list->data[i] = list->data[i + 1];
    }
    list->size--;
}

char getElement(ArrayList *list, size_t index) {
    if (index >= list->size) {
        fprintf(stderr, "Index out of bounds\n");
        exit(EXIT_FAILURE);
    }
    return list->data[index];
}

void printArrayList(ArrayList *list) {
    printf("[");
    for (size_t i = 0; i < list->size; i++) {
        printf("%d", list->data[i]);
        if (i < list->size - 1) {
            printf(", ");
        }
    }
    printf("]\n");
}

void addElement(ArrayList *list, char element) {
    if (list->size == list->capacity) {
        // Resize the array
        list->capacity *= 2;
        list->data = (char *)realloc(list->data, list->capacity * sizeof(char));
        if (!list->data) {
            fprintf(stderr, "Failed to reallocate memory\n");
            exit(EXIT_FAILURE);
        }
    }
    list->data[list->size++] = element;
}
