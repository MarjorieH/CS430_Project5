# CS430_Project4
### CS430: Computer Graphics
### Project 5: Image Viewer
### Marjorie Hahn
### 9 November 2016

This application takes a PPM image file and renders it using OpenGL ES2. This
image can then have one of the affine transformations applied to it. The supported
transformations are: translation, rotation, scale, and shear.

Usage: ezview input.ppm

Where input.ppm is the image file to be displayed.

One the program is running, the following keys can be pressed to perform affine
transformations on the image:

* 1: Translation
* 2: Rotation
* 3: Scale
* 4: Shear
* Any other key: Resets image

In order to run the program, download the files off of Github into a compiled
copy of GLWS so that the program files are all sitting in the root directory for
GLWS. Then, make sure that you are sitting in the directory that holds all of
the files for this application and run the command "make all". Then you will be
able to run the program using the usage command mentioned above.

There is one test PPM file included that you can use to test the functionality
of the program: testimage.ppm. The expected output image for this JSON file is
output.ppm. You can create the corresponding PPM image for this JSON file using
the command "make test".

Note that this application relies on the linmath library included for GLFW. The
hard-coded path for which is "../deps/linmath".
