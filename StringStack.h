#ifndef StringStack_h
#define StringStack_h

#define SUCCESS 0
#define FAILURE -1

#define STRINGSTACK_SIZE_MAX 100
#define STRINGSTACK_STR_SIZE 100

typedef struct {
	char data[STRINGSTACK_SIZE_MAX][STRINGSTACK_STR_SIZE];
	int index;
} StringStack;

StringStack newStringStack();
int push(StringStack *stack, char *str);
char *pop(StringStack *stack);
char *get(StringStack *stack);

#endif

