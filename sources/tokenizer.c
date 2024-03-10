#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "iso646.h"
#include "tokenizer.h"

#define GENERATE_TOKEN_PAIRS(tokenr, token_string, func_parse) { tokenr, token_string, func_parse },
#define array_size(x) sizeof(x)/sizeof(x[0])

// TODO rename
struct token word_to_token(const char* word, const char* token_name, enum token_type type) {
	if (strcmp(token_name, word) == 0)
		return (struct token) { .type = type };
	return (struct token) { .type = tt_none };
}

struct token word_to_bool(const char* word, const char* token_name, enum token_type type) {
	if (strcmp(word, "true") == 0) {
		return (struct token) { .type = tt_bool, .data_section.boolean = true };
	}

	if (strcmp(word, "false") == 0) {
		return (struct token) { .type = tt_bool, .data_section.boolean = false };
	}

	return (struct token) { .type = tt_none };
}

struct token word_to_identifier(const char* word, const char* token_name, enum token_type type) {
	if (strchr(word, '"') or strchr(word, '\\')) {
		return (struct token) { .type = tt_none };
	}
	return (struct token) { .type = tt_ident, .data_section.name = strdup(word) };
}

struct token dotstring(const char* word, const char* token_name, enum token_type type) {
	int word_lenght = strlen(word);
	if (word[word_lenght - 1] != '\"' or strncmp(".\"", word, 2) != 0)
		return (struct token) { .type = tt_none };

	char* word_dup = strdup(word + 2); // +2 skip ." chars
	word_dup[word_lenght - 3] = '\0'; // set zero char to " position. -3 skip "\0
	return (struct token) { .type = tt_string, .data_section.string = word_dup };
}

struct token word_to_integer(const char* word, const char* token_name, enum token_type type) {
	char* enp_pos;
	int value = strtol(word, &enp_pos, 10);
	uintptr_t integer_lenght = enp_pos - word;
	if (integer_lenght == strlen(word))
		return (struct token) { .type = tt_integer, .data_section.ivalue = value };
	return (struct token) { .type = tt_none };
}

struct token word_to_flaot(const char* word, const char* token_name, enum token_type type) {
	char* enp_pos;
	float value = strtof(word, &enp_pos);
	uintptr_t integer_lenght = enp_pos - word;
	if (integer_lenght == strlen(word))
		return (struct token) { .type = tt_float, .data_section.fvalue = value };
	return (struct token) { .type = tt_none };
}

static struct token_type_pair key_words[] = {
	FOREACH_TOKENS(GENERATE_TOKEN_PAIRS)
};

static char* get_token(char* stream, char** next) {
	if (not stream or strlen(stream) == 0 or (*next) == NULL) {
		return NULL;
	}

	while (*stream and (isspace(*stream) or not isprint(*stream))) { // skip first space, tabe end new line
		stream++;
	}

	char* begin_token = stream;

	if (*stream) {
		while (isprint(*stream) and not isspace(*stream)) {
			if (*stream == '(') { // comments skip begin
				while (true) {
					if (*stream == ')') {
						break;
					}
					stream++;
				}
				return get_token(++stream, next);
			}// comments skip end

			if (*stream == '\"') { // string begin
				stream++;
				while (true) {
					if (*stream == '\"') {
						break;
					}
					stream++;
				}
				stream++;
				break; // string end
			}
			stream++;
		}
		if (strlen(stream) == 0) {
			(*next) = NULL;
		} else {
			(*stream) = '\0'; // separate string to sub strings
			stream++;
			(*next) = stream;
		}
		return begin_token;
	}
	return NULL;
}

static int new_line_count(char* start, char* end) {
	int count = 0;
	for (; start != end; start++) {
		if (*start == '\n') {
			count++;
		}
	}
	return count;
}

static void tokens_iterator(const char* stream, token_iterator func, void* arg) {
	char* stream_copy = strdup(stream);
	char* stream_start = stream_copy;
	char* iter = stream_copy;
	char** next = &iter;// space, new line, tab
	char* word = get_token(iter, next);

	while (word != NULL) {
		int line = new_line_count(stream_start, word);
		func(line, word, arg);
		word = get_token(*next, next);
	}

	free(stream_copy);
}

static void tokens_to_lexem(int line, char* word, void* arg) {
	struct lexem_container* lexem_container = (struct lexem_container*)arg;
	for (int key = 0; key < array_size(key_words); key++) {
		const struct token_type_pair key_word = key_words[key];
		struct token new_token = key_word.func_cmp(word, key_word.token_name, key_word.type);
		
		if (new_token.type == tt_none)
			continue;

		new_token.line = line;
		lexem_container->stream[lexem_container->count] = new_token;
		lexem_container->count++;
		break;
	}
}

static void tokens_count(int line, char* word, void* arg) {
	int* token_count = (int*)arg;
	(*token_count)++;
}

struct lexem_container tokenizer(const char* stream) {
	int token_count = 0;
	tokens_iterator(stream, tokens_count, &token_count);

	struct token* tokens = calloc(token_count, sizeof(struct token));

	struct lexem_container lexem_container;

	lexem_container.stream = tokens;
	lexem_container.count = 0;
	tokens_iterator(stream, tokens_to_lexem, &lexem_container);

	return lexem_container;
}