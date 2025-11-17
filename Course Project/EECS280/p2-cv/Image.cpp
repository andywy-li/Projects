#include <cassert>
#include <string>
#include "Image.hpp"

using namespace std;

// REQUIRES: img points to an Image
//           0 < width && 0 < height
// MODIFIES: *img
// EFFECTS:  Initializes the Image with the given width and height, with
//           all pixels initialized to RGB values of 0.
void Image_init(Image* img, int width, int height) {
  assert(0 < width && 0 < height);

  img->width = width;
  img->height = height;

  Matrix_init(&img->red_channel, width, height);
  Matrix_init(&img->green_channel, width, height);
  Matrix_init(&img->blue_channel, width, height);
}

// REQUIRES: img points to an Image
//           is contains an image in PPM format without comments
//           (any kind of whitespace is ok)
// MODIFIES: *img, is
// EFFECTS:  Initializes the Image by reading in an image in PPM format
//           from the given input stream.
// NOTE:     See the project spec for a discussion of PPM format.
void Image_init(Image* img, std::istream& is) {
  string whatever;
  int width;
  int height;
  Pixel color;

  is>>whatever;
  is>>width>>height;
  is>>whatever;
  Image_init(img,width,height);
  for(int r=0; r<height; r++){
    for(int c=0; c<width; c++){
      is>>color.r>>color.g>>color.b;
      Image_set_pixel(img,r,c,color);
    }
  }
}

// REQUIRES: img points to a valid Image
// MODIFIES: os
// EFFECTS:  Writes the image to the given output stream in PPM format.
//           You must use the kind of whitespace specified here.
//           First, prints out the header for the image like this:
//             P3 [newline]
//             WIDTH [space] HEIGHT [newline]
//             255 [newline]
//           Next, prints out the rows of the image, each followed by a
//           newline. Each pixel in a row is printed as three ints
//           for its red, green, and blue components, in that order. Each
//           int is followed by a space. This means that there will be an
//           "extra" space at the end of each line. See the project spec
//           for an example.
void Image_print(const Image* img, std::ostream& os) {
  int width=Image_width(img);
  int height=Image_height(img);

  os<<"P3\n"<<width<<" "<<height<<"\n255\n";
  Pixel color;
  for(int r=0; r<height; r++){
    for(int c=0; c<width; c++){
      color=Image_get_pixel(img,r,c);
      os<<color.r<<" "<<color.g<<" "<<color.b<<" ";
    }
    os<<"\n";
  }
}

// REQUIRES: img points to a valid Image
// EFFECTS:  Returns the width of the Image.
int Image_width(const Image* img) {
  return img->width;
}
// REQUIRES: img points to a valid Image
// EFFECTS:  Returns the height of the Image.
int Image_height(const Image* img) {
  return img->height;
}

// REQUIRES: img points to a valid Image
//           0 <= row && row < Image_height(img)
//           0 <= column && column < Image_width(img)
// EFFECTS:  Returns the pixel in the Image at the given row and column.
Pixel Image_get_pixel(const Image* img, int row, int column) {
  assert(0 <= row && row < Image_height(img));
  assert(0 <= column && column < Image_width(img));

  Pixel pixel;

  pixel.r=*Matrix_at(&img->red_channel, row, column);
  pixel.g=*Matrix_at(&img->green_channel, row, column);
  pixel.b=*Matrix_at(&img->blue_channel, row, column);
  return pixel;
}
// REQUIRES: img points to a valid Image
//           0 <= row && row < Image_height(img)
//           0 <= column && column < Image_width(img)
// MODIFIES: *img
// EFFECTS:  Sets the pixel in the Image at the given row and column
//           to the given color.
void Image_set_pixel(Image* img, int row, int column, Pixel color) {
  assert(0 <= row && row < Image_height(img));
  assert(0 <= column && column < Image_width(img));
  
  *Matrix_at(&img->red_channel, row, column)=color.r;
  *Matrix_at(&img->green_channel, row, column)=color.g;
  *Matrix_at(&img->blue_channel, row, column)=color.b;
}

// REQUIRES: img points to a valid Image
// MODIFIES: *img
// EFFECTS:  Sets each pixel in the image to the given color.
void Image_fill(Image* img, Pixel color) {
  Matrix_fill(&img->red_channel,color.r);
  Matrix_fill(&img->green_channel,color.g);
  Matrix_fill(&img->blue_channel,color.b);
}
