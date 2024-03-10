#include "pyramidscript.h"
#include "tokenizer.h"
#include "compiler.h"
#include "vm.h"
#include <stdio.h>
#include <stdlib.h>


struct PyramidScriptByteCode* Compile(const char* source) {
	struct lexem_container tt = tokenizer(source);
	compile(tt);
	return 0;
}

static struct PyramidScriptByteCode load_file(const char* path) {
	struct PyramidScriptByteCode script;

	FILE* f = fopen(path, "rb");

	fread(&script.data_size, sizeof(uint32_t), 1, f);
	fread(&script.program_size, sizeof(uint32_t), 1, f);

	script.instructions_section = calloc(script.program_size, sizeof(struct byte_code));
	script.data_section = calloc(script.data_size, sizeof(struct object));

	fread(script.instructions_section, sizeof(struct byte_code), script.program_size, f);
	fread(script.data_section, sizeof(struct object), script.data_size, f);

	fclose(f);

	return script;
}

void Run(struct PyramidScriptState* self, const char* path) {
	struct PyramidScriptByteCode script = load_file(path);
	struct PyramidScriptState* self111 = create_state();
	vm_run(self111, script);
}

struct PyramidScript PyramidScript = {
	.Compile = Compile,
	.Make = 0,
	.Release = 0,
	.Run = Run,
};