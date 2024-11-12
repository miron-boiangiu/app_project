# Convolution-based image filters

- Boiangiu Victor-Miron

- Irimia Marian

# Notes

- Link to BMP library used in this project: [Github.com](https://github.com/mattflow/cbmp)


# Description

In this project, we are going to apply convolution-based filters on images, more specifically gaussian blur, and then check how different parallelization techniques affect the performance.

# Temporal Complexity

### Sequential

The complexity of the sequential algorithm is O(N*M), where N is the width of the image and M is the height of the image. This results from calling apply_convolution_as_pos (which runs in constant time, as its for-loops go to 9 total loops at most) on each pixel of the image.

# Spatial complexity

### Sequential

The spatial complexity is, just as the temporal one, O(N*M), as we cannot apply the convolution in-place, we have to create another array of
pixels the size of the original image and put the resulting values from the convolution there.

# Run tests:

``` make <test_small/test_medium/test_big> ```

# Code analysis:
The serial algorithm was tested based on image sizes and blur intensity, multiple times.

***WSL Version*** : Wsman Shell commandLine, version 0.2.1.

***Architecture***:            x86_64 

***CPU op-mode(s)***:        32-bit, 64-bit 

***Address sizes***:         39 bits physical, 48 bits virtual

***Byte Order***:            Little Endian 

***CPU(s)***:                  8

***On-line CPU(s) list***:   0-7

***Vendor ID***:               GenuineIntel

***Model name***:            Intel(R) Core(TM) i5-10300H CPU @ 2.50GHz

***CPU family***:          6    

***Model***:               165   

***Thread(s) per core***:  2  

***Core(s) per socket***:  4 

***Socket(s)***:           1 

***Stepping***:            2 


![image](https://github.com/user-attachments/assets/5f7dc54b-80e6-4260-8b35-009b1c3b966e)


# Profiling:

- Small image, 1 layer of blur :

| % Time | Cumulative Seconds | Self Seconds | Calls     | Self ms/call | Total ms/call | Name                      |
|--------|--------------------|--------------|-----------|--------------|---------------|---------------------------|
| 48.39  | 0.15               | 0.15         | 2178576   | 0.00         | 0.00          | apply_convolution_at_pos   |
| 24.19  | 0.23               | 0.07         | 13071456  | 0.00         | 0.00          | _get_pixel                 |
| 9.68   | 0.26               | 0.03         | 2178576   | 0.00         | 0.00          | get_pixel_rgb              |
| 6.45   | 0.28               | 0.02         | 2178576   | 0.00         | 0.00          | set_pixel_rgb              |
| 3.23   | 0.28               | 0.01         | 6535728   | 0.00         | 0.00          | _update_file_byte_contents |
| 3.23   | 0.29               | 0.01         | 1         | 10.00        | 70.00         | open_bmp_image             |
| 3.23   | 0.30               | 0.01         | 1         | 10.00        | 90.00         | save_bmp_image             |
| 1.61   | 0.31               | 0.01         | 3         | 1.67         | 30.00         | _map                       |

- Small image, 20 layers of blur :

| % Time | Cumulative Seconds | Self Seconds | Calls     | Self ms/call | Total ms/call | Name                      |
|--------|--------------------|--------------|-----------|--------------|---------------|---------------------------|
| 90.39  | 2.54               | 2.54         | 43571520  | 0.00         | 0.00          | apply_convolution_at_pos   |
| 4.63   | 2.67               | 0.13         | 20        | 6.50         | 133.50        | apply_convolution          |
| 1.42   | 2.71               | 0.04         | 1         | 40.00        | 70.00         | open_bmp_image             |
| 1.07   | 2.74               | 0.03         | 13071464  | 0.00         | 0.00          | _get_int_from_buffer       |
| 1.07   | 2.77               | 0.03         | 13071456  | 0.00         | 0.00          | _get_pixel                 |
| 0.71   | 2.79               | 0.02         | 3         | 6.67         | 30.00         | _map                       |
| 0.36   | 2.80               | 0.01         | 6535728   | 0.00         | 0.00          | _update_file_byte_contents |
| 0.36   | 2.81               | 0.01         | 2178576   | 0.00         | 0.00          | set_pixel_rgb              |

- Medium image, 1 layer of blur:

| % Time | Cumulative Seconds | Self Seconds | Calls     | Self ms/Call | Total ms/Call | Function Name                     |
|--------|--------------------|--------------|-----------|--------------|---------------|-----------------------------------|
| 34.48  | 0.50               | 0.50         | 7,500,000 | 0.00         | 0.00          | apply_convolution_at_pos          |
| 12.41  | 0.68               | 0.18         | 45,000,008| 0.00         | 0.00          | _get_int_from_buffer              |
| 11.03  | 0.84               | 0.16         | 3         | 53.33        | 183.33        | _map                              |
| 8.97   | 0.97               | 0.13         | 1         | 130.00       | 630.00        | apply_convolution                 |
| 7.59   | 1.08               | 0.11         | 45,000,000| 0.00         | 0.00          | _get_pixel                        |
| 7.59   | 1.19               | 0.11         | 1         | 110.00       | 516.67        | save_bmp_image                    |
| 6.90   | 1.29               | 0.10         | 22,500,000| 0.00         | 0.00          | _update_file_byte_contents        |
| 4.83   | 1.36               | 0.07         | 1         | 70.00        | 303.33        | open_bmp_image                    |
| 2.76   | 1.40               | 0.04         | 7,500,000 | 0.00         | 0.00          | get_pixel_rgb                     |
| 2.07   | 1.43               | 0.03         | 7,500,000 | 0.00         | 0.00          | set_pixel_rgb                     |
| 0.69   | 1.44               | 0.01         | 2         | 5.00         | 5.00          | _get_file_byte_number             |
| 0.69   | 1.45               | 0.01         | 2         | 5.00         | 188.33        | _populate_pixel_array             |

- Medium image, 20 layers of blur:

| % Time | Cumulative Seconds | Self Seconds | Calls     | Self ms/Call | Total ms/Call | Function Name                     |
|--------|--------------------|--------------|-----------|--------------|---------------|-----------------------------------|
| 76.61  | 11.27              | 11.27        | 150,000,000| 0.00         | 0.00          | apply_convolution_at_pos         |
| 18.97  | 14.06              | 2.79         | 20        | 139.50       | 703.00        | apply_convolution                 |
| 0.99   | 14.21              | 0.14         | 45,000,000| 0.00         | 0.00          | _get_pixel                        |
| 0.92   | 14.34              | 0.14         | 45,000,008| 0.00         | 0.00          | _get_int_from_buffer              |
| 0.65   | 14.44              | 0.10         | 3         | 31.67        | 135.00        | _map                              |
| 0.54   | 14.52              | 0.08         | 7,500,000 | 0.00         | 0.00          | get_pixel_rgb                     |
| 0.54   | 14.60              | 0.08         | 1         | 80.00        | 377.50        | save_bmp_image                    |
| 0.34   | 14.64              | 0.05         | 1         | 50.00        | 272.50        | open_bmp_image                    |
| 0.20   | 14.68              | 0.03         | 22,500,000| 0.00         | 0.00          | _update_file_byte_contents        |
| 0.14   | 14.70              | 0.02         | 7,500,000 | 0.00         | 0.00          | set_pixel_rgb                     |
| 0.07   | 14.71              | 0.01         | 2         | 5.00         | 140.00        | _populate_pixel_array             |
| 0.03   | 14.71              | 0.01         | 2         | 2.50         | 2.50          | _get_file_byte_number             |


- Big image, 1 layer of blur :

| % Time | Cumulative Seconds | Self Seconds | Calls     | Self ms/call | Total ms/call | Name                      |
|--------|--------------------|--------------|-----------|--------------|---------------|---------------------------|
| 33.99  | 0.69               | 0.69         | 10000000  | 0.00         | 0.00          | apply_convolution_at_pos   |
| 13.79  | 0.97               | 0.28         | 1         | 280.00       | 541.67        | open_bmp_image             |
| 9.36   | 1.16               | 0.19         | 80000008  | 0.00         | 0.00          | _get_int_from_buffer       |
| 9.36   | 1.35               | 0.19         | 1         | 190.00       | 880.00        | apply_convolution          |
| 8.37   | 1.52               | 0.17         | 80000000  | 0.00         | 0.00          | _get_pixel                 |
| 7.88   | 1.68               | 0.16         | 3         | 53.33        | 206.67        | _map                       |
| 4.93   | 1.78               | 0.10         | 40000000  | 0.00         | 0.00          | _update_file_byte_contents |
| 4.93   | 1.88               | 0.10         | 1         | 100.00       | 608.33        | save_bmp_image             |
| 3.94   | 1.96               | 0.08         | 10000000  | 0.00         | 0.00          | set_pixel_rgb              |
| 1.97   | 2.00               | 0.04         | 10000000  | 0.00         | 0.00          | get_pixel_rgb              |
| 1.48   | 2.03               | 0.03         | 2         | 15.00        | 221.67        | _populate_pixel_array      |

- Big image, 20 layers of blur :

| % Time | Cumulative Seconds | Self Seconds | Calls     | Self s/call | Total s/call | Name                      |
|--------|--------------------|--------------|-----------|-------------|---------------|---------------------------|
| 75.80  | 16.35              | 16.35        | 200000000 | 0.00        | 0.00          | apply_convolution_at_pos   |
| 18.59  | 20.36              | 4.01         | 20        | 0.20        | 1.02          | apply_convolution          |
| 1.27   | 20.64              | 0.28         | 3         | 0.09        | 0.29          | _map                       |
| 1.07   | 20.86              | 0.23         | 80000008  | 0.00        | 0.00          | _get_int_from_buffer       |
| 1.04   | 21.09              | 0.23         | 80000000  | 0.00        | 0.00          | _get_pixel                 |
| 0.70   | 21.24              | 0.15         | 1         | 0.15        | 0.76          | save_bmp_image             |
| 0.65   | 21.38              | 0.14         | 40000000  | 0.00        | 0.00          | _update_file_byte_contents |
| 0.37   | 21.46              | 0.08         | 1         | 0.08        | 0.45          | open_bmp_image             |
| 0.32   | 21.53              | 0.07         | 10000000  | 0.00        | 0.00          | get_pixel_rgb              |
| 0.14   | 21.56              | 0.03         | 10000000  | 0.00        | 0.00          | set_pixel_rgb              |
| 0.05   | 21.57              | 0.01         | 2         | 0.01        | 0.29          | _populate_pixel_array      |

