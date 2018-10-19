#include <stdio.h>
#include <string.h>
#include "StringStack.h"

void init(StringStack *stack) {
	for (int i = 0; i < STRINGSTACK_SIZE_MAX; i++)
			stack->data[i][0] = '\0';
	stack->index = 0;
}

StringStack newStringStack() {
	StringStack stack;
	init(&stack);
	return stack;
}

int push(StringStack *stack, char *str) {
	if (stack->index >= STRINGSTACK_SIZE_MAX - 1)
		return FAILURE;

	stack->index++;
	strcpy(stack->data[stack->index], str);

	return SUCCESS;
}

char *pop(StringStack *stack) {
	if (stack->index <= 0)
		return "\0";
	return stack->data[stack->index--];
}

char *get(StringStack *stack) {
	return stack->data[stack->index];
}

