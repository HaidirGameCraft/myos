#ifndef __FAT_HEADER__
#define __FAT_HEADER__

#include <stdint.h>

#define FILE_ATTR_READ_ONLY 0x01
#define FILE_ATTR_HIDDEN 0x02
#define FILE_ATTR_SYSTEM 0x04
#define FILE_VOLUME_ID 0x08
#define FILE_ATTR_DIRECTORY 0x10
#define FILE_ATTR_ARCHIVE 0x20

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

typedef struct {
    uint8_t code[3];
    uint8_t oem[8];
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t fat_count;
    uint16_t root_dir_entries;
    uint16_t total_sector;
    uint8_t media;
    uint16_t unused1;
    uint16_t sector_per_track;
    uint16_t number_of_heads;
    uint32_t hidden_sector;
    uint32_t larger_sector;

    uint32_t sectors_per_FAT;
    uint16_t flags;
    uint16_t version;
    uint32_t root_dir_cluster;
    uint16_t fs_info;
    uint16_t backup_boot_sector;
    uint8_t reserved[12];

    uint8_t drive_number, unused2;
    uint8_t boot_signature;
    uint32_t volume_serial;
    uint8_t volume_name[11];
    uint8_t file_type[8];
} __attribute__((packed)) FAT32_Header;

struct FAT_Directory {
    uint8_t filename[8];
    uint8_t extension_file[3];
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

typedef struct {
    uint8_t order_entry;
    uint8_t first_name[10];
    uint8_t attribute;
    uint8_t entry_type;
    uint8_t check_sum;
    uint8_t second_name[12];
    uint16_t unused;
    uint8_t final_name[4];
} __attribute__((packed)) FAT_LSN;

typedef FAT32_Header FileSystemInfo;
typedef struct FAT_Directory FileDirectory;
typedef FAT_LSN FileLongName;

#define FILE_DIR_SIZE sizeof( FileDirectory )
#define FILE_HDR_SIZE sizeof( FileSystemInfo )
#define FILE_LNS_SIZE sizeof( FileLongName )

void FAT_Initialize();
void FAT_ShowListDirectory();

int FAT_CalculateFATRegion(int start_sector, FAT32_Header* hdr);
int FAT_CalculateDataRegion(int start_sector, FAT32_Header* hdr);
int FAT_CalculateNextCluster(int start_sector, FAT32_Header* hdr, int cluster);

FileDirectory* FAT_GetListDirectory(const char* path);
FileDirectory* FAT_FindFile(const char* path);

char* FAT_ReadFile(FileDirectory* file);

char* FAT_LoadFile(const char* filename);
void FAT_FreeFile( void* buffer );
FileSystemInfo* GetInfoFileSystem();


#endif
