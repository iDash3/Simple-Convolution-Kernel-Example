#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

int main(void) {
    // Read image
    int width, height, channels;
    unsigned char* img = stbi_load("res/blur.jpg", &width, &height, &channels, 0);
    if(img == NULL){
        printf("Error ocurred while loading the image!");
        exit(1);
    }
    printf("Loaded the image with a size of %dx%d and channels %d \n", width, height, channels);

    // Image details
    size_t img_size = width * height * channels;
    int gray_channels = channels == 4 ? 2 : 1;
    size_t gray_img_size = width * height * gray_channels;

    // Load gray image
    unsigned char* gray_img = malloc(gray_img_size);
    if (gray_img == NULL) {
        printf("Unable to allocate the memory for gray image.\n");
        exit(1);
    }

    // Make gray image
    for (unsigned char *p = img, *pg = gray_img; p != img + img_size; p += channels, pg += gray_channels) {
        int i = (int)(pg) % (int)(gray_img);
        // Grayscale
        *pg = ((*p + *(p + 1) + *(p + 2)) / 3.0);

        if(i%width == 0){
            *pg = '\n';
        } 

        // Alpha
        if(channels == 4){
            *(pg + 1) = *(pg + 3);
        }
    }

    // Load conv image
    const int kernel[3][3] = {{-1,-1,-1},{-1,6,-1},{-1,-1,-1}};
    // const int kernel[3][3] = {{1,2,1},{2,4,2},{1,2,1}};
    // const int kernel[3][3] = {{1,1,1},{1,1,1},{1,1,1}};
    const int kernel_norm = kernel[0][0]+kernel[0][1]+kernel[0][2]+kernel[1][0]+kernel[1][1]+kernel[1][2]+kernel[2][0]+kernel[2][1]+kernel[2][2];
    unsigned char* conv_img = malloc(gray_img_size);
    if (conv_img == NULL) {
        printf("Unable to allocate the memory for conv image.\n");
        exit(1);
    }

    // Make conv image
    int ii = 0;
    for(unsigned char *pg = gray_img, *cp = conv_img; pg != gray_img + gray_img_size; pg += gray_channels, cp += gray_channels){
        // Kernel matrix
        int a00, a01, a02, a10, a11, a12, a20,a21,a22;

        int row_num = ii / width;
        int row_num_next = (ii+1) / width;
        int row_num_prev = (ii-1) / width;

        // Jump the first and last row / column
        int col_num = 0;
        if(row_num != row_num_next || row_num != row_num_prev) {
            col_num = 1;
        }

        if(row_num == 0 || row_num == height - 1){
            a00 = a01 = a02 = a10 = a11 = a12 = a20 = a21 = a22 = 0;
        }
        else if(col_num == 1){
            a00 = a01 = a02 = a10 = a11 = a12 = a20 = a21 = a22 = 0;
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
        // int conv = (a00 + a01 + a02 + a10 + a11 + a12 + a20 + a21 + a22);
        *cp = conv;
        ii++;
    }

    // Print the results
    // stbi_write_png("./original.png", width, height, channels, img, width * channels);
    // stbi_write_jpg("./gray.jpg", width, height, gray_channels, gray_img, 100);
    stbi_write_jpg("./res/conv.jpg", width, height, gray_channels, conv_img, 100);

    // Free memory
    stbi_image_free(img);
    free(gray_img);
    free(conv_img);
}
