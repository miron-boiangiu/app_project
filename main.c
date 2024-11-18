#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <mpi.h>

#include "image_editor_MPI.h"

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int blur_count = 1; 

    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input file> <output file>\n", argv[0]);
        return 1;
    }

    if (argc >= 4) {
        blur_count = atoi(argv[3]);
    }

    IMAGE img;
    open_bmp_image(argv[1], &img);

    for (int i = 0; i < blur_count; i++) {
        apply_gaussian_blur(&img);
    }
    
    save_bmp_image(argv[1], argv[2], &img);
    free_image(&img);
    MPI_Finalize();
    return 0;
}
