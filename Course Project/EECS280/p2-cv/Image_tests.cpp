#include "Matrix.hpp"
#include "Image_test_helpers.hpp"
#include "unit_test_framework.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <cassert>

using namespace std;

// Here's a free test for you! Model yours after this one.
// Test functions have no interface and thus no RMEs, but
// add a comment like the one here to say what it is testing.
// -----
// Sets various pixels in a 2x2 Image and checks
// that Image_print produces the correct output.
TEST(test_print_basic) {
  Image img;
  const Pixel red = {255, 0, 0};
  const Pixel green = {0, 255, 0};
  const Pixel blue = {0, 0, 255};
  const Pixel white = {255, 255, 255};

  Image_init(&img, 2, 2);
  Image_set_pixel(&img, 0, 0, red);
  Image_set_pixel(&img, 0, 1, green);
  Image_set_pixel(&img, 1, 0, blue);
  Image_set_pixel(&img, 1, 1, white);

  // Capture our output
  ostringstream s;
  Image_print(&img, s);

  // Correct output
  ostringstream correct;
  correct << "P3\n2 2\n255\n";
  correct << "255 0 0 0 255 0 \n";
  correct << "0 0 255 255 255 255 \n";
  ASSERT_EQUAL(s.str(), correct.str());
}

// IMPLEMENT YOUR TEST FUNCTIONS HERE
// You are encouraged to use any functions from Image_test_helpers.hpp as needed.
TEST(Image_init_width_height_get){
  Image img;
  Image_init(&img,2,3);
  ASSERT_EQUAL(img.width,2);
  ASSERT_EQUAL(img.height,3);  
  ASSERT_EQUAL(Image_width(&img),2);
  ASSERT_EQUAL(Image_height(&img),3);
  Pixel Zero={0,0,0};
  ASSERT_EQUAL(Pixel_equal(Image_get_pixel(&img,0,0),Zero),true);
  ASSERT_EQUAL(Pixel_equal(Image_get_pixel(&img,1,0),Zero),true);
  ASSERT_EQUAL(Pixel_equal(Image_get_pixel(&img,2,1),Zero),true);
}

TEST(width_height){
  Image img;
  ASSERT_EQUAL(img.width, Image_width(&img));
  ASSERT_EQUAL(img.height,  Image_height(&img));  
}

TEST(get_set_pixel){
  Image img;
  Image_init(&img,2,3);  
  Pixel White ={0,0,0};
  Image_set_pixel(&img,2,0,White);
  ASSERT_EQUAL(Pixel_equal(Image_get_pixel(&img,2,0),White),true);
}

TEST(fill){
  Pixel Color ={1,250,255};
  Image img1;
  Image_init(&img1,3,2);
  Image_fill(&img1,Color);
  Image img2;
  Image_init(&img2,3,2);
  Matrix mat;
  Matrix_init(&mat,3,2);
  Matrix_fill(&mat,1);
  img2.red_channel = mat;
  Matrix_fill(&mat,250);
  img2.green_channel=mat;
  Matrix_fill(&mat,255);
  img2.blue_channel =mat;
   ASSERT_EQUAL(Image_equal(&img1,&img2),true);
}

TEST_MAIN() // Do NOT put a semicolon here
