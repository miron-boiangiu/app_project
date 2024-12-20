#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "image_editor.h"

int main(int argc, char *argv[]) {

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
    
    save_bmp_image(argv[1], argv[2], &img);
    free_image(&img);

    return 0;
}
