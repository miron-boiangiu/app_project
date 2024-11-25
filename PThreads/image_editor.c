#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <pthread.h>

#include "cbmp.h"

#include "image_editor.h"

#define GAUSSIAN_K_WIDTH 3
#define GAUSSIAN_K_HEIGHT 3

// Constants

float gaussian[GAUSSIAN_K_HEIGHT][GAUSSIAN_K_WIDTH] = {
    {0.07625, 0.11, 0.07625},
    {0.11, 0.255, 0.11},
    {0.07625, 0.11, 0.07625}
};  // https://en.wikipedia.org/wiki/Gaussian_blur

// Functions

int open_bmp_image(char* file_path, IMAGE* destination) {

    BMP* bmp = bopen(file_path);
    unsigned int width = get_width(bmp);
    unsigned int height = get_height(bmp);

    destination->width = width;
    destination->height = height;

    destination->pixel_grid = calloc(height, sizeof(uint8_t*));
    for (unsigned int i = 0; i < height; i++) {
        destination->pixel_grid[i] = calloc(width, sizeof(uint8_t));
    }

    for (unsigned int x = 0; x < width; x++)
    {
        for (unsigned int y = 0; y < height; y++)
        {
            uint8_t r, g, b;
            get_pixel_rgb(bmp, x, y, &r, &g, &b);

            destination->pixel_grid[y][x] = 0.3* r + 0.59 * g + 0.11 * b;  // https://en.wikipedia.org/wiki/Grayscale
        }
    }

    bclose(bmp);
    return 0;
}

// original_image_path required cause the library is poor, will swap it later and fix this
// the poor library also means you cannot change the size of images... 💩 library
int save_bmp_image(char* original_image_path, char* file_path, IMAGE* image) {

    BMP* bmp = bopen(original_image_path);
    unsigned int width = get_width(bmp);
    unsigned int height = get_height(bmp);

    for (unsigned int x = 0; x < width; x++)
    {
        for (unsigned int y = 0; y < height; y++)
        {
            uint8_t grayscale_rgb = image->pixel_grid[y][x];
            set_pixel_rgb(bmp, x, y, grayscale_rgb, grayscale_rgb, grayscale_rgb);
        }
    }

    bwrite(bmp, file_path);
    bclose(bmp);
    return 0;
}

// TODO: change kernel to float**
uint8_t apply_convolution_at_pos(uint8_t** matrix, int width, int height, float kernel[GAUSSIAN_K_HEIGHT][GAUSSIAN_K_WIDTH], int k_width, int k_height, int pos_x, int pos_y) {
    int res = 0;
    int delta_x = k_width / 2;
    int delta_y = k_height / 2;

    for(int kernel_y = 0; kernel_y < k_height; kernel_y++) {
        for (int kernel_x = 0; kernel_x < k_width; kernel_x++) {

            int corresponding_matrix_x = pos_x - delta_x + kernel_x;
            int corresponding_matrix_y = pos_y - delta_y + kernel_y;

            // Check if inside boundaries
            if (corresponding_matrix_x < 0 || corresponding_matrix_y < 0
                || corresponding_matrix_x >= width || corresponding_matrix_y >= height) {
                    //printf("Out.\n");
                return matrix[pos_y][pos_x];
            }

            //printf("%d %d %d %d\n", kernel_y, kernel_x, corresponding_matrix_y, corresponding_matrix_x);

            res += kernel[kernel_y][kernel_x] * matrix[corresponding_matrix_y][corresponding_matrix_x];

        }
    }

    return res > 255 ? 255 : res;
}

typedef struct thread_params {
    uint8_t** new_matrix;
    uint8_t** matrix;
    int width;
    int height;
    int k_width;
    int k_height;
    int thread_id;
    int thread_count;
    float kernel[GAUSSIAN_K_HEIGHT][GAUSSIAN_K_WIDTH];
} thread_params;

void* thread_func(void* arg) {
    thread_params* params = (thread_params*) arg;

    int start_index = params->thread_id * (double) (params->height) / (params->thread_count);
    int end_index = fmin(params->height, ((params->thread_id) + 1) * (double) (params->height) / (params->thread_count));

    for (int y = start_index; y < end_index; y++) {
        for (int x = 0; x < params->width; x++) {
            (params->new_matrix)[y][x] = apply_convolution_at_pos(params->matrix, params->width, params->height, gaussian, params->k_width, params->k_height, x, y);
        }
    }

    // Copy values
    for (int y = start_index; y < end_index; y++) {
        for (int x = 0; x < params->width; x++) {
            (params->matrix)[y][x] = (params->new_matrix)[y][x];
        }
    }

    free(arg);
    pthread_exit(0);
}

// TODO: change kernel to float**
void apply_convolution(uint8_t** matrix, int width, int height, float kernel[GAUSSIAN_K_HEIGHT][GAUSSIAN_K_WIDTH], int k_width, int k_height, int thread_count) {

    uint8_t** new_matrix = calloc(height, sizeof(uint8_t*));
    for (unsigned int i = 0; i < height; i++) {
        new_matrix[i] = calloc(width, sizeof(uint8_t));
    }

    pthread_t* threads = calloc(sizeof(pthread_t), thread_count);

    for (int id = 0; id < thread_count; id++) {
        thread_params* params = calloc(1, sizeof(thread_params));

        // TODO: Flyweight design pattern would work wonders here.
        params->new_matrix = new_matrix;
        params->matrix = matrix;
        params->width = width;
        params->height = height;
        params->k_height = k_height;
        params->k_width = k_width;
        params->thread_id = id;
        params->thread_count = thread_count;

        pthread_create(&threads[id], NULL, thread_func, (void *)params);
    }
 
    for (int id = 0; id < thread_count; id++) {
        pthread_join(threads[id], NULL);
    }

    for (unsigned int i = 0; i < height; i++) {
        free(new_matrix[i]);
    }
    free(new_matrix);
    free(threads);
}

int apply_gaussian_blur(IMAGE* image, int thread_count) {
    apply_convolution(image->pixel_grid, image->width, image->height, gaussian, GAUSSIAN_K_WIDTH, GAUSSIAN_K_HEIGHT, thread_count);
    return 0;
}

int free_image(IMAGE* image) {

    for (unsigned int i = 0; i < image->height; i++) {
        free(image->pixel_grid[i]);
    }
    free(image->pixel_grid);

    return 0;
}
