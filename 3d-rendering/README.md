# 3D Rendering

## Description
This design renders 2D images from 3D models (3D triangle mesh). It is composed of the following stages.
1. Projection: 3D triangle -> 2D triangle
2. Rasterization (1, 2): search pixels in 2D triangle within the bounding box
3. Z-culling: hide or display pixels according to each pixel's "z" value (depth)
4. ColoringFB: coloring framebuffer according to the zbuffer

## Major Optimizations
This application has two major optimizations:
1. Data packing optimization for inputs. Each 3D triangle is packed into a 32-bit integer for faster off-chip data transfer. 
2. Coarse-grained pipelining (dataflow). The stages introduced above are arranged in a coarse-grained pipeline so that their execution time can be overlapped. 

## Usage
### SDAccel
To execute or simulate the design, do

`./rendering_host.exe -f <path_to_xclbin_file>`

### SDSoC
To run design on the board, do

`./rendering.elf`

### SW
To run software version, do

`./rendering_sw.exe`


