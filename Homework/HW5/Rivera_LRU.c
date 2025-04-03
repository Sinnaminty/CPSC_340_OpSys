#include <stdio.h>
#include <stdlib.h>

typedef struct Stack {
  int m_top;
  int m_stackSize;
  int *m_stack;
} Stack;

Stack *createStack(int stackSize) {
  Stack *newStack = (Stack *)malloc(sizeof(struct Stack));
  newStack->m_stackSize = stackSize;
  newStack->m_top = -1;
  newStack->m_stack = (int *)malloc(stackSize * sizeof(int));
  for (int i = 0; i < stackSize; i++) {
    newStack->m_stack[i] = -1;
  }

  return newStack;
}
void freeStack(Stack *stack) {
  free(stack->m_stack);
  free(stack);
}

void pushStack(Stack *stack, int data) {
  if (stack->m_top + 1 == stack->m_stackSize) {
    fprintf(stderr, "Stack Overflow!!!\n");
    exit(1);
  }
  stack->m_top++;
  stack->m_stack[stack->m_top] = data;
}
int popStack(Stack *stack) {
  if (stack->m_top == -1) {
    fprintf(stderr, "Stack Underflow!!!\n");
    exit(1);
  }
  stack->m_top--;
  return stack->m_stack[stack->m_top + 1];
}

void printStack(Stack *stack) {
  int i;
  if (stack->m_top == -1) {
    printf("Stack empty!\n");
  } else {
    printf("Stack Elements: ");
    for (i = stack->m_top; i >= 0; i--) {
      printf("%d ", stack->m_stack[i]);
    }
    printf("\n");
  }
}
int main() {
  Stack *stack = createStack(5);
  printf("Printing stack..\n");
  printStack(stack);

  printf("Pushing stack elements 1, 2, 3, 4, 5\n");
  for (int i = 1; i <= 5; i++) {
    pushStack(stack, i);
  }
  printf("Printing Stack..\n");
  printStack(stack);

  printf("Freeing Stack..\n");
  freeStack(stack);

  return 0;
}
