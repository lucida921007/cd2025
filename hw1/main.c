#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 100

typedef struct Node {
    char value[MAX_LEN];   // 儲存 token 字串
    char type[MAX_LEN];    // 儲存 token 類型字串
    struct Node* next;
} Node;

Node* head = NULL;
Node* tail = NULL;

typedef enum {
    START,
    IN_ID,
    IN_NUM,
    IN_ASSIGN,
    IN_GT,
    IN_LT
} State;

void insert_token(char* value, const char* type) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (!newNode) {
        printf("fail\n");
        exit(1);
    }
    strcpy(newNode->value, value);
    strcpy(newNode->type, type);
    newNode->next = NULL;

    if (!head) {
        head = tail = newNode;
    } else {
        tail->next = newNode;
        tail = newNode;
    }
}

void printList() {
    Node* temp = head;
    while (temp) {
        printf("%s: %s\n", temp->value, temp->type);
        temp = temp->next;
    }
}

int is_letter(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

int is_digit(char c) {
    return (c >= '0' && c <= '9');
}

int is_letter_or_digit_or_underscore(char c) {
    return is_letter(c) || is_digit(c) || c == '_';
}

void classify_and_insert(char* str) {
    if (strcmp(str, "int") == 0)
        insert_token(str, "TYPE_TOKEN");
    else if (strcmp(str, "main") == 0)
        insert_token(str, "MAIN_TOKEN");
    else if (strcmp(str, "if") == 0)
        insert_token(str, "IF_TOKEN");
    else if (strcmp(str, "else") == 0)
        insert_token(str, "ELSE_TOKEN");
    else if (strcmp(str, "while") == 0)
        insert_token(str, "WHILE_TOKEN");
    else if (is_digit(str[0]))
        insert_token(str, "LITERAL_TOKEN");
    else
        insert_token(str, "ID_TOKEN");
}

int main() {
    FILE* file = fopen("test.txt", "r");
    if (!file) {
        printf("fail\n");
        return 1;
    }

    State state = START;
    char ch;
    char buffer[MAX_LEN];
    int idx = 0;

    while ((ch = fgetc(file)) != EOF) {
        switch (state) {
            case START:
                if (ch == ' ' || ch == '\n' || ch == '\t') {
                    // ignore whitespace
                } else if (is_letter(ch) || ch == '_') {
                    buffer[idx++] = ch;
                    state = IN_ID;
                } else if (is_digit(ch)) {
                    buffer[idx++] = ch;
                    state = IN_NUM;
                } else if (ch == '=') {
                    buffer[idx++] = ch;
                    state = IN_ASSIGN;
                } else if (ch == '>') {
                    buffer[idx++] = ch;
                    state = IN_GT;
                } else if (ch == '<') {
                    buffer[idx++] = ch;
                    state = IN_LT;
                } else {
                    char s[2] = {ch, '\0'};
                    if (ch == '(') insert_token(s, "LEFTPAREN_TOKEN");
                    else if (ch == ')') insert_token(s, "REFTPAREN_TOKEN");
                    else if (ch == '{') insert_token(s, "LEFTBRACE_TOKEN");
                    else if (ch == '}') insert_token(s, "REFTBRACE_TOKEN");
                    else if (ch == ';') insert_token(s, "SEMICOLON_TOKEN");
                    else if (ch == '+') insert_token(s, "PLUS_TOKEN");
                    else if (ch == '-') insert_token(s, "MINUS_TOKEN");
                }
                break;

            case IN_ID:
                if (is_letter_or_digit_or_underscore(ch)) {
                    buffer[idx++] = ch;
                } else {
                    buffer[idx] = '\0';
                    classify_and_insert(buffer);
                    idx = 0;
                    state = START;
                    fseek(file, -1, SEEK_CUR);  // unread current char
                }
                break;

            case IN_NUM:
                if (is_digit(ch)) {
                    buffer[idx++] = ch;
                } else {
                    buffer[idx] = '\0';
                    insert_token(buffer, "LITERAL_TOKEN");
                    idx = 0;
                    state = START;
                    fseek(file, -1, SEEK_CUR);
                }
                break;

            case IN_ASSIGN:
                if (ch == '=') {
                    insert_token("==", "EQUAL_TOKEN");
                } else {
                    insert_token("=", "ASSIGN_TOKEN");
                    fseek(file, -1, SEEK_CUR);
                }
                idx = 0;
                state = START;
                break;

            case IN_GT:
                if (ch == '=') {
                    insert_token(">=", "GREATEREQUAL_TOKEN");
                } else {
                    insert_token(">", "GREATER_TOKEN");
                    fseek(file, -1, SEEK_CUR);
                }
                idx = 0;
                state = START;
                break;

            case IN_LT:
                if (ch == '=') {
                    insert_token("<=", "LESSEQUAL_TOKEN");
                } else {
                    insert_token("<", "LESS_TOKEN");
                    fseek(file, -1, SEEK_CUR);
                }
                idx = 0;
                state = START;
                break;
        }
    }

    
    if (state == IN_ID || state == IN_NUM) {
        buffer[idx] = '\0';
        if (state == IN_ID)
            classify_and_insert(buffer);
        else
            insert_token(buffer, "LITERAL_TOKEN");
    }

    fclose(file);
    printList();
    return 0;
}
