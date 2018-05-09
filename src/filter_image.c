#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "image.h"
#define TWOPI 6.2831853

void l1_normalize(image im)
{
    for (int c = 0; c < im.c; c++) {
        for (int y = 0; y < im.h; y++) {
            for (int x = 0; x < im.w; x++) {
                set_pixel(im,x,y,c,get_pixel(im,x,y,c)/(1.0*im.w*im.h));
            }
        }
    }
}

image make_box_filter(int w)
{
    image bf = make_image(w,w,1);
    for (int c = 0; c < bf.c; c++) {
        for (int y = 0; y < bf.h; y++) {
            for (int x = 0; x < bf.w; x++) {
                set_pixel(bf,x,y,c,1.0);
            }
        }
    }
    l1_normalize(bf);
    return bf;
}

image convolve_image(image im, image filter, int preserve)
{
    assert(filter.c == im.c || filter.c == 1);

    int x,y,c,fx,fy;
    float q, filter_val, pixel_val;

    image cim = (preserve == 1) ? make_image(im.w,im.h,im.c) : make_image(im.w,im.h,1);

    if (im.c == filter.c) {
        if (preserve == 0) {
            for (y = 0; y < cim.h; y++) {
                for (x = 0; x < cim.w; x++) {
                    q = 0;
                    for (c = 0; c < im.c; c++) {
                        for (fy = 0; fy < filter.h; fy++) {
                            for (fx = 0; fx < filter.w; fx++) {
                                filter_val = get_pixel(filter,fx,fy,c);
                                pixel_val = get_pixel(im, x-(filter.w/2)+fx, y-(filter.h/2)+fy,c) ;
                                q += filter_val*pixel_val;
                            }
                        }
                    }
                    set_pixel(cim,x,y,0,q);
                }
            }
        }
        else {
            for (y = 0; y < cim.h; y++) {
                for (x = 0; x < cim.w; x++) {
                    for (c = 0; c < im.c; c++) {
                        q = 0;
                        for (fy = 0; fy < filter.h; fy++) {
                            for (fx = 0; fx < filter.w; fx++) {
                                filter_val = get_pixel(filter,fx,fy,c);
                                pixel_val = get_pixel(im, x-(filter.w/2)+fx, y-(filter.h/2)+fy,c);
                                q += filter_val*pixel_val;
                            }
                        }
                        set_pixel(cim,x,y,c,q);
                    }
                }
            }
        }
    } else {
        if (preserve == 0) {
            for (y = 0; y < cim.h; y++) {
                for (x = 0; x < cim.w; x++) {
                    q = 0;
                    for (c = 0; c < im.c; c++) {
                        for (fy = 0; fy < filter.h; fy++) {
                            for (fx = 0; fx < filter.w; fx++) {
                                filter_val = get_pixel(filter,fx,fy,0);
                                pixel_val = get_pixel(im, x-(filter.w/2)+fx, y-(filter.h/2)+fy,c) ;
                                q += filter_val*pixel_val;
                            }
                        }
                    }
                    set_pixel(cim,x,y,0,q);
                }
            }
        } else {
            for (y = 0; y < cim.h; y++) {
                for (x = 0; x < cim.w; x++) {
                    for (c = 0; c < im.c; c++) {
                        q = 0;
                        for (fy = 0; fy < filter.h; fy++) {
                            for (fx = 0; fx < filter.w; fx++) {
                                filter_val = get_pixel(filter,fx,fy,0);
                                pixel_val = get_pixel(im, x-(filter.w/2)+fx, y-(filter.h/2)+fy,c);
                                q += filter_val*pixel_val;
                            }
                        }
                        // q = (q > 0.0) ? ((q < 1.0) ? q : 1.0) : 0.0;
                        set_pixel(cim,x,y,c,q);
                    }
                }
            }
        }
    }

    return cim;
}

image make_highpass_filter()
{
    image f = make_box_filter(3);
    set_pixel(f,0,0,0,0);
    set_pixel(f,1,0,0,-1);
    set_pixel(f,2,0,0,0);
    set_pixel(f,0,1,0,-1);
    set_pixel(f,1,1,0,4);
    set_pixel(f,2,1,0,-1);
    set_pixel(f,0,2,0,0);
    set_pixel(f,1,2,0,-1);
    set_pixel(f,2,2,0,0);
    return f;
}

image make_sharpen_filter()
{
    image f = make_box_filter(3);
    set_pixel(f,0,0,0,0);
    set_pixel(f,1,0,0,-1);
    set_pixel(f,2,0,0,0);
    set_pixel(f,0,1,0,-1);
    set_pixel(f,1,1,0,5);
    set_pixel(f,2,1,0,-1);
    set_pixel(f,0,2,0,0);
    set_pixel(f,1,2,0,-1);
    set_pixel(f,2,2,0,0);
    return f;
}

image make_emboss_filter()
{
    image f = make_box_filter(3);
    set_pixel(f,0,0,0,-2);
    set_pixel(f,1,0,0,-1);
    set_pixel(f,2,0,0,0);
    set_pixel(f,0,1,0,-1);
    set_pixel(f,1,1,0,1);
    set_pixel(f,2,1,0,1);
    set_pixel(f,0,2,0,0);
    set_pixel(f,1,2,0,1);
    set_pixel(f,2,2,0,2);
    return f;
}

// Question 2.2.1: Which of these filters should we use preserve when we run our convolution and which ones should we not? Why?
// Answer: We should use preserve on sharpen and emboss because their purpose is to add an effect to the image
// whereas the highpass filter is designed to look for features in the image where we don't care about color.
// i.e. an edge between a red and green thing should be treated the same as an edge between a blue and a red thing.

