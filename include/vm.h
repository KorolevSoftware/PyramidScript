#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "pyramidscript.h"

enum object_type {
	Integer,
	Float,
	Bool,
	Aarithmetic, // from +-
	String,
	IPointerToFunction,
	SafeField,
	GetField,
	UserType,
	None,
};

enum byte_code_instructio {
	Skip, // skip instruction 
	PushReturnStack,
	PushMainStack,
	PushLocalStack,
	Plus,
	Minus,
	Jump,
	If,
	Div,
	Mult,
	PushLocal2MainStack,
	DropMainStack,
	DropReturnStack,
	DropLocalStack,
	MakeLocalStack,
	PushCurrentStateToReturn,
	Malloc,
	Print,
	Return,
};

struct byte_code {
	enum byte_code_instructio instructio;
	uint32_t index_of_object;
};

struct object {
	enum object_type type;
	union data_section {
		int ivalue;
		int string_index;
		bool boolean;
		float fvalue;
		uintptr_t data_section; // string or pointer
	};
};

enum named_type {
	nt_constant,
	nt_variable,
	nt_alias,
	nt_function,
	nt_function_native,
	nt_not_found,
};

struct named_any {
	enum named_type type;
	const char* name;
	uint32_t data; // pointer from variable, jump position from function, value from constant
};

struct PyramidScriptByteCode {
	struct byte_code* instructions_section;
	uint32_t program_size;

	struct object* data_section;
	uint32_t data_size;

	struct named_any* dictionary;
	uint32_t dictionary_count;
};

enum vm_runtime_error {
	Ok,
	ErrorType
};

struct field_table {
	char* name;
	struct object self;
};

struct newType {
	struct type_field* fields; // fields
	uint32_t fields_count;
};

struct state_config_size {
	uint32_t* return_satck;
	uint32_t* main_satck;
	uint32_t* raw_data_satck;
};

struct PyramidScriptState {
	struct object* main_stack;
	uint32_t main_stack_top;

	struct object* local_stack;
	uint32_t local_stack_top;

	uint32_t* return_stack;
	uint32_t return_stack_top;
};

struct PyramidScriptState* create_state();

void vm_run(struct PyramidScriptState* self, const struct PyramidScriptByteCode program);