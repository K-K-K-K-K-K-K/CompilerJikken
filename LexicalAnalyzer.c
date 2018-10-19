#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "LexicalAnalyzer.h"

Token tokens[TOKENS_MAX];

int isSymb(char c) {
	switch(c) {
		case '+':
		case '-':
		case '*':
		case '/':
		case '(':
		case ')':
		case '=':
		case '<':
		case '>':
		case ',':
		case '.':
		case ';':
		case ':':
			return 1;
		default:
			return 0;
	}
}

int is2LetSymb(char c1, char c2) {
	switch (c1) {
		case '<':
			if (c2 == '>')
				return 1;
		case '>':
		case ':':
			if (c2 == '=')
				return 1;
		default:
			return 0;
	}
}

int getTknType(char *tkn) {
	if (!strcmp(tkn, "begin")
	|| !strcmp(tkn, "end")
	|| !strcmp(tkn, "if")
	|| !strcmp(tkn, "then")
	|| !strcmp(tkn, "while")
	|| !strcmp(tkn, "do")
	|| !strcmp(tkn, "return")
	|| !strcmp(tkn, "function")
	|| !strcmp(tkn, "var")
	|| !strcmp(tkn, "const")
	|| !strcmp(tkn, "odd")
	|| !strcmp(tkn, "write")
	|| !strcmp(tkn, "writeln"))
		return Keyword;
	else if (!strcmp(tkn, "+")
	|| !strcmp(tkn, "-")
	|| !strcmp(tkn, "*")
	|| !strcmp(tkn, "/")
	|| !strcmp(tkn, "(")
	|| !strcmp(tkn, ")")
	|| !strcmp(tkn, "=")
	|| !strcmp(tkn, "<")
	|| !strcmp(tkn, ">")
	|| !strcmp(tkn, "<>")
	|| !strcmp(tkn, "<=")
	|| !strcmp(tkn, ">=")
	|| !strcmp(tkn, ",")
	|| !strcmp(tkn, ".")
	|| !strcmp(tkn, ";")
	|| !strcmp(tkn, ":="))
		return Symbol;
	else if (atoi(tkn))
		return Number;
	else
		return Ident;

}

char *getTknTypeStr(TknType type) {
	switch(type) {
		case 0:
			return "Keyword";
		case 1:
			return "Symbol";
		case 2:
			return "Ident";
		case 3:
			return "Number";
	}
}

// tknCnt++ -> 表示 の方式へ変更すべし, lexResShwで判断
void lexicalAnalize(FILE *fp, bool lexResShw) {
	// 字句配列位置指示等用
	int tknCnt = 0;
	int idx = 0;

	// 計数用
	int line = 1;
	int chr = 0;
	
	char c;
	while ((c = getc(fp)) != EOF) {
		/* エラー処理用 ------------------------------------------------------ */
		if (c != '\n')
			chr++;
		/* ------------------------------------------------------------------- */
		if (isdigit(c)) { /* 数字: Ident/Keyword | Number */
			if (idx != 0 && isSymb(tokens[tknCnt].string[0])) { // 開始文字: 記号 -> Symbol
				tknCnt++;
				idx = 0;
			}

			tokens[tknCnt].string[idx] = c;
			idx++;
		} else if (isalpha(c)) { /* 英字: Ident/Keyword */
			if (idx != 0 && !isalpha(tokens[tknCnt].string[0])) { // 開始文字: 非英字 -> Number | Symbol
				tknCnt++;
				idx = 0;
			}

			tokens[tknCnt].string[idx] = c;
			idx++;
		} else if(isSymb(c)) { /* 記号: Symbol */
			if (idx != 0) // 非開始文字時
				if (!isSymb(tokens[tknCnt].string[0])) { // 開始文字: 非記号 -> Ident/Keyword | Number
					tknCnt++;
					idx = 0;
				} else if (!is2LetSymb(tokens[tknCnt].string[idx - 1], c)) { // 開始文字: 非特定 -> 次字句へ
					tknCnt++;
					idx = 0;
				}

			tokens[tknCnt].string[idx] = c;
			idx++;
		} else if (isspace(c) || c == '\t' || c == '\n') { /* {空格 | タブ | 改行} 文字 */
			/* エラー処理用 -------------------------------------------------- */
			if (c == '\n') {
				line++;
				chr = 0;
			}
			/* --------------------------------------------------------------- */
			if (idx == 0) // 開始文字 -> 棄却
				continue;

			tokens[tknCnt].string[idx + 1] = '\0';
			tknCnt++;
			idx = 0;
		} else {
			printf("[エラー] 認識不能な文字が出現\n");
			printf("    場所: 第%d行 第%d字\n", line, chr);
			printf("    位置: 第%d字句 第%d字後\n", tknCnt, idx);
			printf("          \"%s%c\"\n", tokens[tknCnt].string, c);
			printf("    文字: %c\n", c);
			printf("\n");
			exit(-1);
		}
	}

	/* TknType 付与 ----------------------------------------------------------- */
	for (int i = 0; i < tknCnt; i++)
		tokens[i].type = getTknType(tokens[i].string);
	/* ------------------------------------------------------------------------ */

	/* 字句解析結果表示 ------------------------------------------------------- */
	if (lexResShw)
		for (int i = 0; i < tknCnt; i++)
			printf("%3d: %15s %15s\n", i, tokens[i].string, getTknTypeStr(tokens[i].type));
	/* ------------------------------------------------------------------------ */
}

