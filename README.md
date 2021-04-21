# Pixeler

Pixelate an image with any block size using either average pixel value or by the brightest pixel for each block

---
## Insallation
1. Install [openCV](https://docs.opencv.org/master/d7/d9f/tutorial_linux_install.html)
2. Run `cmake ./` to generate Makefile
3. Run `make` to compile

---
## Usage
Currently `pixeler` uses extremely rudimentary command line parsing to take arguments

`pixeler block_heigh block_width brightest file_path_src (optional) output_path`

- block_height: height of the blocks in pixels [integer]
- block_width: width of the blocks in pixels [integer]
- brightest: whether to use average or brightest [1: brightest, any value for average]
- file_path_src: path to input image [string]
- (optional) output_path: path to save pixelated image to, if not passed image is displayed [string]

#### Example
`lspixeler 10 5 1 example/input.png example/out.png`
