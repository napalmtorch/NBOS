#pragma once
#include <kernel/core/global.h>

extern uint32_t _kernel_start;
extern uint32_t _kernel_end;
extern uint32_t _stack_top;
extern uint32_t _stack_bottom;

void kernel_boot();
void kernel_main();
void kernel_parse_args();
int  idle_main(int argc, char** argv);

uint32_t kernel_get_size();