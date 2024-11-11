# Convolution-based image filters

Boiangiu Victor-Miron

Irimia Marian

# Notes

- Link to BMP library used in this project: [Github.com](https://github.com/mattflow/cbmp)

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


![image](https://github.com/user-attachments/assets/23c49b1c-f6a1-43af-8063-8638cb5d1f3f)

# Profiling:

- Small image, 1 layer of blur :

| % Time | Cumulative Seconds | Self Seconds | Calls   | Self ms/call | Total ms/call | Name                    |
|--------|--------------------|--------------|---------|--------------|---------------|-------------------------|
| 40.00  | 0.04               | 0.04         | 512640  | 0.00         | 0.00          | apply_convolution_at_pos |
| 15.00  | 0.06               | 0.01         | 3075840 | 0.00         | 0.00          | _get_pixel               |
| 15.00  | 0.07               | 0.01         | 3       | 5.00         | 10.00         | _map                    |
| 10.00  | 0.08               | 0.01         | 512640  | 0.00         | 0.00          | get_pixel_rgb            |
| 10.00  | 0.09               | 0.01         | 1       | 10.00        | 50.00         | apply_convolution        |
| 10.00  | 0.10               | 0.01         | 1       | 10.00        | 30.00         | save_bmp_image           |


- Small image, 20 layers of blur :
  
| % Time | Cumulative Seconds | Self Seconds | Calls   | Self ms/call | Total ms/call | Name                        |
|--------|--------------------|--------------|---------|--------------|---------------|-----------------------------|
| 80.21  | 0.77               | 0.77         | 10252800| 0.00         | 0.00          | apply_convolution_at_pos    |
| 8.33   | 0.85               | 0.08         | 20      | 4.00         | 42.50         | apply_convolution           |
| 3.12   | 0.88               | 0.03         | 1537920 | 0.00         | 0.00          | _update_file_byte_contents  |
| 2.08   | 0.90               | 0.02         | 3075848 | 0.00         | 0.00          | _get_int_from_buffer        |
| 2.08   | 0.92               | 0.02         | 3075840 | 0.00         | 0.00          | _get_pixel                  |
| 1.04   | 0.93               | 0.01         | 512640  | 0.00         | 0.00          | set_pixel_rgb               |
| 1.04   | 0.94               | 0.01         | 3       | 3.33         | 26.67         | _map                        |

- Medium image, 1 layer of blur :

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

- Medium image, 20 layers of blur :

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

