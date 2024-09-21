#ifndef __DISK_HEADER__
#define __DISK_HEADER__

#include <stdint.h>

void Disk_Read(uint32_t lba, void* buffer);
void Disk_Write(uint32_t lba, void* data);

#endif