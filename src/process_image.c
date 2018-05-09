#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "image.h"

float get_pixel(image im, int x, int y, int c)
{
    // TODO Fill this in
    x = (x >= 0) ? ( (x < im.w) ? x : im.w-1 ) : 0;
    y = (y >= 0) ? ( (y < im.h) ? y : im.h-1 ) : 0;
    c = (c >= 0) ? ( (c < im.c) ? c : im.c-1 ) : 0;

    return im.data[x + y*im.w + c*im.w*im.h];
}

void set_pixel(image im, int x, int y, int c, float v)
{
    if (x < 0 || x >= im.w || y < 0 || y >= im.h || c < 0 || c >= im.c) {
        return;
    }

    im.data[x + y*im.w + c*im.w*im.h] = v;
}

image copy_image(image im)
{
    image copy = make_image(im.w, im.h, im.c);
    
    memcpy(copy.data, im.data, im.w*im.h*im.c*sizeof(float));

    return copy;
}

image rgb_to_grayscale(image im)
{
    // Y' = 0.299 R' + 0.587 G' + .114 B'
    assert(im.c == 3);
    image gray = make_image(im.w, im.h, 1);
    float r,g,b,z;
    for (int x = 0; x < im.w; x++) {
        for (int y = 0; y < im.h; y++) {
            r = get_pixel(im,x,y,0);
            g = get_pixel(im,x,y,1);
            b = get_pixel(im,x,y,2);

            z = 0.299*r + 0.587*g + 0.114*b;
            set_pixel(gray,x,y,0,z);
        }
    }
    return gray;
}

void shift_image(image im, int c, float v)
{
    for (int x = 0; x < im.w; x++) {
        for (int y = 0; y < im.h; y++) {
            set_pixel(im,x,y,c, get_pixel(im,x,y,c) + v);
        }
    }
}

void clamp_image(image im)
{
    for (int c = 0; c < im.c; c++) {
        for (int y = 0; y < im.h; y++) {
            for (int x = 0; x < im.w; x++) {
                float v = get_pixel(im,x,y,c);
                v = (v >= 0) ? ( (v <= 1) ? v : 1.0 ) : 0.0;
                set_pixel(im,x,y,c,v);
            }
        }
    }
}


// These might be handy
float three_way_max(float a, float b, float c)
{
    return (a > b) ? ( (a > c) ? a : c) : ( (b > c) ? b : c) ;
}

float three_way_min(float a, float b, float c)
{
    return (a < b) ? ( (a < c) ? a : c) : ( (b < c) ? b : c) ;
}

void rgb_to_hsv(image im)
{
    float r,g,b;
    float H,S,V,C,m ;
    for (int y = 0; y < im.h; y++) {
        for (int x = 0; x < im.w; x++) {
            r = get_pixel(im,x,y,0);
            g = get_pixel(im,x,y,1);
            b = get_pixel(im,x,y,2);

            V = three_way_max(r,g,b);
            m = three_way_min(r,g,b);
            C = V - m;
            S = (V==0) ? 0 : C / V;
            H = (C == 0) ? 0 :
                (V == r) ? (g-b)/C :
                (V == g) ? ((b-r)/C)+2 : ((r-g)/C)+4;

            H = (H < 0) ? H + 6 : H;

            H = H/6;


            set_pixel(im,x,y,0,H);
            set_pixel(im,x,y,1,S);
            set_pixel(im,x,y,2,V);
        }
    }

}

void hsv_to_rgb(image im)
{
    float H,S,V,C,m;
    float r,g,b;

    for (int y = 0; y < im.h; y++) {
        for (int x = 0; x < im.w; x++) {
            H = get_pixel(im,x,y,0);
            S = get_pixel(im,x,y,1);
            V = get_pixel(im,x,y,2);

            H = H*6;
            C = S*V;
            m = V-C;


            if (S == 0) {
                r = V;
                g = V;
                b = V;
            } else if (H >= 0 && H <= 1) {
                // R is max, G > B
                r = V;
                b = m;
                g = H*C+b;
            } else if (H > 1 && H <= 2) {
                // G is max, R > B
                g = V;
                b = m;
                r = b-C*(H-2);
            } else if (H > 2 && H <= 3) {
                // G is max, B > R
                g = V;
                r = m;
                b = C*(H-2)+r;
            } else if (H > 3 && H <= 4) {
                // B is max, G > R
                b = V;
                r = m;
                g = r - C*(H-4);
            } else if (H > 4 && H <= 5) {
                // B is max, R > G
                b = V;
                g = m;
                r = C*(H-4) + g;
            } else {
                // R is max, B > G
                H = H - 6;
                r = V;
                g = m;
                b = g - C*H;
            }

            set_pixel(im,x,y,0,r);
            set_pixel(im,x,y,1,g);
            set_pixel(im,x,y,2,b);
        }
    }
}
