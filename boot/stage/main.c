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

extern void vesa_info_addr();
extern void vesa_mode_addr();

void main( ) { 
    VBE_Info* vesa_info = (VBE_Info*) vesa_info_addr;
    VBE_Mode_Info *vesamode = (VBE_Mode_Info*) vesa_mode_addr;

    VGA_ClearScreen();
    unsigned int lba = 30;
    uint8_t* kernel_buffer  = (uint8_t*) 0x00100000;
    uint8_t* entry_kernel   = (uint8_t*) 0x00100000;
    
    for(int i = 0; i < 512; i++) {
        reserved_buffer[i] = 0;
    }

    FAT32_Header header;
    int VOLUME_START = 16;
    FAT_ReadSector( VOLUME_START, reserved_buffer);
    for(int i = 0; i < sizeof( FAT32_Header ); i++) {
        ((uint8_t*) &header)[i] = reserved_buffer[i];
    }

    int FATSector = VOLUME_START + header.reserved_sectors;
    int RootDirSector = FATSector + (header.fat_count * header.sectors_per_FAT);
    int DataSector = RootDirSector;

    FAT_ReadSector( RootDirSector, reserved_buffer );
    FAT_Directory *kernel_dir = &((FAT_Directory*) reserved_buffer)[1];

    print("File Name: ");
    println( (char*) kernel_dir->filename );

    char sbuffer[30];
    htos( kernel_dir->cluster, sbuffer);
    print("Cluster Start: ");
    println( sbuffer );
    
    uint32_t cluster = kernel_dir->cluster;
    int eof = 0;

    FAT_ReadSector( FATSector, reserved_buffer);
    uint16_t* FAT_ALLOCATE = (uint16_t*) reserved_buffer;

    
    print("Cluster Start: ");
    println( sbuffer );

    while ( cluster < 0x0FFFFFF8 ) {
        int file_sector = DataSector + ( (cluster - 2) * header.sectors_per_cluster);

        for(int i = 0; i < header.sectors_per_cluster; i++) {
            FAT_ReadSector( file_sector + i, kernel_buffer );
            kernel_buffer += 512;
        }

        //htos( file_sector * 512 , sbuffer);
        //print("Address Data: ");
        //println( sbuffer );
        
        cluster = ((uint32_t*) reserved_buffer)[ cluster ];
        htos( cluster, sbuffer);
        print("Next Cluster: ");
        println( sbuffer );
    }

    char s[20];
    uhtos( vesamode->physical_address, s);
    println( s );

    KERNEL kernel = (KERNEL) entry_kernel;
    kernel(vesa_mode_addr);

    return;
}
