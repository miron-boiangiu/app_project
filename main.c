#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "image_editor.h"

int main(int argc, char *argv[]) {

    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <input file> <output file>\n", argv[0]);
        exit(1);
    }

    IMAGE img;
    open_bmp_image(argv[1], &img);
    save_bmp_image(argv[1], argv[2], &img);
    free_image(&img);

    return 0;
}
