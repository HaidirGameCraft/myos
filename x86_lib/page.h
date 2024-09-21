#ifndef __PAGE_HEADER__
#define __PAGE_HEADER__

#include <stdint.h>

void Initialize_Paging();
void SetVirtualMapping(uint32_t physical_address, uint32_t virtual_address);

#endif