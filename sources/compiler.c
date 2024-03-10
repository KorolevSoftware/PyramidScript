#include "compiler.h"
#include "vm.h"
#include <stdlib.h>
#include <iso646.h>
#include <stdio.h>
#include <string.h>

void push_instruction(struct PyramidScriptByteCode* result, struct byte_code code) {
	result->instructions_section[result->program_size] = code;
	result->program_size++;
}

void push_value(struct PyramidScriptByteCode* result, struct object value) {
	result->data_section[result->data_size] = value;
	push_instruction(result, (struct byte_code) { .instructio = PushMainStack, .index_of_object = result->data_size });
	result->data_size++;
}

void push_dictionary(struct PyramidScriptByteCode* result, const char* name, enum named_type type, uint32_t data) {
	result->dictionary[result->dictionary_count] = (struct named_any){ .name = name, .data = data, .type = type };
	result->dictionary_count++;
}

void drop_dictionary(struct PyramidScriptByteCode* result, uint32_t count) {
	result->dictionary_count -= count;
}

struct named_any get_dictionary(struct PyramidScriptByteCode* result, const char* name) {
	for (size_t i = result->dictionary_count - 1; i >= 0; i--) {
		struct named_any curent = result->dictionary[i];
		if (strcmp(curent.name, name) == 0) {
			return curent;
		}
	}

	return (struct named_any) { .type = nt_not_found };
}


void compile_to(struct PyramidScriptByteCode* result, struct lexem_container self, uint32_t start, uint32_t end) {
	for (uint32_t token_index = start; token_index < end; token_index++) {
		struct token current_token = self.stream[token_index];

		// PushMainStack
#pragma region values

		if (current_token.type == tt_bool) {
			push_value(result, (struct object) { .boolean = current_token.data_section.boolean, .type = Bool });
		}

		if (current_token.type == tt_float) {
			push_value(result, (struct object) { .fvalue = current_token.data_section.fvalue, .type = Float });
		}

		if (current_token.type == tt_integer) {
			push_value(result, (struct object) { .ivalue = current_token.data_section.ivalue, .type = Integer });
		}

#pragma endregion

#pragma region operations

		if (current_token.type == tt_plus) {
			push_instruction(result, (struct byte_code) { .instructio = Plus, .index_of_object = 0 });
		}

		if (current_token.type == tt_minus) {
			push_instruction(result, (struct byte_code) { .instructio = Minus, .index_of_object = 0 });
		}

		if (current_token.type == tt_multip) {
			push_instruction(result, (struct byte_code) { .instructio = Mult, .index_of_object = 0 });
		}

		if (current_token.type == tt_div) {
			push_instruction(result, (struct byte_code) { .instructio = Div, .index_of_object = 0 });
		}

#pragma endregion

		if (current_token.type == tt_if) {
			int find_else = find_controll_flow_token(self.stream, token_index, tt_if, tt_else);
			int find_then = find_controll_flow_token(self.stream, token_index, tt_if, tt_then);


			if (find_else > 0) {
				push_instruction(result, (struct byte_code) { .instructio = If, .index_of_object = find_else+1 }); // +1 from skip jump instruction
				compile_to(result, self, token_index + 1, find_else); // +1 from skip token if
				push_instruction(result, (struct byte_code) { .instructio = Jump, .index_of_object = find_then });
				token_index = find_else;
			} else {
				push_instruction(result, (struct byte_code) { .instructio = If, .index_of_object = find_then });
			}
		}

		if (current_token.type == tt_ident) {
			struct named_any is_it =  get_dictionary(result, current_token.data_section.name);
			if (is_it.type == nt_alias) { // Push local variable to main stack
				push_instruction(result, (struct byte_code) { .instructio = PushLocal2MainStack, .index_of_object = is_it.data});
			}

			if (is_it.type == nt_function) { // Call function
				push_instruction(result, (struct byte_code) { .instructio = PushCurrentStateToReturn, .index_of_object = is_it.data });
				push_instruction(result, (struct byte_code) { .instructio = Jump, .index_of_object = is_it.data});
			}

		}

		if (current_token.type == tt_function) {
			struct token func_name = self.stream[token_index + 1];
			push_dictionary(result, func_name.data_section.name, nt_function, result->program_size);

			int end_function_position = find_controll_flow_token(self.stream, token_index, tt_function, tt_semicolon);
			uint32_t alias_count = 0;
			uint32_t function_body = token_index + 2;
			for (uint32_t index_alias = function_body; ; function_body+=2, index_alias += 2) {
				if (self.stream[index_alias].type != tt_alias) {
					break;
				}
				alias_count++;
				struct token alias_name = self.stream[index_alias+1];
				push_dictionary(result, alias_name.data_section.name, nt_alias, alias_count);
				
			}
			if (alias_count > 0) {
				push_instruction(result, (struct byte_code) { .instructio = PushLocalStack, .index_of_object = alias_count });
			}
			compile_to(result, self, function_body, end_function_position); // +2 skip token funtion and function name

			drop_dictionary(result, alias_count);
			push_instruction(result, (struct byte_code) { .instructio = DropLocalStack, .index_of_object = alias_count });
			push_instruction(result, (struct byte_code) { .instructio = Return, .index_of_object = 0 });
			token_index = end_function_position;
		}

	}
}


void save_to_file(struct PyramidScriptByteCode self) {
	FILE* f = fopen("e:\\Project\\Develop_2023\\Sima-Engine\\scripts\\test11.foe", "wb");
	fwrite(&self.data_size,    sizeof(uint32_t), 1, f);
	fwrite(&self.program_size, sizeof(uint32_t), 1, f);

	fwrite(self.instructions_section, sizeof(struct byte_code), self.program_size, f);
	fwrite(self.data_section, sizeof(struct object), self.data_size, f);

	fclose(f);
}

int find_controll_flow_token(const struct token* stream, int position, enum token_type incriment, enum token_type find) {
	int temp_position = position + 1;
	int if_stack = 0;
	while (true) {
		enum token_type type = stream[temp_position].type;
		if (type == incriment) {
			if_stack++;
		}

		if (type == find) {
			if (if_stack == 0) {
				return temp_position;
			} else {
				if_stack--;
			}
		}

		if (type == tt_semicolon) {
			return -1;
		}

		temp_position++;
	}
}

void compile(struct lexem_container self) {
	struct PyramidScriptByteCode bytes;
	bytes.program_size = 3;
	bytes.data_size = 0;
	bytes.dictionary_count = 0;
	bytes.data_section = calloc(self.count, sizeof(struct object));
	bytes.instructions_section = calloc(self.count, sizeof(struct byte_code));
	bytes.dictionary = calloc(self.count, sizeof(struct named_any));
	
	compile_to(&bytes, self, 0, self.count);

	struct named_any is_it = get_dictionary(&bytes, "main");
	bytes.instructions_section[0] = (struct byte_code){ .instructio = PushCurrentStateToReturn, .index_of_object = is_it.data };
	bytes.instructions_section[1] = (struct byte_code){ .instructio = Jump, .index_of_object = is_it.data };
	bytes.instructions_section[2] = (struct byte_code){ .instructio = Skip, .index_of_object = is_it.data };

	save_to_file(bytes);
}