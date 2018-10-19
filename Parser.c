#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "LexicalAnalyzer.h"

typedef enum {
	Factor,
	Term,
	Expression,
	ConstDecl,
	VarDecl,
	FuncDecl,
	Condition,
	Statement,
	Block,
	Program
} Location;

char *getLocationStr(Location loc) {
	switch (loc) {
		case Factor:
			return "Factor";
		case Term:
			return "Term";
		case Expression:
			return "Expression";
		case ConstDecl:
			return "ConstDecl";
		case VarDecl:
			return "VarDecl";
		case FuncDecl:
			return "FuncDecl";
		case Condition:
			return "Condition";
		case Statement:
			return "Statement";
		case Block:
			return "Block";
		case Program:
			return "Program";
	}
}

int tknCnt = -1;
Token tkn;
Location loc;
bool disp = false;

void setNextTkn() {
	tkn = tokens[++tknCnt];
	if (disp)
		printf("%3d: %15s %15s\n", tknCnt, getLocationStr(loc), tkn.string);
}

void setPrevTkn() {
	tkn = tokens[--tknCnt];
	if (disp)
		printf("%3d: %15s %15s\n", tknCnt, getLocationStr(loc), tkn.string);
}

void syntaxErrorWithLexicalMissmatch(char *expected) {
	if (disp)
		printf("\n");
	printf("[エラー] 構文規則に非従\n");
	printf("    位置: 第%d字句\n", tknCnt);
	printf("    期待上の字句: %s\n", expected);
	printf("    実際上の字句: %s\n", tkn.string);
	printf("\n");
	exit(-1);
}

void syntaxErrorWithTknTypeMissmatch(TknType type) {
	if (disp)
		printf("\n");
	printf("[エラー] 構文規則に非従\n");
	printf("    位置: 第%d字句\n", tknCnt);
	printf("    期待上の字句の種類: %s\n", getTknTypeStr(type));
	printf("    期待上の字句の種類: %s\n", getTknTypeStr(tkn.type));
	printf("\n");
	exit(-1);
}

void syntaxError() {
	if (disp)
		printf("\n");
	printf("[エラー] 構文規則に非従\n");
	printf("    位置: 第%d字句\n", tknCnt);
	printf("    字句: %s\n", tkn.string);
	printf("\n");
	exit(-1);
}

void parseExpression();
void parseBlock();

void parseFactor() {
	Location loctmp = loc;
	loc = Factor;

	setNextTkn();
	if (tkn.type == Ident) {
		setNextTkn();
		if (!strcmp(tkn.string, "(")) {
			setNextTkn();
			if (strcmp(tkn.string, ")")) {
				setPrevTkn();
				do {
					parseExpression();
					setNextTkn();
				} while (!strcmp(tkn.string, ","));
				if (strcmp(tkn.string, ")"))
					syntaxErrorWithLexicalMissmatch(")");
			}
		} else
			setPrevTkn();
	} else if (tkn.type == Number) {
	} else if (!strcmp(tkn.string, "(")) {
		parseExpression();
		setNextTkn();
		if (strcmp(tkn.string, ")"))
			syntaxErrorWithLexicalMissmatch(")");
	} else
		syntaxError();

	loc = loctmp;
}

void parseTerm() {
	Location loctmp = loc;
	loc = Term;

	parseFactor();
	setNextTkn();
	if (!strcmp(tkn.string, "*")
	|| !strcmp(tkn.string, "/")) {
		do {
			parseFactor();
			setNextTkn();
		} while (!strcmp(tkn.string, "*")
		|| !strcmp(tkn.string, "/"));
		setPrevTkn();
	} else
		setPrevTkn();

	loc = loctmp;
}

void parseExpression() {
	Location loctmp = loc;
	loc = Expression;

	setNextTkn();
	if (strcmp(tkn.string, "+")
	|| strcmp(tkn.string, "-"))
		setPrevTkn();
	parseTerm();
	setNextTkn();
	if (!strcmp(tkn.string, "+")
	|| !strcmp(tkn.string, "-")) {
		do {
			parseTerm();
			setNextTkn();
		} while (!strcmp(tkn.string, "+")
		|| !strcmp(tkn.string, "-"));
		setPrevTkn();
	} else
		setPrevTkn();

	loc = loctmp;
}

void parseConstDecl() {
	Location loctmp = loc;
	loc = ConstDecl;

	do {
		setNextTkn();
		if (tkn.type != Ident)
			syntaxErrorWithTknTypeMissmatch(Ident);
		setNextTkn();
		if (strcmp(tkn.string, "="))
			syntaxErrorWithLexicalMissmatch("=");
		setNextTkn();
		if (tkn.type != Number)
			syntaxErrorWithTknTypeMissmatch(Number);
		setNextTkn();
	} while (!strcmp(tkn.string, ","));
	if (strcmp(tkn.string, ";"))
		syntaxErrorWithLexicalMissmatch(";");

	loc = loctmp;
}

void parseVarDecl() {
	Location loctmp = loc;
	loc = VarDecl;

	do {
		setNextTkn();
		if (tkn.type != Ident)
			syntaxErrorWithTknTypeMissmatch(Ident);
		setNextTkn();
	} while (!strcmp(tkn.string, ","));
	if (strcmp(tkn.string, ";"))
		syntaxErrorWithLexicalMissmatch(";");

	loc = loctmp;
}

