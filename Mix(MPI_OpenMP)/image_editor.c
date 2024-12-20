#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <omp.h>
#include <mpi.h>

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

// TODO: change kernel to float**
void apply_convolution(uint8_t** matrix, int width, int height, float kernel[GAUSSIAN_K_HEIGHT][GAUSSIAN_K_WIDTH], int k_width, int k_height) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int mpi_start = rank * (double) height / size;
    int mpi_end = fmin(height, (rank + 1) * (double) height / size);

    uint8_t** new_matrix = calloc(height, sizeof(uint8_t*));

    #pragma omp parallel for shared(new_matrix, matrix) schedule(static)
    for (unsigned int i = 0; i < height; i++) {
        new_matrix[i] = calloc(width, sizeof(uint8_t));
    }

    int start_index = mpi_start;  // Computational start
    int end_index = mpi_end;

    start_index = fmax(0, start_index - 10);
    end_index = fmin(height, end_index + 10);

    #pragma omp parallel for shared(new_matrix, matrix) schedule(static)
    for (int y = start_index; y < end_index; y++) {
        for (int x = 0; x < width; x++) {
            (new_matrix)[y][x] = apply_convolution_at_pos(matrix, width, height, gaussian, k_width, k_height, x, y);
        }
    }

    #pragma omp parallel for shared(new_matrix, matrix) schedule(static)
    for (int y = start_index; y < end_index; y++) {
        for (int x = 0; x < width; x++) {
            (matrix)[y][x] = (new_matrix)[y][x];
        }
    }

    #pragma omp parallel for shared(new_matrix, matrix) schedule(static)
    for (unsigned int i = 0; i < height; i++) {
        free(new_matrix[i]);
    }

    // Gather all MPI data :)
    if (rank == 0) {
        MPI_Status recv_status;

        for (int other_rank = 1; other_rank < size; other_rank++) {
            int other_mpi_start = other_rank * (double) height / size;
            int other_mpi_end = fmin(height, (other_rank + 1) * (double) height / size);

            for (int current_pos = other_mpi_start; current_pos < other_mpi_end; current_pos++) {
                MPI_Recv(matrix[current_pos], width, MPI_CHAR, other_rank, other_rank, MPI_COMM_WORLD, &recv_status);
            }
        }
    } else {
        for (int i = mpi_start; i < mpi_end; i++) {
            MPI_Send(matrix[i], width, MPI_CHAR, 0, rank, MPI_COMM_WORLD);
        }
    }

    free(new_matrix);
}

int apply_gaussian_blur(IMAGE* image) {
    apply_convolution(image->pixel_grid, image->width, image->height, gaussian, GAUSSIAN_K_WIDTH, GAUSSIAN_K_HEIGHT);
    return 0;
}

int free_image(IMAGE* image) {

    for (unsigned int i = 0; i < image->height; i++) {
        free(image->pixel_grid[i]);
    }
    free(image->pixel_grid);

    return 0;
}
