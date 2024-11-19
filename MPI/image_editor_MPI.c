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

// Helper function to calculate start and end rows for each process
int calculate_start_row(int rank, int base_rows, int extra_rows) {
    return rank * base_rows + (rank < extra_rows ? rank : extra_rows);
}

int calculate_rows_to_process(int rank, int base_rows, int extra_rows) {
    return base_rows + (rank < extra_rows ? 1 : 0);
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

    // Calculate base number of rows and extra rows

    int base_rows = height / size;
    int extra_rows = height % size;

    // Determine start row and number of rows to process for this rank
    int start_row = calculate_start_row(rank, base_rows, extra_rows);
    int rows_to_process = calculate_rows_to_process(rank, base_rows, extra_rows);

    // Allocate memory for local results
    uint8_t* local_result = malloc(rows_to_process * width * sizeof(uint8_t));

    // Perform convolution on assigned rows
    for (int i = 0; i < rows_to_process; i++) {
        int y = start_row + i;
        for (int x = 0; x < width; x++) {
            local_result[i * width + x] = apply_convolution_at_pos(
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

    // Prepare to gather results at root process
    uint8_t* recvbuf = NULL;
    int* recvcounts = NULL;
    int* displs = NULL;
    if (rank == 0) {
        recvbuf = malloc(height * width * sizeof(uint8_t));
        recvcounts = malloc(size * sizeof(int));
        displs = malloc(size * sizeof(int));

        int offset = 0;
        for (int i = 0; i < size; i++) {
            int rows = calculate_rows_to_process(i, base_rows, extra_rows);
            recvcounts[i] = rows * width;
            displs[i] = offset;
            offset += recvcounts[i];
        }
    }

    // Gather the local results to the root process
    MPI_Gatherv(
        local_result,
        rows_to_process * width,
        MPI_UNSIGNED_CHAR,
        recvbuf,
        recvcounts,
        displs,
        MPI_UNSIGNED_CHAR,
        0,
        MPI_COMM_WORLD
    );

    // Root process reconstructs the full matrix
    if (rank == 0) {
        for (int i = 0; i < height; i++) {
            memcpy(matrix[i], recvbuf + i * width, width * sizeof(uint8_t));
        }
        // Free root-specific memory
        free(recvbuf);
        free(recvcounts);
        free(displs);
    }

    // Free local memory
    free(local_result);
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
