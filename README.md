# Convolution-based image filters

### Students:

- Boiangiu Victor-Miron

- Irimia Marian

# Notes

- Link for tests: [Google Drive](https://drive.google.com/file/d/1NNv27hdThwMfk2xqycP2imAbKbM2QiVU/view?usp=sharing)
- Link to BMP library used in this project: [Github](https://github.com/mattflow/cbmp)

# Description

In this project, we are going to apply convolution-based filters on images, more specifically gaussian blur, and then check how different parallelization techniques affect the performance.

# Temporal Complexity

### Pseudocode

```
Function apply_convolution(matrix, N, M, kernel, k_width, k_height):
    Allocate a new matrix with the same dimensions as the original (M x N)

    For each x from 0 to N - 1:
        For each y from 0 to M - 1:
            Call apply_convolution_at_pos(matrix, N, M, kernel, k_width, k_height, x, y)
            Store the returned value in new_matrix[y][x]

    For each x from 0 to N - 1:
        For each y from 0 to M - 1:
            Copy new_matrix[y][x] to matrix[y][x]

    Free memory allocated for new_matrix


Function apply_convolution_at_pos(matrix, N, M, kernel, k_width, k_height, pos_x, pos_y):
    Initialize result to 0
    Set delta_x to (k_width / 2)
    Set delta_y to (k_height / 2)

    For each kernel_y from 0 to k_height - 1:
        For each kernel_x from 0 to k_width - 1:
            Calculate corresponding_matrix_x = pos_x - delta_x + kernel_x
            Calculate corresponding_matrix_y = pos_y - delta_y + kernel_y

            If corresponding_matrix_x is out of bounds (i.e., < 0 or >= N) or
               corresponding_matrix_y is out of bounds (i.e., < 0 or >= M):
                Return matrix[pos_y][pos_x] // Pixel is out of bounds, return original value

            Multiply kernel[kernel_y][kernel_x] by matrix[corresponding_matrix_y][corresponding_matrix_x]
            Add the result to the total result

    If result > 255:
        Set result to 255 // Clamp the result to the valid pixel range [0, 255]

    Return result
```

The complexity of the sequential algorithm is O(N\*M), where N is the width of the image and M is the height of the image. This results from calling apply_convolution_as_pos (which runs in constant time, as its for-loops go to 9 total loops at most, the convolution matrix being of size 3x3) on each pixel of the image.

# Parallelization Variants

### PThreads

In this variant of parallelization, we simply split the rows of the image evenly across however many threads we were told to create. Each thread processes his 
share of the image, then waits for all the others to finish before moving all the data from the buffer to the image's matrix.

### MPI

This variant works like the PThreads one, with a couple of important mentions: now, each program has to read the file, since sending it would take more time. The problem of having separate memory areas in each program where the image is kept and each program only blurring his share of the image is that artifacts will occur at the edges of the blur zones. In order to fix that, each program actually does a couple pixels more up and down his share of the image.

As expected, this variant is significantly slower due to the overhead of having to transfer the image's data after applying the blur.

### OpenMP

Similar to PThreads, just that this time the code is way easier to follow. We chose static loop scheduling so that we take advantage of caching as much as we can.

### Mix (MPI + PThreads)

The image is split evenly in rows per program, with each program's share of rows being split once again evenly by the number of threads given.

As expected, this variant is slightly faster than the bare MPI one (this may differ based on the number of cores in your CPU though!).

### Mix (MPI + OpenMP)

Exactly like the other mix version, but this one's code is easier to follow.

# Additional Notes

- You may notice that we did not apply parallelization at apply_convolution_at_pos()'s level in any variant. This is because creating threads (or, God forbid, even separate programs) for traversing a 3x3 matrix does not save any time. It is more efficient to parallelize at the image's traversal level.