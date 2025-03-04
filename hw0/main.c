#include <stdio.h>
#include <stdlib.h>

#define TABLE_SIZE 256 

typedef struct Node {
    char ch;
    int count;
    struct Node* next;
} Node;


Node* hashTable[TABLE_SIZE] = {NULL};
Node* head = NULL;

void insert_with_hash_table(char ch) {
    if (hashTable[(unsigned char)ch]) {
        hashTable[(unsigned char)ch]->count++;
        return;
    }

    Node* newNode = (Node*)malloc(sizeof(Node));
    if (!newNode) {
        printf("fail\n");
        exit(1);
    }

    newNode->ch = ch;
    newNode->count = 1;
    newNode->next = head;
    head = newNode;

   
    hashTable[(unsigned char)ch] = newNode;
}

void printList() {
    Node* temp = head;
    while (temp) {
        printf("%c : %d\n", temp->ch, temp->count);
        temp = temp->next;
    }
}

void freeList() {
    Node* temp;
    while (head) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

int main() {
    FILE* file = fopen(__FILE__, "r");
    if (!file) {
        printf("fail\n");
        return 1;
    }

    char ch;
    while ((ch = fgetc(file)) != EOF) {
        if (ch != '\n' && ch != ' ') {
            insert_with_hash_table(ch);
        }
    }
    fclose(file);

    printList();
    freeList();

    return 0;
}
