#include "stdint.h"
#include "string.h"
#include "fat.h"
#include "port.h"
#include "vga.h"
#include "vesa.h"
#include "stdio.h"

void main();

typedef void (*KERNEL)(VBE_Mode_Info* vbe_mode);
void FAT_ReadSector(uint32_t lba, void* buffer) {
    outb( 0x1f6, 0xe0 | ((lba >> 24) & 0x0f));

    outb(0x1f2, 1);
    outb(0x1f3, (uint8_t)(lba & 0xFF));
    outb(0x1f4, (uint8_t)((lba >> 8) & 0xFF));
    outb(0x1f5, (uint8_t)((lba >> 16) & 0xFF));

    outb(0x1f7, 0x20);

    while( inb(0x1f7) & 0x80 );

    for(int j = 0; j < 256; j++) {
        ((uint16_t*) buffer)[j] = inw(0x1f0);
    }
}

unsigned int page_directory[1024] __attribute__((aligned(4096)));
unsigned int page_table[1024][1024] __attribute__((aligned(4096)));

char reserved_buffer[512];

extern void vesa_addr();

void main( ) {
    // for(int i = 0; i < 1024; i++) {
    //     page_directory[i] = ((unsigned int) page_table[i]) | 7;
    // }

    // for(int i = 0; i < 1024; i++) {
    //     for(int j = 0; j < 1024; j++) {
    //         page_table[i][j] = (i * 1024 * 4096 + j * 4096) | 7;
    //     }
    // }

    // page_directory[768] = ((unsigned int) page_table[768])| 7;
    
    VBE_Mode_Info *vesamode = (VBE_Mode_Info*) vesa_addr;

    VGA_ClearScreen();
    unsigned int lba = 30;
    uint8_t* kernel_buffer = (uint8_t*) 0x00100000;
    
    for(int i = 0; i < 512; i++) {
        reserved_buffer[i] = 0;
    }

    FAT_Header header;
    int VOLUME_START = 30;
    FAT_ReadSector( VOLUME_START, reserved_buffer);
    for(int i = 0; i < sizeof( FAT_Header ); i++) {
        ((uint8_t*) &header)[i] = reserved_buffer[i];
    }

    int FATSector = VOLUME_START + header.reserved_sectors;
    int RootDirSector = FATSector + (header.fat_count * header.sectors_per_FAT);
    int DataSector = RootDirSector + ( header.root_dir_entries * 32 ) / header.bytes_per_sector;

    FAT_ReadSector( RootDirSector, reserved_buffer );
    FAT_Directory *kernel_dir = &((FAT_Directory*) reserved_buffer)[1];

    print("File Name: ");
    println( (char*) kernel_dir->filename );

    char sbuffer[30];
    htos( kernel_dir->cluster, sbuffer);
    print("Cluster Start: ");
    println( sbuffer );
    
    uint16_t cluster = kernel_dir->cluster;
    int eof = 0;


    FAT_ReadSector( FATSector, reserved_buffer);
    uint16_t* FAT_ALLOCATE = (uint16_t*) reserved_buffer;

    while ( cluster < 0xFFF8 ) {
        int file_sector = DataSector + ( (cluster - 2) * header.sectors_per_cluster);

        for(int i = 0; i < header.sectors_per_cluster; i++) {
            FAT_ReadSector( file_sector + i, kernel_buffer );
            kernel_buffer += 512;
        }

        htos( file_sector * 512 , sbuffer);
        print("Address Data: ");
        println( sbuffer );
        
        cluster = ((uint16_t*) reserved_buffer)[ cluster ];
        htos( cluster, sbuffer);
        print("Next Cluster: ");
        println( sbuffer );
    }

    // for(int i = 0; i < 256; i++) {
    //     page_table[768][i] = (0x00010000 + (i * 4096)) | 7;
    // }

    // asm volatile("mov %0, %%cr3" :: "r"(&page_directory));

    // unsigned int cr0;
    // asm volatile("mov %%cr0, %0" : "=r"(cr0));
    // cr0 |= 0x80000001;
    // asm volatile("mov %0, %%cr0" :: "r"(cr0));

    char s[20];
    uhtos( vesamode->physical_address, s);
    println( s );

    KERNEL kernel = (KERNEL) 0x00100000;
    kernel(vesa_addr);

    return;
}