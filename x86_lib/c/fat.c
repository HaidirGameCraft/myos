#include <fat.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <disk.h>

#define FILE_CLUSTER_EOF 0x0FFFFFF8
#define FILE_SYSTEM_START_SECTOR 16



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

    printf("FAT Infomation\n");

    int RootDirRegion = FILE_SYSTEM_START_SECTOR + header_disk->reserved_sectors + ( header_disk->fat_count * header_disk->sectors_per_FAT );
    Disk_Read( RootDirRegion, buffer_disk );
    cd_root_dir = (FileDirectory*) buffer_disk;
}

void FAT_ShowListDirectory() {
    int FATRegion = FILE_SYSTEM_START_SECTOR + header_disk->reserved_sectors;
    int RootDirRegion = FILE_SYSTEM_START_SECTOR + header_disk->reserved_sectors + ( header_disk->fat_count * header_disk->sectors_per_FAT );
    int DataRegion = RootDirRegion;
    int range = DataRegion - RootDirRegion;
    uint8_t DirSizePerSector = header_disk->bytes_per_sector / sizeof( struct FAT_Directory );

    int dir_cluster = 2;
    char *filename = CREATE_ALLOC(char*, 12);
    while( dir_cluster < FILE_CLUSTER_EOF ) {
        memset( filename, 0, 12 );
        int region = RootDirRegion + (dir_cluster - 2) * header_disk->sectors_per_cluster;
        Disk_Read( region, buffer_disk);

        for(int i = 0; i < DirSizePerSector; i++) {
            FileDirectory* directory = &((FileDirectory*) buffer_disk)[i];
            if( directory->attributes == NULL || directory->attributes == 0x0F ) continue;

            memcpy(filename, directory, 11);
            print(filename);

            print("    ");
            char* size = itos( directory->file_size );
            print( size );

            print("    ");
            if( directory->attributes & FILE_ATTR_DIRECTORY ) {
                print("DIRECTORY");
            } else {
                print("FILE");
            }

            println("");

            free( size );
        }

        int fat_location_sector = (int)( dir_cluster / 128 ); // 128 -> is the counter FAT per bytes per sector
        int fat_index = dir_cluster % 128;
        Disk_Read(FATRegion + fat_location_sector, buffer_disk);
        dir_cluster = ((uint32_t*) buffer_disk)[fat_index];
    }

}

char* FAT_ReadFile(FileDirectory* file) {
    printf("Hai %i\n", ((int)(file->file_size / header_disk->bytes_per_sector) + 1) * 512);
    char* buffer = (char*) malloc( ((int)(file->file_size / header_disk->bytes_per_sector) + 1) * 512 );
    printf("Buffer Address: 0x%x\n", buffer);

    int FATRegion = FAT_CalculateFATRegion( FILE_SYSTEM_START_SECTOR, header_disk  );
    int DataRegion = FAT_CalculateDataRegion( FILE_SYSTEM_START_SECTOR, header_disk );
    int cluster = file->cluster;
    int next_ = 0;
    int size = file->file_size;

    printf("Size: %i\n", size);

    while( cluster < FILE_CLUSTER_EOF ) {
        int region = DataRegion + ( cluster - 2 ) * header_disk->sectors_per_cluster;

        Disk_Read( region, (void*)(buffer + next_) );
        next_ += 512;
        // if( size >= 512 )
        // {
        //     memcpy( buffer + next_, buffer_disk, 512 );
        //     next_ += header_disk->bytes_per_sector;
        //     size -= 512;
        // } else {
        //     memcpy( buffer + next_, buffer_disk, size );
        // }

        printf("Cluster: 0x%x\n", cluster);
        printf("Region: 0x%x\n", region * 512);

        cluster = FAT_CalculateNextCluster(FILE_SYSTEM_START_SECTOR, header_disk, cluster);
    }

    return buffer;
}

