#include <iso646.h>
#include <stdbool.h>
#include <stdlib.h>
#include <vm.h>
#include <stdio.h>

void push_main_stack(struct PyramidScriptState* self, const struct object data_section) {
	self->main_stack[self->main_stack_top] = data_section;
	self->main_stack_top++;
}
void drop_local_stack(struct PyramidScriptState* self, uint32_t count) {
	self->local_stack_top -= count;
}

void push_local_stack(struct PyramidScriptState* self, uint32_t count) {
	for (uint32_t index = 0; index < count; index++) {
		self->main_stack_top--;
		self->local_stack[self->local_stack_top] = self->main_stack[self->main_stack_top];
		self->local_stack_top++;
	}
}

void push_return_stack(struct PyramidScriptState* self, uint32_t position) {
	self->return_stack[self->return_stack_top] = position;
	self->return_stack_top++;
}

void drop_main_stack(struct PyramidScriptState* self) {
	self->main_stack_top--;
}

uint32_t drop_return_stack(struct PyramidScriptState* self) {
	self->return_stack_top--;
	return self->return_stack[self->return_stack_top];
}

void local_to_main_stack(struct PyramidScriptState* self, uint32_t stack_pos) { //stack top - stack_pos
	uint32_t pos = self->local_stack_top - stack_pos;
	push_main_stack(self, self->local_stack[pos]);
}

enum vm_runtime_error if_operator(struct PyramidScriptState* self, uint32_t position_from_jump, uint32_t* current_instruction) {
	self->main_stack_top--;
	struct object obj_first = self->main_stack[self->main_stack_top];
	
	if (obj_first.type != Bool) {
		return ErrorType;
	}

	if (not obj_first.boolean) {
		*current_instruction = position_from_jump - 1;
	}
	return Ok;
}

enum vm_runtime_error minus(struct PyramidScriptState* self) {
	struct object obj_first = self->main_stack[self->main_stack_top - 1];
	struct object obj_secend = self->main_stack[self->main_stack_top - 2];

	if (not (obj_first.type < Aarithmetic) or not (obj_secend.type < Aarithmetic)) {
		return ErrorType;
	}

	if (obj_secend.type == Float and obj_first.type == Float) {
		obj_secend.fvalue = obj_secend.fvalue - obj_first.fvalue;

	} else if (obj_secend.type == Integer and obj_first.type == Integer) {
		obj_secend.ivalue = obj_secend.ivalue - obj_first.ivalue;

	} else if (obj_secend.type == Float and obj_first.type == Integer) {
		obj_secend.fvalue = obj_secend.fvalue - obj_first.ivalue;

	} else if (obj_secend.type == Integer and obj_first.type == Float) {
		obj_secend.fvalue = obj_secend.ivalue - obj_first.fvalue;
	}

	self->main_stack[self->main_stack_top - 2] = obj_secend;
	self->main_stack_top -= 1;
	return Ok;
}

enum vm_runtime_error plus(struct PyramidScriptState* self) {
	struct object obj_first = self->main_stack[self->main_stack_top - 1];
	struct object obj_secend = self->main_stack[self->main_stack_top - 2];

	if (not (obj_first.type < Aarithmetic) or not (obj_secend.type < Aarithmetic)) {
		return ErrorType;
	}
	
	if (obj_secend.type == Float and obj_first.type == Float) {
		obj_secend.fvalue = obj_secend.fvalue + obj_first.fvalue;

	} else if (obj_secend.type == Integer and obj_first.type == Integer) {
		obj_secend.ivalue = obj_secend.ivalue + obj_first.ivalue;

	} else if (obj_secend.type == Float and obj_first.type == Integer) {
		obj_secend.fvalue = obj_secend.fvalue + obj_first.ivalue;

	} else if (obj_secend.type == Integer and obj_first.type == Float) {
		obj_secend.fvalue = obj_secend.ivalue + obj_first.fvalue;
	}

	self->main_stack[self->main_stack_top - 2] = obj_secend;
	self->main_stack_top -= 1;
	return Ok;
}

void vm(struct PyramidScriptState* self, const struct PyramidScriptByteCode program) {
	for (uint32_t current_instruction = 0; current_instruction < program.program_size; current_instruction++) {
		struct byte_code current = program.instructions_section[current_instruction];

		switch (current.instructio) {
			case PushReturnStack:
				push_return_stack(self, current.index_of_object); break;

			case PushLocalStack:
				push_local_stack(self, current.index_of_object); break;

			case PushMainStack:
				push_main_stack(self, program.data_section[current.index_of_object]); break;

			case DropMainStack:
				drop_main_stack(self); break;

			case DropLocalStack:
				drop_local_stack(self, current.index_of_object); break;

			case PushLocal2MainStack:
				local_to_main_stack(self, current.index_of_object); break;

			case Plus:
				plus(self); break;

			case Minus:
				minus(self); break;

			case Jump:
				current_instruction = current.index_of_object - 1; break;

			case If:
				if_operator(self, current.index_of_object, &current_instruction); break;

			case PushCurrentStateToReturn:
				push_return_stack(self, current_instruction + 1);  break;

			case Return:
				current_instruction = drop_return_stack(self);  break;

			case Skip:
				printf("Stack main top: %d", self->main_stack[self->main_stack_top-1].ivalue);
				return;  break;

			default:
				break;
		}
	}

	int gg = 0;
}

struct PyramidScriptState* create_state() {
	struct PyramidScriptState* state = malloc(sizeof(struct PyramidScriptState)); // state
	state->local_stack = calloc(30, sizeof(struct object));
	state->local_stack_top = 0;
		 
	state->return_stack = calloc(30, sizeof(uint32_t));
	state->return_stack_top = 0;
		 
	state->main_stack = calloc(30, sizeof(struct object));
	state->main_stack_top = 0;

	return state;
}


void vm_run(struct PyramidScriptState* self, const struct PyramidScriptByteCode program) {
	vm(self, program);
}
//void start_vm() {
//
//
//	struct PyramidScriptByteCode program;
//	program.program_size = 10;
//	program.code = calloc(program.program_size, sizeof(struct byte_code));
//	program.data_size = 10;
//	program.data = calloc(program.data_size, sizeof(struct object));
//
//	program.code[0].instructio = PushMainStack;
//	program.code[0].index_of_object = 0;
//
//	program.code[1].instructio = PushMainStack;
//	program.code[1].index_of_object = 1;
//
//	program.code[2].instructio = Plus;
//	program.code[2].index_of_object = 0;
//
//	program.code[3].instructio = PushMainStack;
//	program.code[3].index_of_object = 2;
//
//	program.code[4].instructio = If;
//	program.code[4].index_of_object = 0;
//
//	struct object value1 = { .ivalue = 1, .type = Integer };
//	struct object value2 = { .fvalue = 6.7, .type = Float };
//	struct object value3 = { .boolean = false, .type = Bool };
//
//	program.data[0] = value1;
//	program.data[1] = value2;
//	program.data[2] = value3;
//
//
//	vm(&state, program);
//}