void parseFuncDecl() {
	Location loctmp = loc;
	loc = FuncDecl;

	setNextTkn();
	if (tkn.type != Ident)
		syntaxErrorWithTknTypeMissmatch(Ident);
	setNextTkn();
	if (strcmp(tkn.string, "("))
		syntaxErrorWithLexicalMissmatch("(");
	setNextTkn();
	if (strcmp(tkn.string, ")")) {
		setPrevTkn();
		do {
			setNextTkn();
			if (tkn.type != Ident)
				syntaxErrorWithTknTypeMissmatch(Ident);
			setNextTkn();
		} while (!strcmp(tkn.string, ","));
		if (strcmp(tkn.string, ")"))
			syntaxErrorWithLexicalMissmatch(")");
	}
	parseBlock();
	setNextTkn();
	if (strcmp(tkn.string, ";"))
		syntaxErrorWithLexicalMissmatch(";");

	loc = loctmp;
}

void parseCondition() {
	Location loctmp = loc;
	loc = Condition;

	setNextTkn();
	if (!strcmp(tkn.string, "odd"))
		parseExpression();
	else {
		setPrevTkn();
		parseExpression();
		setNextTkn();
		if (!strcmp(tkn.string, "=")
		|| !strcmp(tkn.string, "<>")
		|| !strcmp(tkn.string, "<")
		|| !strcmp(tkn.string, ">")
		|| !strcmp(tkn.string, "<=")
		|| !strcmp(tkn.string, ">=")) {
			parseExpression();
		} else
			syntaxErrorWithLexicalMissmatch("=, <>, <, >, <=, >=");
	}

	loc = loctmp;
}

void parseStatement() {
	Location loctmp = loc;
	loc = Statement;

	setNextTkn();
	if (tkn.type == Ident) {
		setNextTkn();
		if (strcmp(tkn.string, ":="))
			syntaxErrorWithLexicalMissmatch(":=");
		parseExpression();
	} else if (!strcmp(tkn.string, "begin")) {
		do {
			parseStatement();
			setNextTkn();
		} while (!strcmp(tkn.string, ";"));
		if (strcmp(tkn.string, "end"))
			syntaxErrorWithLexicalMissmatch("end");
	} else if (!strcmp(tkn.string, "if")) {
		parseCondition();
		setNextTkn();
		if (strcmp(tkn.string, "then"))
			syntaxErrorWithLexicalMissmatch("then");
		parseStatement();
	} else if (!strcmp(tkn.string, "while")) {
		parseCondition();
		setNextTkn();
		if (strcmp(tkn.string, "do"))
			syntaxErrorWithLexicalMissmatch("do");
		parseStatement();
	} else if (!strcmp(tkn.string, "return"))
		parseExpression();
	else if (!strcmp(tkn.string, "write"))
		parseExpression();
	else if (!strcmp(tkn.string, "writeln")) {
	} else
		setPrevTkn();

	loc = loctmp;
}

void parseBlock() {
	Location loctmp = loc;
	loc = Block;

	setNextTkn();
	while (!strcmp(tkn.string, "const")
	|| !strcmp(tkn.string, "var")
	|| !strcmp(tkn.string, "function")) {
		if (!strcmp(tkn.string, "const"))
			parseConstDecl();
		else if (!strcmp(tkn.string, "var"))
			parseVarDecl();
		else if (!strcmp(tkn.string, "function"))
			parseFuncDecl();
		setNextTkn();
	}
	setPrevTkn();
	parseStatement();

	loc = loctmp;
}

void parseProgram() {
	loc = Program;

	parseBlock();
	setNextTkn();
	if (strcmp(tkn.string, "."))
		syntaxErrorWithLexicalMissmatch(".");

	disp = false;
	setNextTkn();
	if (strcmp(tkn.string, "")) {
		printf("\n");
		syntaxError();
	}
}

void parse(bool prsProcShw) {
	disp = prsProcShw;
	parseProgram();
}

// コンパイル: gcc Parser.c LexicalAnalyzer.c -o Parser
void main(int argc, char **argv) {
	switch (argc) {
		case 1:
			printf("[エラー] ソースファイルの非指定\n");
			printf("    書式: ./Parser [FILE] {0, 1} {0, 1}\n");
			printf("\n");
			exit(-1);
		case 2:
			printf("[エラー] 字句解析結果表示/非表示の非指定\n");
			printf("    書式: ./Parser [FILE] {0, 1} {0, 1}\n");
			printf("    非表示 => 0\n");
			printf("    表示   => 1\n");
			printf("\n");
			exit(-1);
		case 3:
			printf("[エラー] 構文解析行程表示/非表示の非指定\n");
			printf("    書式: ./Parser [FILE] {0, 1} {0, 1}\n");
			printf("    非表示 => 0\n");
			printf("    表示   => 1\n");
			printf("\n");
			exit(-1);
		case 4:
			break;
		default:
			printf("[エラー] コマンドライン引数が不正\n");
			printf("    書式: ./Parser [FILE] {0, 1} {0, 1}\n");
			printf("\n");
			exit(-1);
	}

	FILE *fp;
	if ((fp = fopen(argv[1], "r")) == NULL) {
		printf("[エラー] ソースファイルの非存在\n");
		printf("\n");
		exit(-1);
	}

	bool lexResShw = atoi(argv[2]);
	bool prsProcShw = atoi(argv[3]);
	printf("字句解析 -------------------------------------------------------------\n");
	lexicalAnalize(fp, lexResShw);
	printf("----------------------------------------------------------------------\n");
	printf("\n");
	printf("構文解析 -------------------------------------------------------------\n");
	parse(prsProcShw);
	printf("----------------------------------------------------------------------\n");
}

