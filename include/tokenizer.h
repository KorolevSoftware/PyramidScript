#pragma once
#include <stdbool.h>

struct token word_to_token(const char* word, const char* token_name, enum token_type type);
struct token word_to_identifier(const char* word, const char* token_name, enum token_type type);
struct token dotstring(const char* word, const char* token_name, enum token_type type);
struct token word_to_integer(const char* word, const char* token_name, enum token_type type);
struct token word_to_flaot(const char* word, const char* token_name, enum token_type type);
struct token word_to_bool(const char* word, const char* token_name, enum token_type type);

#define FOREACH_TOKENS(TOKENS) \
	TOKENS(tt_dup, "dup",	word_to_token) \
	TOKENS(tt_drop, "drop", word_to_token) \
	TOKENS(tt_swap, "swap", word_to_token) \
	TOKENS(tt_over, "over", word_to_token) \
	TOKENS(tt_rot, "rot",	word_to_token) \
	\
	TOKENS(tt_equal, "=",	word_to_token) \
	TOKENS(tt_great, "<",	word_to_token) \
	TOKENS(tt_less, ">",	word_to_token) \
	TOKENS(tt_invert, "invert", word_to_token) \
	TOKENS(tt_and, "and",	word_to_token) \
	TOKENS(tt_or, "or",		word_to_token) \
	\
	TOKENS(tt_plus, "+",	word_to_token) \
	TOKENS(tt_minus, "-",	word_to_token) \
	TOKENS(tt_mod, "mod",	word_to_token) \
	TOKENS(tt_multip, "*",	word_to_token) \
	TOKENS(tt_div, "/",		word_to_token) \
	\
	TOKENS(tt_if, "if",		word_to_token) \
	TOKENS(tt_else, "else", word_to_token) \
	TOKENS(tt_then, "then", word_to_token) \
	TOKENS(tt_do, "for",	word_to_token) \
	TOKENS(tt_index, "i",	word_to_token) \
	TOKENS(tt_loop, "loop", word_to_token) \
	TOKENS(tt_begin, "begin", word_to_token) \
	TOKENS(tt_until, "until", word_to_token) \
	\
	TOKENS(tt_allot, "allot", word_to_token) \
	TOKENS(tt_alias, "alias", word_to_token) \
	TOKENS(tt_cells, "cells", word_to_token) \
	TOKENS(tt_constant, "constant", word_to_token) \
	TOKENS(tt_variable, "variable", word_to_token) \
	TOKENS(tt_at, "@",			word_to_token) \
	TOKENS(tt_setvalue, "!",	word_to_token) \
	TOKENS(tt_function, ":",	word_to_token) \
	TOKENS(tt_semicolon, ";",	word_to_token) \
	TOKENS(tt_bool, "bool", word_to_bool) \
	TOKENS(tt_string, "string", dotstring ) \
	TOKENS(tt_integer, "123123",	word_to_integer ) \
	TOKENS(tt_float, "3.14",	word_to_flaot ) \
	TOKENS(tt_ident, "ident",	word_to_identifier) \



#define GENERATE_ENUM(ENUM) ENUM,
typedef void(*token_iterator)(int line, char* word, void* arg);

enum token_type {
	FOREACH_TOKENS(GENERATE_ENUM)
	tt_none,
};

struct token {
	enum token_type type;
	int line;
	union {
		int ivalue;
		float fvalue;
		char* string;
		char* name;
		bool boolean;
	} data_section;
};

struct lexem_container {
	struct token* stream;
	int count;
};

struct token_type_pair {
	enum token_type type;
	char token_name[16];
	struct token(*func_cmp)(const char*, const char*, enum token_type);
};

struct lexem_container tokenizer(const char* stream);

#undef GENERATE_ENUM