// Question 2.2.2: Do we have to do any post-processing for the above filters? Which ones and why?
// Answer: We need to do post-processing for any filter which does not sum to 0 (e.g. sharpen,emboss),
// because otherwise we may end up with pixel values >1 if sum is + or <0 if sum is -.

image make_gaussian_filter(float sigma)
{
    float G,gx,gy;
    image f = make_box_filter((6*sigma) + 1);
    for (int y = 0; y < f.h; y++) {
        for (int x = 0; x < f.w; x++) {
            gx = x - 3*sigma;
            gy = y - 3*sigma;
            G = (1.0/(TWOPI*powf(sigma,2))) * powf(M_E, -1.0*(powf(gx,2)+powf(gy,2))/(2*powf(sigma,2)));
            set_pixel(f,x,y,0,G);
        }
    }
    return f;
}

image add_image(image a, image b)
{
    assert(a.w == b.w && a.h == b.h && a.c == b.c);
    float new_val;
    image a_plus_b = make_image(a.w,a.h,a.c);
    for (int c = 0; c < a.c; c++) {
        for (int y = 0; y < a.h; y++) {
            for (int x = 0; x < a.w; x++) {
                new_val = get_pixel(a,x,y,c) + get_pixel(b,x,y,c);
                // new_val = (new_val > 1.0) ? 1.0 : new_val;
                set_pixel(a_plus_b,x,y,c,new_val);
            }
        }
    }
    return a_plus_b;
}

image sub_image(image a, image b)
{
    assert(a.w == b.w && a.h == b.h && a.c == b.c);
    float new_val;
    image a_minus_b = make_image(a.w,a.h,a.c);
    for (int c = 0; c < a.c; c++) {
        for (int y = 0; y < a.h; y++) {
            for (int x = 0; x < a.w; x++) {
                new_val = get_pixel(a,x,y,c) - get_pixel(b,x,y,c);
                // new_val = (new_val < 0.0) ? 0.0 : new_val;
                set_pixel(a_minus_b,x,y,c,new_val);
            }
        }
    }
    return a_minus_b;
}

image make_gx_filter()
{
    image f = make_box_filter(3);
    set_pixel(f,0,0,0,-1);
    set_pixel(f,1,0,0,0);
    set_pixel(f,2,0,0,1);
    set_pixel(f,0,1,0,-2);
    set_pixel(f,1,1,0,0);
    set_pixel(f,2,1,0,2);
    set_pixel(f,0,2,0,-1);
    set_pixel(f,1,2,0,0);
    set_pixel(f,2,2,0,1);
    return f;
}

image make_gy_filter()
{
    image f = make_box_filter(3);
    set_pixel(f,0,0,0,-1);
    set_pixel(f,1,0,0,-2);
    set_pixel(f,2,0,0,-1);
    set_pixel(f,0,1,0,0);
    set_pixel(f,1,1,0,0);
    set_pixel(f,2,1,0,0);
    set_pixel(f,0,2,0,1);
    set_pixel(f,1,2,0,2);
    set_pixel(f,2,2,0,1);
    return f;
}

void feature_normalize(image im)
{
    float v;
    float min = INFINITY;
    float max = -1.0 * INFINITY;
    // find min and max
    for (int c = 0; c < im.c; c++) {
        for (int y = 0; y < im.h; y++) {
            for (int x = 0; x < im.w; x++) {
                v = get_pixel(im,x,y,c);
                min = (v < min) ? v : min;
                max = (v > max) ? v : max;
            }
        }
    }
    float range = max - min;

    // subtract min, divide by the range
    for (int c = 0; c < im.c; c++) {
        for (int y = 0; y < im.h; y++) {
            for (int x = 0; x < im.w; x++) {
                set_pixel(im,x,y,c, (get_pixel(im,x,y,c)-min) / range);
            }
        }
    }
}

image *sobel_image(image im)
{
    image gx = make_gx_filter();
    image gy = make_gy_filter();

    image * sobel = calloc(2, sizeof(image));

    image xsobel = convolve_image(im,gx,0);
    image ysobel = convolve_image(im,gy,0);

    sobel[0] = make_image(im.w,im.h,1);
    sobel[1] = make_image(im.w,im.h,1);
    for (int y = 0; y < im.h; y++) {
        for (int x = 0; x < im.w; x++) {
            float mag = sqrtf( powf(get_pixel(xsobel,x,y,0),2) + powf(get_pixel(ysobel,x,y,0),2) );
            float dir = atan2f(get_pixel(ysobel,x,y,0), get_pixel(xsobel,x,y,0));
            set_pixel(sobel[0],x,y,0,mag);
            set_pixel(sobel[1],x,y,0,dir);
        }
    }

    return sobel;
}

image colorize_sobel(image im)
{
    image * sobel = sobel_image(im);
    feature_normalize(sobel[0]);
    feature_normalize(sobel[1]);

    image csobel = make_image(im.w,im.h,3);
    for (int c = 0; c < csobel.c; c++) {
        for (int y = 0; y < csobel.h; y++) {
            for (int x = 0; x < csobel.w; x++) {
                if (c == 0) {
                    set_pixel(csobel,x,y,c,get_pixel(sobel[1],x,y,0));
                } else {
                    set_pixel(csobel,x,y,c,get_pixel(sobel[0],x,y,0));
                }
            }
        }
    }
    hsv_to_rgb(csobel);
    return convolve_image(csobel,make_gaussian_filter(1),1);
}
