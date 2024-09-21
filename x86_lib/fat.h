#ifndef __FAT_HEADER__
#define __FAT_HEADER__

#include <stdint.h>

struct FAT_Header {
    uint8_t unused_1[3];
    uint8_t oem[8];
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t fat_count;
    uint16_t root_dir_entries;
    uint16_t total_sector;
    uint8_t media;
    uint16_t sectors_per_FAT;
    uint16_t sector_per_track;
    uint16_t number_of_heads;
    uint32_t hidden_sector;
    uint32_t larger_sector;

    uint8_t drive_number;
    uint8_t reserved;
    uint8_t boot_signature;
    uint32_t volume_serial;
    uint8_t volume_name[11];
    uint8_t file_type[8];
} __attribute__((packed));

struct FAT_Directory {
    uint8_t filename[11];
    uint8_t attributes;
    uint8_t reserved;
    uint8_t creation_milisecond;
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t last_access_date;
    uint16_t reserved_fat32;
    uint16_t last_write_time;
    uint16_t last_write_data;
    uint16_t cluster;
    uint32_t file_size;
} __attribute__((packed));

typedef struct FAT_Header FileSystemInfo;
typedef struct FAT_Directory FileDirectory;

void FAT_Initialize();
void FAT_ShowListDirectory();
FileSystemInfo* GetInfoFileSystem();


#endif