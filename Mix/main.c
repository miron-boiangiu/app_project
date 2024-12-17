#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <mpi.h>

#include "image_editor.h"

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int blur_count = 1;

    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input file> <output file> [blur_count] [thread_count]\n", argv[0]);
        return 1;
    }

    if (argc >= 4) {
        blur_count = atoi(argv[3]);
    }

    int thread_count = 1;

    if (argc >= 5) {
        thread_count = atoi(argv[4]);
    }

    IMAGE img;
    open_bmp_image(argv[1], &img);

    for (int i = 0; i < blur_count; i++) {
        apply_gaussian_blur(&img, thread_count);
    }
    
    if (rank == 0)
        save_bmp_image(argv[1], argv[2], &img);
    free_image(&img);

    MPI_Finalize();
    return 0;
}
