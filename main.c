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

    return 0;
}
