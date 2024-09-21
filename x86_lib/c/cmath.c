#include <cmath.h>

int floori(int value) {
    return (int) value;
}

int abs( int val ) {
    return ( val < 0 ) ? -1 * val : val;
}

float pow( float val, int x ) {
    float r = val;
    for(int i = 0; i < x; i++) {
        r *= val;
    }
    return r;
}
float sqrt( float val ) {
    float r = 5;
    for(int i = 0; i < 10; i++) {
        r = 0.5f * ( r + val / r );
    }

    return r;
}