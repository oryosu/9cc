#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    TK_RESERVED, // symbol
    TK_NUM, // integer token
    TK_EOF, // end of file
} TokenKind;

typedef struct Token Token;

// token type
struct Token {
    TokenKind kind; // type of token
    Token *next; // next input token
    int val; // the variance if the kind is TK_NUM
    char *str; // strings of token
};

Token *token;

void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

bool consume(char op) {
    if (token->kind != TK_RESERVED || token->str[0] != op)
        return false;
    token = token->next;
    return true;
}

void expect(char op) {
    if (token->kind != TK_RESERVED || token->str[0] != op)
        error("This is not '%c'.", op);
    token = token->next;
}

int expect_number() {
    if (token->kind != TK_NUM)
        error("This is not number");
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof() {
    return token->kind == TK_EOF;
}

// generize new token and concatenate to the cur
Token *new_token(TokenKind kind, Token *cur, char *str) {
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    cur->next = tok;
    return tok;
}

// tokenize input string p and return it
Token *tokenize(char *p) {
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while(*p) {
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (*p == '+' || *p == '-') {
            cur = new_token(TK_RESERVED, cur, p++);
            continue;
        }

        if (isdigit(*p)) {
            cur = new_token(TK_NUM, cur, p);
            cur->val = strtol(p, &p, 10);
            continue;
        }

        error("cannot tokenized");
    }

    new_token(TK_EOF, cur, p);
    return head.next;
    
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "incorrect number of arguments\n");
        return 1;
    }

    token = tokenize(argv[1]);

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    // the first letter of equation should be
    // the number, so check it ad output the first instruction
    // of mov
    printf("    mov rax, %d\n", expect_number());

    while (!at_eof()) {
        if (consume('+')) {
            printf("    add rax, %d\n", expect_number());
            continue;
        }
        
        expect('-');
        printf("    sub rax, %d\n", expect_number());
    }
    
    printf("    ret\n");
    return 0;
}