char* FAT_LoadFile(const char* filename) {
    int FATRegion = FILE_SYSTEM_START_SECTOR + header_disk->reserved_sectors;
    int RootDirRegion = FATRegion + ( header_disk->fat_count * header_disk->sectors_per_FAT );
    int DataRegion = RootDirRegion + ( header_disk->root_dir_entries * sizeof( struct FAT_Directory ) / header_disk->bytes_per_sector );
    int range = DataRegion - RootDirRegion;
    uint8_t DirSizePerSector = header_disk->bytes_per_sector / sizeof( struct FAT_Directory );

    // Seperate Name and extension
    char* fname = (char*) malloc( 8 );
    char* efname = (char*) malloc( 3 );
    memset( fname, 0x20, 8 );

    int count = 0;
    while( filename[count] != '.') {
        fname[count] = touppercase(filename[count]);
        count++;
    }

    memcpy( efname, filename + (count + 1), 3 );
    for(int i = 0; i < 3; i++) {
        efname[i] = touppercase(efname[i]);
    }

    FileDirectory* target_file = (FileDirectory*) malloc( sizeof( FileDirectory ) );
    memempty( target_file, sizeof( FileDirectory ) );

    int cluster_dir = 2;
    while( cluster_dir < FILE_CLUSTER_EOF ) {
        int region = RootDirRegion + ( ( cluster_dir - 2 ) * header_disk->sectors_per_cluster );
        Disk_Read( region, buffer_disk);
        for(uint8_t i = 0; i < DirSizePerSector; i++) {
            FileDirectory* directory = &((FileDirectory*) buffer_disk)[i];
            if( directory->attributes == NULL || directory->attributes == 0x0F ) continue;

            if( strncmp( directory->filename, fname, 8) != 0  || strncmp( directory->extension_file, efname, 3 ) != 0) continue;

            memcpy( target_file, directory, sizeof( FileDirectory ));
            goto read_file;
        }

        int FAT_index = cluster_dir % 128;
        int FAT_sector = (int)( cluster_dir / 128 );
        Disk_Read( FATRegion + FAT_sector, buffer_disk );
        cluster_dir = ((uint32_t*) buffer_disk)[FAT_index];
    }
read_file:
    free( fname );
    free( efname );
    if( target_file->attributes == NULL )
    {
        printf("FAT File Error> File not found\n");
        return NULL;
    }

    char* reserved_data = (char*) malloc( target_file->file_size );
    memempty( reserved_data, target_file->file_size);
    uint32_t addr = (uint32_t) reserved_data; // save the address


    uint32_t size = target_file->file_size;
    uint32_t cluster = target_file->cluster;
    uint16_t fat_sector = 0;
    Disk_Read(FATRegion, buffer_disk);

    while( cluster <  FILE_CLUSTER_EOF) {
        int region = RootDirRegion + ( cluster - 2 ) * header_disk->sectors_per_cluster;

        char* buffer = (char*) malloc( 512 );
        Disk_Read(region, buffer);
        memcpy(reserved_data, buffer, header_disk->bytes_per_sector);
        reserved_data += header_disk->bytes_per_sector;

        // if( size > 4096 ) {
        //     memcpy(reserved_data, (char*) local, 4096);
        //     reserved_data += 4096;
        //     size -= 4096;
        // } else {
        //     memcpy(reserved_data, (char*) local, size);
        //     size -= size;
        // }
        free( buffer );

        int FAT_index = cluster % 128;
        int FAT_sector = (int)( cluster / 128 );
        Disk_Read( FATRegion + FAT_sector, buffer_disk );
        cluster = ((uint32_t*) buffer_disk)[FAT_index];
    }

    return ( char* ) addr;
}

void FAT_FreeFile( void* buffer ) {
    free( buffer );
}

FileDirectory* FAT_FindFile(const char* name) {
    FileDirectory* trgt = (FileDirectory*) malloc( FILE_DIR_SIZE );
    memset( trgt, 0, FILE_DIR_SIZE);

    int sn_index = 0;
    int cluster = 2; // always starting 2
    while( sn_index < strlen(name) ) {
        char* fname = (char*) malloc(2);
        int size_fname = 2;

        // get string when spliting '/' or '\0'
        while(name[ sn_index ] != '/' && name[ sn_index ] != '\0') {
            fname[size_fname - 2] = name[sn_index];
            fname[size_fname - 1] = 0;
            size_fname++;
            char* tmp_n = (char*) malloc(size_fname);
            memcpy(tmp_n, fname, size_fname);
            free(fname);
            fname = tmp_n;
            sn_index++;
        }
        sn_index++;
        


        char* filename_ = (char*) malloc(8);
        char* ext_file = (char*) malloc(3);

        memset(filename_, 0x20, 8);
        memset(ext_file, 0x20, 3);
        
        int sidx = 0;
        while( fname[sidx] != 0 && sidx < 12 ) {
            if( fname[sidx] == '.' ) {
                sidx++;
                int i = 0;
                while( fname[sidx] != 0 && sidx < 12 ) {
                    ext_file[i] = touppercase(fname[sidx]);
                    sidx++;
                    i++;
                }
            } else {
                filename_[sidx] = touppercase(fname[sidx]);
                sidx++;
            }
        }

        while( cluster < FILE_CLUSTER_EOF ) {
            int FATRegion = FAT_CalculateFATRegion(FILE_SYSTEM_START_SECTOR, header_disk);
            int StartRegion = FAT_CalculateDataRegion(FILE_SYSTEM_START_SECTOR, header_disk);
            int cluster_crt = StartRegion + ( cluster - 2 ) * header_disk->sectors_per_cluster;
            
            // Read/Get directory
            Disk_Read( cluster_crt, buffer_disk );
            for(int i = 0; i < 16; i++) {
                FileDirectory* dir = &((FileDirectory*) buffer_disk)[i];
                if( dir->attributes == NULL || dir->attributes == 0x0F ) continue;

                if( strncmp(dir->filename, filename_, 8) == 0 && strncmp(dir->extension_file, ext_file, 3) == 0 ) {
                    if( dir->attributes & FILE_ATTR_DIRECTORY ) {
                        cluster = dir->cluster;
                        memcpy( trgt, dir, FILE_DIR_SIZE);
                        free(filename_);
                        free(ext_file);
                        free(fname);
                        goto next_dirs;
                    } else {
                        memcpy( trgt, dir, FILE_DIR_SIZE);
                        free(filename_);
                        free(ext_file);
                        free(fname);
                        goto find_file;
                    }
                }

            }

            int FATindex = cluster % 128;
            int FatSector = (int)(cluster / 128);
            Disk_Read(FATRegion + FatSector, buffer_disk);
            cluster = ((uint32_t*) buffer_disk)[FATindex];
        }

        printf("Error> File Not Found\n");
        free(filename_);
        free(ext_file);
        free(fname);
        return NULL;
next_dirs:
        continue;
    }
find_file:
    printf("File Found\n");
    return trgt;
}

