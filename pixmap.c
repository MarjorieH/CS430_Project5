#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Global variables to hold image data
int imageWidth; // image width in pixels
int imageHeight; // image height in pixels
int maxColor; // maximum numeric color value for the image
int numChars; // total number of color channels in image (w * h * 3)
char format[64]; // format of the image, supports P3 or P6
unsigned char* pixmap; // array of pixels to hold the image data

/* Reads in P3 formatted data into the pixmap
 * Takes in the file handler of the file to be read in, beginning at the image data */
void readP3 (FILE* fh) {
  // Fill the pixel map up with the image data
  int r, g, b;
  for (int i = 0; i < numChars; i++) {
    if ((fscanf(fh, "%i", &r) != EOF) &&
        (fscanf(fh, "%i", &g) != EOF) &&
        (fscanf(fh, "%i", &b) != EOF)) {
      if (r > maxColor || g > maxColor || b > maxColor) {
        fprintf(stderr, "Error: Image data is not 8 bits per channel.\n");
        exit(1);
      }
      pixmap[i] = r;
      i++;
      pixmap[i] = g;
      i++;
      pixmap[i] = b;
    }
    else {
      // Hit the end of the file before finished reading all of the pixels
      fprintf(stderr, "Error: Image data cannot fill 8 bits per channel.\n");
      exit(1);
    }
  }

  int extra;
  if (fscanf(fh, "%i", &extra) != EOF) {
    // Finished reading the file, but there is still data left over
    fprintf(stderr, "Error: Image data exceeds 8 bits per channel.\n");
    exit(1);
  }
}

/* Reads in P6 formatted data into the pixmap
 * Takes in the file handler of the file to be read in, beginning at the image data */
void readP6 (FILE* fh) {

  unsigned char pixel[3];

  for (int i = 0; i < numChars; i++) {
    if (fread(pixel, sizeof(unsigned char), 3, fh) != 3) {
      fprintf(stderr, "Error: Image data is not 8 bits per channel.\n");
      exit(1);
    }
    else {
      pixmap[i] = pixel[0];
      i++;
      pixmap[i] = pixel[1];
      i++;
      pixmap[i] = pixel[2];
    }
  }
  int extra;
  if (fscanf(fh, "%i", &extra) != EOF) {
    // Finished reading the file, but there is still data left over
    fprintf(stderr, "Error: Image data exceeds 8 bits per channel.\n");
    exit(1);
  }
}

void storeImage(FILE* fh) {

  // Get the image metadata from the header, making sure to skip over comments
  int headerCount = 0;
  char buffer[64];
  while (headerCount < 4) {
    if (!fscanf(fh, "%s", buffer)) { // scan in a value
      fprintf(stderr, "Error: Insufficient image data.\n");
      exit(1);
    }
    if (!strcmp("#", buffer)) { // check if we just scanned in a comment
      int c = fgetc(fh);
      while (c != 10) { // skip the line since it is a comment
        c = fgetc(fh);
      }
    }
    else { // otherwise, save the line based on where we are in the header
      if (headerCount == 0) { // save format
        strcpy(format, buffer);
      }
      else if (headerCount == 1) { // save width
        imageWidth = atoi(buffer);
      }
      else if (headerCount == 2) { // save height
        imageHeight = atoi(buffer);
      }
      else { // save maximum color value
        maxColor = atoi(buffer);
      }
      headerCount++;
    }
  }
  // make sure that the file handler is pointing to the
  // start of the image data before beginning to read it in
  fscanf(fh, "\n");

  if (maxColor > 255) {
    fprintf(stderr, "Error: Image data is not 8 bits per channel.\n");
    exit(1);
  }
  numChars = imageWidth * imageHeight * 3;

  // Allocate memory for the pixel map
  pixmap = malloc(sizeof(unsigned char) * numChars);

  // Read image data into memory based on format
  if (!strcmp(format, "P3")) { // reading P3 image data
    readP3(fh);
  }
  else if (!strcmp(format, "P6")) { // reading P6 image data
    readP6(fh);
  }
  else {
    fprintf(stderr, "Error: Unrecognized image format.\n");
    exit(1);
  }

  // Close the file and return success
  fclose(fh);
}
