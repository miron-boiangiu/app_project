#ifndef IMAGE_EDITOR_H
#define IMAGE_EDITOR_H

#include <stdint.h>

typedef struct image {
    uint8_t** pixel_grid;  // Monochrome
    int width;
    int height;
} IMAGE;

int open_bmp_image(char* file_path, IMAGE* destination);
int save_bmp_image(char* original_image_path, char* file_path, IMAGE* image);  // TODO: retarded
int apply_gaussian_blur(IMAGE* image);
int free_image(IMAGE* image);

#endif // IMAGE_EDITOR_H
