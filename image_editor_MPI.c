#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <mpi.h>

#include "cbmp.h"

#include "image_editor_MPI.h"

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
void apply_convolution(
    uint8_t** matrix,
    int width,
    int height,
    float kernel[GAUSSIAN_K_HEIGHT][GAUSSIAN_K_WIDTH],
    int k_width,
    int k_height
) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Determine the portion of the image each process will handle
    int rows_per_proc = height / size;
    int remainder = height % size;

    int start_row = rank * rows_per_proc + (rank < remainder ? rank : remainder);
    int end_row = start_row + rows_per_proc + (rank < remainder ? 1 : 0);

    // Allocate memory for the local portion of the new matrix
    int local_height = end_row - start_row;
    uint8_t** local_new_matrix = calloc(local_height, sizeof(uint8_t*));
    for (int i = 0; i < local_height; i++) {
        local_new_matrix[i] = calloc(width, sizeof(uint8_t));
    }

    // Each process computes its portion of the convolution
    for (int y = start_row; y < end_row; y++) {
        for (int x = 0; x < width; x++) {
            local_new_matrix[y - start_row][x] = apply_convolution_at_pos(
                matrix,
                width,
                height,
                kernel,
                k_width,
                k_height,
                x,
                y
            );
        }
    }

    // Gather the computed portions back to the root process
    int* recvcounts = NULL;
    int* displs = NULL;
    if (rank == 0) {
        recvcounts = malloc(size * sizeof(int));
        displs = malloc(size * sizeof(int));
        int offset = 0;
        for (int i = 0; i < size; i++) {
            int proc_rows = height / size + (i < remainder ? 1 : 0);
            recvcounts[i] = proc_rows * width;
            displs[i] = offset;
            offset += recvcounts[i];
        }
    }

    // Flatten the local new matrix
    uint8_t* local_flat_new_matrix = malloc(local_height * width * sizeof(uint8_t));
    for (int i = 0; i < local_height; i++) {
        memcpy(&local_flat_new_matrix[i * width], local_new_matrix[i], width * sizeof(uint8_t));
    }

    // Gather all computed data at the root process
    uint8_t* flat_new_matrix = NULL;
    if (rank == 0) {
        flat_new_matrix = malloc(height * width * sizeof(uint8_t));
    }

    MPI_Gatherv(
        local_flat_new_matrix,
        local_height * width,
        MPI_UNSIGNED_CHAR,
        flat_new_matrix,
        recvcounts,
        displs,
        MPI_UNSIGNED_CHAR,
        0,
        MPI_COMM_WORLD
    );

    // Root reconstructs the new matrix
    if (rank == 0) {
        for (int y = 0; y < height; y++) {
            memcpy(matrix[y], &flat_new_matrix[y * width], width * sizeof(uint8_t));
        }
        free(flat_new_matrix);
        free(recvcounts);
        free(displs);
    }

    // Broadcast the updated matrix to all processes
    for (int y = 0; y < height; y++) {
        MPI_Bcast(matrix[y], width, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);
    }

    // Free local matrices
    for (int i = 0; i < local_height; i++) {
        free(local_new_matrix[i]);
    }
    free(local_new_matrix);
    free(local_flat_new_matrix);
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
