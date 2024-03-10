#pragma once

typedef unsigned int uint;

// types
struct PyramidScriptByteCode;
typedef void (*PyramidScriptNativeFunction) (struct PyramidScriptState* self);
struct PyramidScriptState;

struct PyramidScriptStateConfigSize {
	uint data_raw; // in byte
	uint data_stack;
	uint return_stack;
	uint dictionary;
};

enum PyramidScriptErrors {
	NoError,

	//StackOverflow
	DataStackOverflow,
	DataRawStackOverflow,
	VariableStackOverflow,
	ReturnStackOverflow,
	FunctionStackOverflow,

	// Parser
	Syntax,
	TokenValue,
};
 
struct PyramidScript {
	struct PyramidScriptState* (*Make) ();
	struct PyramidScriptByteCode* (*Compile) (const char* source);
	void (*Release) (struct PyramidScriptState* self);
	void (*Run) (struct PyramidScriptState* self, const char* path);
};

extern struct PyramidScript PyramidScript;