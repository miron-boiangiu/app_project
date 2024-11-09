#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "cbmp.h"

#include "image_editor.h"

int open_bmp_image(char* file_path, IMAGE* destination) {

    BMP* bmp = bopen(file_path);
    unsigned int width = get_width(bmp);
    unsigned int height = get_height(bmp);

    destination->width = width;
    destination->height = height;
    destination->rgb_grid = calloc(height, sizeof(uint8_t**));

    for (unsigned int i = 0; i < height; i++) {

        destination->rgb_grid[i] = calloc(width, sizeof(uint8_t*));

        for (unsigned int j = 0; j < width; j++) {
            destination->rgb_grid[i][j] = calloc(3, sizeof(uint8_t)); // 3 for R, G, B
        }
    }

    for (unsigned int x = 0; x < width; x++)
    {
        for (unsigned int y = 0; y < height; y++)
        {
            uint8_t r, g, b;
            get_pixel_rgb(bmp, x, y, &r, &g, &b);

            destination->rgb_grid[y][x][0] = r;  // Should we make this black & white only?
            destination->rgb_grid[y][x][1] = g;
            destination->rgb_grid[y][x][2] = b;
        }
    }

    bclose(bmp);
    return 0;
}

// original_image_path required cause the library is poor, will swap it later and fix this
// the poor library also means you cannot change the size of images
int save_bmp_image(char* original_image_path, char* file_path, IMAGE* image) {

    BMP* bmp = bopen(original_image_path);
    unsigned int width = get_width(bmp);
    unsigned int height = get_height(bmp);

    for (unsigned int x = 0; x < width; x++)
    {
        for (unsigned int y = 0; y < height; y++)
        {
            uint8_t r = image->rgb_grid[y][x][0];
            uint8_t g = image->rgb_grid[y][x][1];
            uint8_t b = image->rgb_grid[y][x][2];
            set_pixel_rgb(bmp, x, y, r, g, b);
        }
    }

    bwrite(bmp, file_path);
    bclose(bmp);
    return 0;
}

int apply_gaussian_blur(IMAGE* image) {
    return 0;
}

int free_image(IMAGE* image) {

    for (unsigned int i = 0; i < image->height; i++) {
        for (unsigned int j = 0; j < image->width; j++) {
            free(image->rgb_grid[i][j]);
        }
        free(image->rgb_grid[i]);
    }
    free(image->rgb_grid);

    return 0;
}
