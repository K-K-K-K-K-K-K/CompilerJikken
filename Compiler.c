#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "StringStack.h"

typedef enum {
	Variable,
	Operator,
	OpenBracket,
	CloseBracket
} TknType;

TknType getTknType(char *tkn) {
	if (!strcmp(tkn, "+")
	|| !strcmp(tkn, "-")
	|| !strcmp(tkn, "*")
	|| !strcmp(tkn, "/")
	|| !strcmp(tkn, "="))
		return Operator;
	
	else if (!strcmp(tkn, "("))
		return OpenBracket;
	
	else if (!strcmp(tkn, ")"))
		return CloseBracket;

	else //if ('A' <= && <= 'Z')変数大文字検査
		return Variable;
}

int getOpPty(char *op) {
	if (!strcmp(op, "*")
	|| !strcmp(op, "/"))
		return 3;

	else if (!strcmp(op, "+")
	|| !strcmp(op, "-"))
		return 2;

	else if (!strcmp(op, "=")
	|| !strcmp(op, ")"))
		return 1;

	else if (!strcmp(op, ")"))
			return 0;
}

int cmpOps(char *op1, char *op2) {
	return getOpPty(op1) >= getOpPty(op2);
}

typedef struct {
	char Operator[STRINGSTACK_STR_SIZE];
	char Operand1[STRINGSTACK_STR_SIZE];
	char Operand2[STRINGSTACK_STR_SIZE];
	char Variable[STRINGSTACK_STR_SIZE];
} ThreeStateUnit;

// コンパイル: gcc Compiler.c StringStack.c -o Compiler
void main() {
	char formula[STRINGSTACK_STR_SIZE][STRINGSTACK_SIZE_MAX];
	int tknCnt;
	char rpnf[STRINGSTACK_SIZE_MAX];
	strcpy(rpnf, "");

	// 数式入力 -------------------------------------------------------
	for (tknCnt = 0;; tknCnt++) {
		printf("字句[%d]: ", tknCnt);
		char tkn[STRINGSTACK_STR_SIZE];
		scanf("%s", tkn);

		if (!strcmp(tkn, "q"))
			break;

		strcpy(formula[tknCnt], tkn);

		if (tknCnt > STRINGSTACK_SIZE_MAX) {
			printf("[エラー] 入力可能な字句数を超越");
			exit(-1);
		}
	}
	// ----------------------------------------------------------------

	// RPN化 ----------------------------------------------------------
	StringStack stack = newStringStack();
	for (int i = 0; i < tknCnt; i++) {
		char tkn[STRINGSTACK_STR_SIZE];
		strcpy(tkn, formula[i]);
		switch(getTknType(tkn)) {
			case Variable:
				sprintf(rpnf, "%s%s", rpnf, tkn);
				break;

			case OpenBracket:
				push(&stack, tkn);
				break;

			case CloseBracket:
				{
					char tkn[STRINGSTACK_STR_SIZE];
					while (strcmp(get(&stack), "(")) {
						strcpy(tkn, pop(&stack));
						sprintf(rpnf, "%s%s", rpnf, tkn);
					}
					pop(&stack);
				}
				break;	

			case Operator:
				if (stack.index == 0)
					push(&stack, tkn);
				else if (cmpOps(tkn, get(&stack)))
					push(&stack, tkn);
				else {
					do {
						char tknTmp[STRINGSTACK_STR_SIZE];
						strcpy(tknTmp, pop(&stack));
						sprintf(rpnf, "%s%s", rpnf, tknTmp);
					} while (!cmpOps(tkn, get(&stack)));
					push(&stack, tkn);
				}
				break;
		}
	}

	for (;;) {
		char tkn[STRINGSTACK_STR_SIZE];
		strcpy(tkn, pop(&stack));
		if (!strcmp(tkn, "\0"))
			break;
		sprintf(rpnf, "%s%s", rpnf, tkn);
	}

	printf("\n< 逆ポーランド記法 >\n");
	for (int i = 0; i < strlen(rpnf); i++)
		printf("%c ", rpnf[i]);
	printf("\n\n");
	// ----------------------------------------------------------------
	
	ThreeStateUnit pool[STRINGSTACK_SIZE_MAX]; // 大きさに意味無し
	int poolIdx = 0;

	// 三番地コード生成並出力 -----------------------------------------
	printf("< 三番地コード >\n");
	
	stack = newStringStack();
	char tkn[STRINGSTACK_STR_SIZE];
	char c = 'a'; // 変数はaから
	for (int i = 0; i < strlen(rpnf); i++) {
		sprintf(tkn, "%c", rpnf[i]); // 字句
		switch(getTknType(tkn)) {
			case Variable:
				push(&stack, tkn);
				break;

			case Operator:
				{
					strcpy(pool[poolIdx].Operator, tkn);
					strcpy(pool[poolIdx].Operand2, pop(&stack)); // 第二引数
					strcpy(pool[poolIdx].Operand1, pop(&stack)); // 第一引数
					sprintf(pool[poolIdx].Variable, "$%c", c++); // 変数
					printf("%s (%s, %s, %s)\n", pool[poolIdx].Operator, pool[poolIdx].Operand1, pool[poolIdx].Operand2, pool[poolIdx].Variable);
					push(&stack, pool[poolIdx].Variable);
					poolIdx++;
				}
				break;
		}
	}
	printf("\n");
	// ----------------------------------------------------------------

	// 仮想CPU用機械語列生成並出力 ------------------------------------
	printf("< 仮想CPU用機械語列 >\n");

	char tmp[3] = "";
	for (int i = 0; i < poolIdx; i++) {
		if (!strcmp(tmp, pool[poolIdx].Operand1) && !strcmp(tmp, pool[poolIdx].Operand2)) {
			if (i != 0)
				printf("ST %s\n", tmp);
			printf("LD %s\n", pool[i].Operand1);
		}
		switch (pool[i].Operator[0]) {
			case '+':
				printf("AD %s\n", pool[i].Operand2);
				break;

			case '-':
				printf("SB %s\n", pool[i].Operand2);
				break;

			case '*':
				printf("ML %s\n", pool[i].Operand2);
				break;

			case '/':
				printf("DV %s\n", pool[i].Operand2);
				break;
		}
		sprintf(tmp, "%s", pool[i].Variable);
		if (i == poolIdx - 1)
			printf("ST %s\n", tmp);
	}	
	// ----------------------------------------------------------------

	printf("\n");
}
