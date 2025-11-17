#include <cassert>
#include "Matrix.hpp"
#include <algorithm> // sort

using namespace std;

// REQUIRES: mat points to a Matrix
//           0 < width && 0 < height
// MODIFIES: *mat
// EFFECTS:  Initializes *mat as a Matrix with the given width and height,
//           with all elements initialized to 0.
void Matrix_init(Matrix* mat, int width, int height) {
  assert(0 < width && 0 < height);

  mat->width=width;
  mat->height=height;
  int capacity=width*height;
  mat->data.assign (capacity,0);
}

// REQUIRES: mat points to a valid Matrix
// MODIFIES: os
// EFFECTS:  First, prints the width and height for the Matrix to os:
//             WIDTH [space] HEIGHT [newline]
//           Then prints the rows of the Matrix to os with one row per line.
//           Each element is followed by a space and each row is followed
//           by a newline. This means there will be an "extra" space at
//           the end of each line.
void Matrix_print(const Matrix* mat, std::ostream& os) {
  int element = 0;

  os << mat->width << " " << mat->height << "\n";
  for(int r = 0; r < mat->height; r++){
    for(int c = 0; c < mat->width; c++){
      element = mat->data[r*mat->width+c];
      os << element << " ";
    os << "\n";
  }
  }
}

// REQUIRES: mat points to a valid Matrix
// EFFECTS:  Returns the width of the Matrix.
int Matrix_width(const Matrix* mat) {
  return mat->width;
}

// REQUIRES: mat points to a valid Matrix
// EFFECTS:  Returns the height of the Matrix.
int Matrix_height(const Matrix* mat) {
  return mat->height;
}

// REQUIRES: mat points to a valid Matrix
//           0 <= row && row < Matrix_height(mat)
//           0 <= column && column < Matrix_width(mat)
//
// MODIFIES: (The returned pointer may be used to modify an
//            element in the Matrix.)
// EFFECTS:  Returns a pointer to the element in the Matrix
//           at the given row and column.
int* Matrix_at(Matrix* mat, int row, int column) {
  assert(0 <= row && row < Matrix_height(mat));
  assert(0 <= column && column < Matrix_width(mat));
  return &mat->data[row*mat->width+column];
}

// REQUIRES: mat points to a valid Matrix
//           0 <= row && row < Matrix_height(mat)
//           0 <= column && column < Matrix_width(mat)
//
// EFFECTS:  Returns a pointer-to-const to the element in
//           the Matrix at the given row and column.
const int* Matrix_at(const Matrix* mat, int row, int column) {
  assert(0 <= row && row < Matrix_height(mat));
  assert(0 <= column && column < Matrix_width(mat));
  return &mat->data[row*mat->width+column];
}

// REQUIRES: mat points to a valid Matrix
// MODIFIES: *mat
// EFFECTS:  Sets each element of the Matrix to the given value.
void Matrix_fill(Matrix* mat, int value) {
  int capacity=mat->width*mat->height;
  mat->data.assign (capacity,value);
}

// REQUIRES: mat points to a valid Matrix
// MODIFIES: *mat
// EFFECTS:  Sets each element on the border of the Matrix to
//           the given value. These are all elements in the first/last
//           row or the first/last column.
void Matrix_fill_border(Matrix* mat, int value) {
  for(int r = 0; r < Matrix_height(mat) ; r++){
    for(int c = 0; c < Matrix_width(mat); c++){
      if(r==0 || c==0 || r == Matrix_height(mat)-1 || c == Matrix_width(mat)-1){
        mat->data[r*Matrix_width(mat)+c]=value;
      }
  }
  }
}

// REQUIRES: mat points to a valid Matrix
// EFFECTS:  Returns the value of the maximum element in the Matrix
int Matrix_max(const Matrix* mat) {
  vector<int> v=mat->data;
  sort(v.begin(), v.end());
  return v.back() ;
}

// REQUIRES: mat points to a valid Matrix
//           0 <= row && row < Matrix_height(mat)
//           0 <= column_start && column_end <= Matrix_width(mat)
//           column_start < column_end
// EFFECTS:  Returns the column of the element with the minimal value
//           in a particular region. The region is defined as elements
//           in the given row and between column_start (inclusive) and
//           column_end (exclusive).
//           If multiple elements are minimal, returns the column of
//           the leftmost one.
int Matrix_column_of_min_value_in_row(const Matrix* mat, int row,
                                      int column_start, int column_end) {
  assert(0 <= row && row < Matrix_height(mat));
  assert(0 <= column_start && column_end <= Matrix_width(mat));
  assert(column_start < column_end);
  vector<int> v=mat->data;
  vector<int> region(v.begin()+row*Matrix_width(mat),v.begin()+(1+row)*Matrix_width(mat));
  int min=region[column_start];
  int min_col=column_start;
  for (int i = column_start; i<column_end+1; i++){
    if(region[i]<min){
      min_col=i;
      min=region[i];}
  }

  return min_col;
}

// REQUIRES: mat points to a valid Matrix
//           0 <= row && row < Matrix_height(mat)
//           0 <= column_start && column_end <= Matrix_width(mat)
//           column_start < column_end
// EFFECTS:  Returns the minimal value in a particular region. The region
//           is defined as elements in the given row and between
//           column_start (inclusive) and column_end (exclusive).
int Matrix_min_value_in_row(const Matrix* mat, int row,
                            int column_start, int column_end) {
  assert(0 <= row && row < Matrix_height(mat));
  assert(0 <= column_start && column_end <= Matrix_width(mat));
  assert(column_start < column_end);
  vector<int> v=mat->data;
  vector<int> region(v.begin()+row*Matrix_width(mat),v.begin()+(1+row)*Matrix_width(mat));
  int min=region[column_start];
  int min_col=column_start;
  for (int i = column_start; i<column_end+1; i++){
    if(region[i]<min){
      min_col=i;
      min=region[i];}
  }

  return region[min_col];
}
