#ifndef __VESA__
#define __VESA__

#include "stdint.h"

typedef struct {
    char        signature[4];
    uint16_t    version;
    uint16_t    oemStringPtr[2];
    uint8_t     capabilitiess[4];
    uint16_t    videoModePtr[2];
    uint16_t    TotalMemory;
    uint8_t     reserved[492];
} __attribute__((packed)) VBE_Info;

typedef struct {
    uint16_t    attributes;
    uint8_t     winAattr, winBattr;
    uint16_t    WinGranularity;
    uint16_t    WinSize;
    uint16_t    WinASegment;
    uint16_t    WinBSegment;
    uint32_t    WinFuncPtr;
    uint16_t    BytesPerScanLine;

    uint16_t    width;
    uint16_t    height;
    uint8_t     char_width;
    uint8_t     char_height;
    uint8_t     planes;
    uint8_t     bpp;
    uint8_t     banks;
    uint8_t     memory_model;
    uint8_t     bank_size;
    uint8_t     images_pages;
    uint8_t     reserved1;

    uint8_t     red_mask_size;
    uint8_t     red_field_position;
    uint8_t     green_mask_size;
    uint8_t     green_field_position;
    uint8_t     blue_mask_size;
    uint8_t     blue_field_position;
    uint8_t     reserved_mask_size;
    uint8_t     reserved_field_position;
    uint8_t     direct_color_mode;

    uint32_t    physical_address;
    uint32_t    offscreen_segment;
    uint16_t    offscreen_offset;
    uint8_t     reserved2[256];
} __attribute__((packed)) VBE_Mode_Info;

#endif
