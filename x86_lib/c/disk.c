#include <disk.h>

#include <driver/port.h>

#define ATA_COMMAND 0x7
#define ATA_SECTOR_COUNT 0x2
#define ATA_LBA_LOW 0x3
#define ATA_LBA_MID 0x4
#define ATA_LBA_HIGH 0x5
#define ATA_IO_BASE 0x0
#define ATA_DISK_SELECT 0x6
#define ATA_STATUS 0x7

#define SECTOR_SIZE 512

#define DRIVE_MASTER 0xE0
#define DRIVE_SLAVE 0xF0

#define ATA_CMD_READ 0x20
#define ATA_CMD_WRITE 0x30

#define PRIMARY_DISK 0x1F0
#define SECONDARY_DISK 0x170

void Disk_Read(uint32_t lba, void* buffer) {
    outb( PRIMARY_DISK + ATA_DISK_SELECT, DRIVE_MASTER | ( (lba >> 24) & 0x0F ));

    outb( PRIMARY_DISK + ATA_SECTOR_COUNT, 1);
    outb( PRIMARY_DISK + ATA_LBA_LOW, (uint8_t)( lba & 0xFF ));
    outb( PRIMARY_DISK + ATA_LBA_MID, (uint8_t) (lba >> 8) & 0xFF);
    outb( PRIMARY_DISK + ATA_LBA_HIGH, (uint8_t) (lba >> 16) & 0xFF);

    // Send Command Read
    outb( PRIMARY_DISK + ATA_COMMAND, 0x20 );

    while( inb( PRIMARY_DISK + ATA_STATUS ) & 0x80 );

    for( int i = 0; i < SECTOR_SIZE / 2; i++ ) {
        ((uint16_t*) buffer)[i] = inw( PRIMARY_DISK + ATA_IO_BASE );
    }
}

void Disk_Write(uint32_t lba, void* data) {
    outb( PRIMARY_DISK + ATA_DISK_SELECT, 0xE0 | ( (lba >> 24) & 0x0F ));

    outb( PRIMARY_DISK + ATA_SECTOR_COUNT, 1);
    outb( PRIMARY_DISK + ATA_LBA_LOW, (uint8_t)( lba & 0xFF ));
    outb( PRIMARY_DISK + ATA_LBA_MID, (uint8_t) (lba >> 8) & 0xFF);
    outb( PRIMARY_DISK + ATA_LBA_HIGH, (uint8_t) (lba >> 16) & 0xFF);

    // Send Command Write
    outb( PRIMARY_DISK + ATA_COMMAND, ATA_CMD_WRITE);

    while( inb( PRIMARY_DISK + ATA_STATUS ) & 0x80 );

    for( int i = 0; i < SECTOR_SIZE / 2; i++ ) {
        outw( PRIMARY_DISK + ATA_IO_BASE, ((uint16_t*) data)[i]);
    }

    outb( PRIMARY_DISK + ATA_COMMAND, 0xE7); // Cache Flush: after write the data to disk
}