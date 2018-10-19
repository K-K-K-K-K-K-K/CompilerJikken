#ifndef LexicalAnalyzer_h
#define LexicalAnalyzer_h

#define TOKENS_MAX 200
#define TOKEN_MAX_LEN 50

typedef enum {
	Keyword,
	Symbol,
	Ident,
	Number
} TknType;

typedef struct {
	char string[TOKEN_MAX_LEN];
	TknType type;
} Token;

extern Token tokens[TOKENS_MAX];

char *getTknTypeStr(TknType type);
void lexicalAnalize(FILE *fp, bool lexResShw);

#endif

