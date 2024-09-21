#include <vesa.h>

int width_screen = 0;
int height_screen = 0;
int bytes_per_pixel = 0;

uint16_t RGBto565bit( uint32_t color );

void Initialize_VBE_Info( VBE_Mode_Info* vbe_info ) {
    vbe_ptr = vbe_info;
    framebuffer = (uint8_t*) vbe_info->physical_address;
    width_screen = vbe_info->width;
    height_screen = vbe_info->height;
    bytes_per_pixel = vbe_info->bpp / 8;
}

int GetWidthVideo() {
    return vbe_ptr->width;
}
int GetHeightVideo() {
    return vbe_ptr->height;
}

uint32_t GetPixel(int x, int y) {
    int index = (x + y * width_screen) * bytes_per_pixel;

    uint32_t color = 0x00000000;
    color |= framebuffer[ index ] & 0xFF;
    color |= ( framebuffer[ index + 1 ] & 0xFF) << 8;
    color |= ( framebuffer[ index + 2 ] & 0xFF) << 16;
    return color;
}

void ClearScreen( uint32_t color ) {
    for( int y = 0; y < height_screen; y++ ) {
        for( int x = 0; x < width_screen; x++ ) {
            PutPixel( x, y, color);
        }
    }
}

void PutPixel(int x, int y, uint32_t color ) {
    int index = ( x + y * width_screen ) * bytes_per_pixel;
    framebuffer[ index ] = color & 0xFF;
    framebuffer[ index + 1 ] = (color >> 8) & 0xFF;
    framebuffer[ index + 2 ] = (color >> 16 ) & 0xFF;
}

void FillRect(int x, int y, int width, int height, uint32_t color ) {
    int index = x + y * width_screen;

    for(int i = y; i < height_screen; i++) {

        if( i >= y + height) break; // Offscreen Y

        for(int j = x; j <width_screen; j++) {
            if( j >= x + width ) break; // Offscreen X

            PutPixel( j, i, color );
        }
    }
}

void FillCircle(int x, int y, int radius, uint32_t color) {
    int X1 = x - radius;
    int Y1 = y - radius;
    int X2 = x + radius;
    int Y2 = y + radius;


    X1 = (X1 < 0) ? 0 : X1;
    X1 = (X1 >= width_screen) ? width_screen : X1;
    X2 = (X2 < 0) ? 0 : X2;
    X2 = (X2 >= width_screen) ? width_screen : X2;

    Y1 = (Y1 < 0) ? 0 : Y1;
    Y1 = (Y1 >= height_screen) ? height_screen : Y1;
    Y2 = (Y2 < 0) ? 0 : Y2;
    Y2 = (Y2 >= height_screen) ? height_screen : Y2;

    for(int i = Y1; i < Y2; i++) {
        for(int j = X1; j < X2; j++) {
            float dist = sqrt((j - x)*(j - x) + (i - y)*(i - y));
            if( dist <= (float) radius) {
                PutPixel(j, i, color);
            }
        }
    }
}

uint16_t RGBto565bit( uint32_t color ) {
    uint16_t red = color & 0xFF;
    uint16_t green = ( color >> 16 ) & 0xFF;
    uint16_t blue = ( color >> 24 ) & 0xFF;

    red = ( red * 31 ) / 0xFF;
    green = ( green * 63 ) / 0xFF;
    blue = ( blue * 31) / 0xFF;

    return ( red << 11 ) | ( green << 5 ) | blue;
}
