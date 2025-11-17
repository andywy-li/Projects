#include "Matrix.hpp"
#include "Matrix_test_helpers.hpp"
#include "unit_test_framework.hpp"

using namespace std;

// Here's a free test for you! Model yours after this one.
// Test functions have no interface and thus no RMEs, but
// add a comment like the one here to say what it is testing.
// -----
// Fills a 3x5 Matrix with a value and checks
// that Matrix_at returns that value for each element.



// ADD YOUR TESTS HERE
// You are encouraged to use any functions from Matrix_test_helpers.hpp as needed.


TEST(initial){
  Matrix mat;
  Matrix_init(&mat, 3, 5);
  ASSERT_EQUAL(Matrix_width(&mat),3);
  ASSERT_EQUAL(Matrix_height(&mat),5);
}

// TEST(negative_initial){
  // Matrix mat;
  // Matrix_init(&mat, -3, 5);
  // Matrix_init(&mat, 3, -5);
  // Matrix_init(&mat, -3, -5);
  // Matrix_init(&mat, 0, 0);
// }

TEST(Print){
  Matrix mat;
  Matrix_init(&mat, 1, 1);

  *Matrix_at(&mat, 0, 0) = 42;
  ostringstream expected;
  expected << "1 1\n"
           << "42 \n";
  ostringstream actual;
  Matrix_print(&mat, actual);
  ASSERT_EQUAL(expected.str(), actual.str());
}

TEST(width_height){
  Matrix mat;
  Matrix_init(&mat, 3, 5); 
  ASSERT_EQUAL(Matrix_width(&mat), 3); //changed from mat.width to function
  ASSERT_EQUAL(Matrix_height(&mat), 5);
}

TEST(Matrix_at){
  Matrix mat;
  Matrix_init(&mat, 3, 2);
  mat.data = {0,2,4,6,8,10};
  ASSERT_EQUAL(*Matrix_at(&mat,0,0),0);
  ASSERT_EQUAL(*Matrix_at(&mat,0,1),2);
  ASSERT_EQUAL(*Matrix_at(&mat,0,2),4);
  ASSERT_EQUAL(*Matrix_at(&mat,1,0),6);
  ASSERT_EQUAL(*Matrix_at(&mat,1,1),8);
  ASSERT_EQUAL(*Matrix_at(&mat,1,2),10);
}

// TEST(Negative_Matrix_at){
//   Matrix mat;
//   Matrix_init(&mat, 3, 5);
//   const int width = 3;
//   const int height = 5;
//   const int value = 42;
//   Matrix_fill(&mat, value);
//   ASSERT_EQUAL(*Matrix_at(&mat, -1, 2), value);
//   ASSERT_EQUAL(*Matrix_at(&mat, 1, -2), value);
//   ASSERT_EQUAL(*Matrix_at(&mat, -1, -2), value);
// }

TEST(constMatrix_at){
  Matrix mat;
  Matrix_init(&mat, 3, 5);
  const int value = 42;
  Matrix_fill(&mat, value);
  const int* a = Matrix_at(&mat, 1, 2);
  ASSERT_EQUAL(*a, value);
}

// TEST(Negative_constMatrix_at){
//   Matrix mat;
//   Matrix_init(&mat, 3, 5);
//   const int width = 3;
//   const int height = 5;
//   const int value = 42;
//   Matrix_fill(&mat, value);
//   const int* a = Matrix_at(&mat, -1, 2);
//   ASSERT_EQUAL(*a, value);
//   const int* b = Matrix_at(&mat, 1, -2);
//   ASSERT_EQUAL(*b, value);
//   const int* c = Matrix_at(&mat, -1, -2);
//   ASSERT_EQUAL(*c, value);
//   const int* d = Matrix_at(&mat, 0, 0);
//   ASSERT_EQUAL(*b, value);
// }

TEST(fill_border){
  Matrix mat;
  Matrix_init(&mat, 3, 5);
  const int value = 42;
  Matrix_fill(&mat, 40);
  Matrix_fill_border(&mat, value);  
  ASSERT_EQUAL(*Matrix_at(&mat,0,2),value);
  ASSERT_EQUAL(*Matrix_at(&mat,0,0),value);
  ASSERT_EQUAL(*Matrix_at(&mat,1,0),value);
  ASSERT_EQUAL(*Matrix_at(&mat,1,2),value);
  ASSERT_EQUAL(*Matrix_at(&mat,4,2),value);
  ASSERT_EQUAL(*Matrix_at(&mat,1,1),40);
  ASSERT_EQUAL(*Matrix_at(&mat,3,1),40);
}

