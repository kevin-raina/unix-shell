#include "../include/paren_checker.h"

typedef struct node {
  char data;
  struct node *next;
} node;

static void push(node **top, char c) {
  node *newnode = (node *)malloc(sizeof(node));
  newnode->data = c;
  newnode->next = *top;
  *top = newnode;
}

static char pop(node **top) {
  if (*top == NULL)
    return '\0';
  char val = (*top)->data;
  node *temp = *top;
  *top = (*top)->next;
  free(temp);
  return val;
}

static void clearstack(node **top) {
  while (*top)
    pop(top);
}

static int ismatch(char open, char close) {
  return (open == '(' && close == ')') || (open == '{' && close == '}');
}

int isbalanced(const char *expr) {
  node *top = NULL;
  for (int i = 0; i < strlen(expr); i++) {
    char ch = expr[i];

    if (ch == '(' || ch == '{') {
      push(&top, ch);
    } else if (ch == ')' || ch == '}') {
      if (top == NULL) {
        clearstack(&top);
      }
      char t = pop(&top);
      if (!ismatch(t, ch)) {
        clearstack(&top);
      }
    }
  }
  int result = (top == NULL);
  clearstack(&top);
  return result;
}
