#ifndef __CMD_HEADER__
#define __CMD_HEADER__

#include <stdint.h>

bool_t *sys_exit;
void CMD_Initialize();
void CMD_Command(char* buffer);

#endif