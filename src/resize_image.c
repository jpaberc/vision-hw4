#include <math.h>
#include "image.h"

float nn_interpolate(image im, float x, float y, int c)
{
    int nn_x = (int) roundf(x);
    int nn_y = (int) roundf(y);

    return get_pixel(im,nn_x,nn_y,c);
}

image nn_resize(image im, int w, int h)
{
    image rs_im = make_image(w,h,im.c);

    float otl_x = -0.5;
    float otl_y = -0.5;
    float obr_x = im.w - 0.5;
    float obr_y = im.h - 0.5;

    float ntl_x = -0.5;
    float ntl_y = -0.5;
    float nbr_x = w - 0.5;
    float nbr_y = h - 0.5;

    // a*new + b = old
    float ax = (obr_x - otl_x) / (nbr_x - ntl_x);
    float bx = otl_x - (ax * ntl_x);

    float ay = (obr_y - otl_y) / (nbr_y - ntl_y);
    float by = otl_y - (ay * ntl_y);


    float nx,ny;
    for (int k = 0; k < rs_im.c; k++) {
    	for (int y = 0; y < rs_im.h; y++) {
    		for (int x = 0; x < rs_im.w; x++) {

    			nx = ax*x + bx;
    			ny = ay*y + by;
    			set_pixel(rs_im,x,y,k,nn_interpolate(im,nx,ny,k));

    		}
    	}
    }
    return rs_im;
}

float bilinear_interpolate(image im, float x, float y, int c)
{

    int l = (int) floorf(x);
    int r = (int) ceilf(x);
    int t = (int) floorf(y);
    int b = (int) ceilf(y);

    float q1 = (ceilf(y) - y) * get_pixel(im,l,t,c) + (y - floorf(y)) * get_pixel(im,l,b,c);

    float q2 = (ceilf(y) - y) * get_pixel(im,r,t,c) + (y - floorf(y)) * get_pixel(im,r,b,c);

    return (ceilf(x) - x) * q1 + (x - floorf(x)) * q2;
}

image bilinear_resize(image im, int w, int h)
{
    image rs_im = make_image(w,h,im.c);

    float otl_x = -0.5;
    float otl_y = -0.5;
    float obr_x = im.w - 0.5;
    float obr_y = im.h - 0.5;

    float ntl_x = -0.5;
    float ntl_y = -0.5;
    float nbr_x = w - 0.5;
    float nbr_y = h - 0.5;

    // a*new + b = old
    float ax = (obr_x - otl_x) / (nbr_x - ntl_x);
    float bx = otl_x - (ax * ntl_x);

    float ay = (obr_y - otl_y) / (nbr_y - ntl_y);
    float by = otl_y - (ay * ntl_y);


    float nx,ny;
    for (int k = 0; k < rs_im.c; k++) {
    	for (int y = 0; y < rs_im.h; y++) {
    		for (int x = 0; x < rs_im.w; x++) {

    			nx = ax*x + bx;
    			ny = ay*y + by;
    			set_pixel(rs_im,x,y,k,bilinear_interpolate(im,nx,ny,k));

    		}
    	}
    }
    return rs_im;
}