TEST(Matrix_max){
  Matrix mat;
  Matrix_init(&mat, 3, 5);
  mat.data = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
  ASSERT_EQUAL(Matrix_max(&mat),15);
  mat.data = {1,2,3,4,5,6,7,8,9,15,10,11,12,13,14};
  ASSERT_EQUAL(Matrix_max(&mat),15);
  Matrix_fill(&mat, 15);
  ASSERT_EQUAL(Matrix_max(&mat),15);
}

TEST(Matrix_column_of_min_value_in_row){
  Matrix mat;
  Matrix_init(&mat, 5, 3);
  mat.data = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
  ASSERT_EQUAL(Matrix_column_of_min_value_in_row(&mat, 0,0,2),0);
  ASSERT_EQUAL(Matrix_column_of_min_value_in_row(&mat, 0,0,1),0);
  ASSERT_EQUAL(Matrix_column_of_min_value_in_row(&mat, 1,2,4),2);
  ASSERT_EQUAL(Matrix_column_of_min_value_in_row(&mat, 2,1,4),1);
  mat.data = {15,14,13,12,11,10,9,8,7,6,5,4,3,2,1};
  ASSERT_EQUAL(Matrix_column_of_min_value_in_row(&mat, 1,2,3),3);
  ASSERT_EQUAL(Matrix_column_of_min_value_in_row(&mat, 2,2,4),4);
  ASSERT_EQUAL(Matrix_column_of_min_value_in_row(&mat, 0,0,3),3);
    mat.data = {1,1,2,2,2,4,4,3,3,3,5,5,6,6,6};
  ASSERT_EQUAL(Matrix_column_of_min_value_in_row(&mat, 0,1,4),1);
  ASSERT_EQUAL(Matrix_column_of_min_value_in_row(&mat, 1,2,4),2);
  ASSERT_EQUAL(Matrix_column_of_min_value_in_row(&mat, 2,0,3),0);
}

// TEST(Negative_Matrix_column_of_min_value_in_row){
//   Matrix mat;
//   mat.data = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
//   Matrix_column_of_min_value_in_row(&mat, -1,1,4);
//   Matrix_column_of_min_value_in_row(&mat, 0,-1,4);
//   Matrix_column_of_min_value_in_row(&mat, 0,1,-4);
//   Matrix_column_of_min_value_in_row(&mat, 0,3,2);
// }


TEST(Matrix_min_value_in_row){
  Matrix mat;
  Matrix_init(&mat, 5, 3);
  mat.data = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
  ASSERT_EQUAL(Matrix_min_value_in_row(&mat, 0,0,2),1);
  ASSERT_EQUAL(Matrix_min_value_in_row(&mat, 0,3,4),4);
  ASSERT_EQUAL(Matrix_min_value_in_row(&mat, 1,2,4),8);
  ASSERT_EQUAL(Matrix_min_value_in_row(&mat, 2,1,4),12);
  mat.data = {15,14,13,12,11,10,9,8,7,6,5,4,3,2,1};
  ASSERT_EQUAL(Matrix_min_value_in_row(&mat, 1,2,3),7);
  ASSERT_EQUAL(Matrix_min_value_in_row(&mat, 2,2,4),1);
  ASSERT_EQUAL(Matrix_min_value_in_row(&mat, 0,0,3),12);
    mat.data = {1,1,2,2,2,4,4,3,3,3,5,5,6,6,6};
  ASSERT_EQUAL(Matrix_min_value_in_row(&mat, 0,1,4),1);
  ASSERT_EQUAL(Matrix_min_value_in_row(&mat, 1,2,4),3);
  ASSERT_EQUAL(Matrix_min_value_in_row(&mat, 2,0,3),5);
}
// TEST(Negative_Matrix_column_of_min_value_in_row){
//   Matrix mat;
//   mat.data = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
//   Matrix_min_value_in_row(&mat, -1,1,4);
//   Matrix_min_value_in_row(&mat, 0,-1,4);
//   Matrix_value_in_row(&mat, 0,1,-4);
//   Matrix_min_value_in_row(&mat, 0,3,2);
// }
TEST_MAIN() // Do NOT put a semicolon here