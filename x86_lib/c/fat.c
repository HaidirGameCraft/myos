#include <fat.h>
#include <string.h>
#include <memory.h>
#include <disk.h>

#define FILE_SYSTEM_START_SECTOR 30

FileDirectory* cd_root_dir = NULLPTR;
FileSystemInfo* header_disk = NULLPTR;
uint8_t* buffer_disk = NULLPTR;

void FAT_Initialize() {
    header_disk = (FileSystemInfo*) malloc( sizeof( FileSystemInfo ));
    buffer_disk = (uint8_t*) malloc( 512 );
    cd_root_dir = (FileDirectory*) malloc( sizeof( FileDirectory ) );

    Disk_Read( FILE_SYSTEM_START_SECTOR, buffer_disk );

    for(int i = 0; i < sizeof( FileSystemInfo ); i++ ) {
        ((uint8_t*) header_disk)[i] = buffer_disk[i];
    }

    int RootDirRegion = FILE_SYSTEM_START_SECTOR + header_disk->reserved_sectors + ( header_disk->fat_count * header_disk->sectors_per_FAT );
    Disk_Read( RootDirRegion, buffer_disk );
    cd_root_dir = (FileDirectory*) buffer_disk;
}

void FAT_ShowListDirectory() {
    int RootDirRegion = FILE_SYSTEM_START_SECTOR + header_disk->reserved_sectors + ( header_disk->fat_count * header_disk->sectors_per_FAT );
    uint8_t DirSizePerSector = header_disk->bytes_per_sector / sizeof( struct FAT_Directory );

    int count = 0;
    println("=================================================================");

    Disk_Read( RootDirRegion, buffer_disk);
    for(uint8_t i = 0; i < DirSizePerSector; i++) {
        FileDirectory* directory = &((FileDirectory*) buffer_disk)[i];
        if( directory->attributes == NULL ) break;

        char* filename = directory->filename;
        print(filename);
        print(", ");

        char* size = itos( directory->file_size );
        println( size );
        free( size );
    }
}

FileSystemInfo* GetInfoFileSystem() {
    return header_disk;
}