int FAT_CalculateFATRegion(int start_sector, FAT32_Header* hdr) {
    return start_sector + hdr->reserved_sectors;
}
int FAT_CalculateDataRegion(int start_sector, FAT32_Header* hdr) {
    return FAT_CalculateFATRegion(start_sector, hdr) + ( hdr->fat_count * hdr->sectors_per_FAT );
}
int FAT_CalculateNextCluster(int start_sector, FAT32_Header* hdr, int cluster) {
    int FATIndex = cluster % 128;
    int FATReg = (int)( cluster / 128 );
    Disk_Read( FAT_CalculateFATRegion( start_sector, hdr ) + FATReg, buffer_disk);
    return ((uint32_t*) buffer_disk)[FATIndex];
}

FileDirectory* FAT_GetListDirectory(const char* path) {
    FileSystemInfo *hdr = header_disk;
    int DataReion = FAT_CalculateDataRegion(FILE_SYSTEM_START_SECTOR, header_disk);
    int FATRegion = FAT_CalculateFATRegion(FILE_SYSTEM_START_SECTOR, header_disk);

    FileDirectory* dir = FAT_FindFile(path);
    if( !(dir->attributes & FILE_ATTR_DIRECTORY) ) {
        return NULL;
    }


    FileDirectory* _list = NULL;
    
    int cluster = dir->cluster;
    int alloc_dir_index = 1;
    while( cluster < FILE_CLUSTER_EOF ) {
        int ccluster = DataReion + (cluster - 2) * header_disk->sectors_per_cluster;

        Disk_Read(ccluster, buffer_disk);
        for(int i = 0; i < 16; i++) {
            FileDirectory* dir = &((FileDirectory*) buffer_disk)[i];

            if( dir->attributes == NULL || dir->attributes == 0x0F ) continue;
            char* filename = (char*) malloc(12);
            memset(filename, 0, 12);

            memcpy(filename, dir, 11);
            print(filename);

            print("    ");
            char* size = itos( dir->file_size );
            print( size );
            free(size);

            print("    ");
            if( dir->attributes & FILE_ATTR_DIRECTORY ) {
                print("DIRECTORY");
            } else {
                print("FILE");
            }

            println("");
            free(filename);

            if( _list == NULL ) {
                _list = (FileDirectory*) malloc( alloc_dir_index * FILE_DIR_SIZE );
                memcpy( _list, dir, FILE_DIR_SIZE);
                alloc_dir_index++;
            } else {
                FileDirectory* l = (FileDirectory*) malloc( alloc_dir_index * FILE_DIR_SIZE );
                memset(l, 0, alloc_dir_index * FILE_DIR_SIZE);
                memcpy( l, _list, (alloc_dir_index - 1) * FILE_DIR_SIZE);
                memcpy(&l[alloc_dir_index - 1], dir, FILE_DIR_SIZE);
                free(_list);
                _list = l;
                alloc_dir_index++;
            }         

        }

        cluster = FAT_CalculateNextCluster(FILE_SYSTEM_START_SECTOR, hdr, cluster);
    }

    return _list;
}

FileSystemInfo* GetInfoFileSystem() {
    return header_disk;
}
