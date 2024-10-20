#ifndef __PAGE_HEADER__
#define __PAGE_HEADER__

#include <stdint.h>

#define PAGE_SIZE                   0x1000
#define PAGE_PRESENT                1
#define PAGE_READ_WRITE             2
#define PAGE_USER                   4
#define PAGE_WRITE_THROUGH          8
#define PAGE_CACHE_DISABLE          16
#define PAGE_ACCESS                 32


void Initialize_Paging();
void Clear_Paging();
void SetVirtualMapping(uint32_t physical_address, uint32_t virtual_address, int size);
void ClearVirtualMapping(uint32_t virtual_address, int size);

#endif
