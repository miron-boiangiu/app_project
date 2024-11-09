#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "image_editor.h"

int main(int argc, char *argv[]) {

    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <input file> <output file>\n", argv[0]);
        return 1;
    }

    IMAGE img;
    open_bmp_image(argv[1], &img);

    apply_gaussian_blur(&img);
    
    save_bmp_image(argv[1], argv[2], &img);
    free_image(&img);

    return 0;
}
