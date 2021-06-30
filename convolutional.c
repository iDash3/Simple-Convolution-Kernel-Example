#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

int main(void) {
    int width, height, channels;
    unsigned char* img = stbi_load("img/gab.jpg", &width, &height, &channels, 0);
    if(img == NULL){
        printf("Error ocurred while loading the image!");
        exit(1);
    }
    printf("Loaded the image with a size of %dx%d and channels %d \n", width, height, channels);

    size_t img_size = width * height * channels;
    int gray_channels = channels == 4 ? 2 : 1;
    size_t gray_img_size = width * height * gray_channels;

    unsigned char* gray_img = malloc(gray_img_size);
    if (gray_img == NULL) {
        printf("Unable to allocate the memory for gray image.\n");
        exit(1);
    }

    for (unsigned char *p = img, *pg = gray_img; p != img + img_size; p += channels, pg += gray_channels) {
        int i = (int)(pg) % (int)(gray_img);
        // Grayscale this mf
        *pg = ((*p + *(p + 1) + *(p + 2)) / 3.0);

        // I think this one is faster (we already know the nums are well behaved)
        // int p_check = ((int)(*pg) + shades_stop - 1) / shades_stop;
        // int p_check = (int)(*pg)/shades_stop + ((int)(*pg) % shades_stop != 0);
        // *pg = shades[p_check - 1];
        // Im fucking stupid: 
        // *pg = shades[(int)(*pg / shades_stop)];

        if(i%width == 0){
            *pg = '\n';
        } 

        // For alpha and shit
        if(channels == 4){
            *(pg + 1) = *(pg + 3);
        }
    }

    // const int kernel[3][3] = {{1,2,1},{2,4,2},{1,2,1}};
    const int kernel[3][3] = {{-1,-1,-1},{-1,9,-1},{-1,-1,-1}};
    // const int kernel[3][3] = {{1,1,1},{1,1,1},{1,1,1}};
    const int kernel_norm = kernel[0][0]+kernel[0][1]+kernel[0][2]+kernel[1][0]+kernel[1][1]+kernel[1][2]+kernel[2][0]+kernel[2][1]+kernel[2][2];
    unsigned char* conv_img = malloc(gray_img_size);
    if (conv_img == NULL) {
        printf("Unable to allocate the memory for gray image.\n");
        exit(1);
    }

    int ii = 2;
    for(unsigned char *pg = gray_img, *cp = conv_img; pg != gray_img + gray_img_size; pg += gray_channels, cp += gray_channels){
        int a00, a01, a02, a10, a11, a12, a20,a21,a22;

        int row_num = ii / width;
        int row_num_next = (ii+1) / width;
        int row_num_prev = (ii-1) / width;

        int col_num = 0;
        if(row_num != row_num_next || row_num != row_num_prev) {
            col_num = 1;
        }

        if(row_num == 0 || row_num == height -1){
            a00 = a01 = a02 = a10 = a11 = a12 = a20 = a21 = a22 = 1;
        }
        else if(col_num == 1){
            a00 = a01 = a02 = a10 = a11 = a12 = a20 = a21 = a22 = 1;
        } else {
            a00 = *(pg - width - gray_channels) * kernel[0][0];
            a01 = *(pg - width) * kernel[0][1];
            a02 = *(pg - width + gray_channels) * kernel[0][2];;
            a10 = *(pg - gray_channels) * kernel[1][0];;
            a11 = *(pg) * kernel[1][1];;
            a12 = *(pg + gray_channels) * kernel[1][2];;
            a20 = *(pg + width - gray_channels) * kernel[2][0];;
            a21 = *(pg + width) * kernel[2][1];;
            a22 = *(pg + width + gray_channels) * kernel[2][2];;
        }

        int conv = (a00 + a01 + a02 + a10 + a11 + a12 + a20 + a21 + a22) / kernel_norm;
        *cp = conv;
        ii++;
    }

    // Print the results
    // stbi_write_png("./original.png", width, height, channels, img, width * channels);
    // stbi_write_jpg("./gray.jpg", width, height, gray_channels, gray_img, 100);
    stbi_write_jpg("./res/conv.jpg", width, height, gray_channels, conv_img, 100);

    // Free mem
    stbi_image_free(img);
    free(gray_img);
    free(conv_img);
}
