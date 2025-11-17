#include "BinarySearchTree.hpp"
#include "unit_test_framework.hpp"
#include <sstream>
#include <vector>


using namespace std;

TEST(bst_public_test) {
  BinarySearchTree<int> tree;

  tree.insert(5);

  ASSERT_TRUE(tree.size() == 1);
  ASSERT_TRUE(tree.height() == 1);

  ASSERT_TRUE(tree.find(5) != tree.end());

  tree.insert(7);
  tree.insert(3);

  ASSERT_TRUE(tree.check_sorting_invariant());
  ASSERT_TRUE(*tree.max_element() == 7);
  ASSERT_TRUE(*tree.min_element() == 3);
  ASSERT_TRUE(*tree.min_greater_than(5) == 7);

  ostringstream oss_preorder;
  ostringstream oss_inorder;
  tree.traverse_preorder(oss_preorder);
  cout << "preorder" << endl;
  cout << oss_preorder.str() << endl << endl;
  ASSERT_TRUE(oss_preorder.str() == "5 3 7 ");
  tree.traverse_inorder(oss_inorder);
  ASSERT_TRUE(oss_inorder.str() == "3 5 7 ");
}

TEST(A3_2) {
  BinarySearchTree<int> tree;
  ASSERT_TRUE(tree.empty());
  tree.insert(3);
  tree.insert(2);
  ASSERT_FALSE(tree.empty());
  BinarySearchTree<int>::Iterator it = tree.begin();
  BinarySearchTree<int>::Iterator itt = tree.find(2);
  ASSERT_TRUE(itt == it);
  ASSERT_TRUE(tree.size() == 2);
  ASSERT_TRUE(tree.height() == 2);

  ASSERT_TRUE(tree.check_sorting_invariant());
  ASSERT_TRUE(*tree.max_element() == 3);
  ASSERT_TRUE(*tree.min_element() == 2);
  ASSERT_TRUE(tree.min_greater_than(3) == tree.end());

  ostringstream expected;
  expected << "[ 2 3 ]";
  ostringstream actual;
  actual <<  tree;
  ASSERT_EQUAL(expected.str(), actual.str());

  ostringstream oss_preorder;
  tree.traverse_preorder(oss_preorder);
  ASSERT_TRUE(oss_preorder.str() == "3 2 ");

  ostringstream oss_inorder;
  tree.traverse_inorder(oss_inorder);
  ASSERT_TRUE(oss_inorder.str() == "2 3 ");
}

TEST(B5_3_2_4_6) {
  BinarySearchTree<int> tree1;
  tree1.insert(5);
  tree1.insert(3);
  tree1.insert(2);
  tree1.insert(4);
  tree1.insert(6);
  BinarySearchTree<int> tree = tree1; 
  ASSERT_TRUE(tree.size() == 5);
  ASSERT_TRUE(tree.height() == 3);
  ASSERT_TRUE(tree.check_sorting_invariant());
  ASSERT_TRUE(*tree.max_element() == 6);
  ASSERT_TRUE(*tree.min_element() == 2);
  ASSERT_TRUE(*tree.min_greater_than(3) == 4);
  ostringstream expected;
  expected << "[ 2 3 4 5 6 ]";
  ostringstream actual;
  actual <<  tree;
  ASSERT_EQUAL(expected.str(), actual.str());

  ostringstream oss_preorder;
  tree.traverse_preorder(oss_preorder);
  ASSERT_TRUE(oss_preorder.str() == "5 3 2 4 6 ");

  ostringstream oss_inorder;
  tree.traverse_inorder(oss_inorder);
  ASSERT_TRUE(oss_inorder.str() == "2 3 4 5 6 ");
}

TEST(C5_3_2_1_6) {
  BinarySearchTree<int> tree1;
  tree1.insert(5);
  tree1.insert(3);
  tree1.insert(2);
  tree1.insert(1);
  tree1.insert(6);
  BinarySearchTree<int> tree;
  tree.insert(2);
  tree.insert(1);
  tree.insert(6);
  tree = tree1;

  BinarySearchTree<int>::Iterator it = tree.begin();
  it++;
  it++;
  BinarySearchTree<int>::Iterator itt = tree.find(3);
  BinarySearchTree<int>::Iterator itt1 = tree.find(7);
  BinarySearchTree<int>::Iterator itt2 = tree.find(0);
  BinarySearchTree<int>::Iterator itt3 = tree.find(4);
  ASSERT_TRUE(itt1 == tree.end());
  ASSERT_TRUE(itt2 == tree.end()); 
  ASSERT_TRUE(itt3 == tree.end());  
  ASSERT_TRUE(it == itt);
  ASSERT_TRUE(tree.size() == 5);
  ASSERT_TRUE(tree.height() == 4);
//  BinarySearchTree<int>::Iterator it = tree.begin();
  ASSERT_TRUE(tree.check_sorting_invariant());
  ASSERT_TRUE(*tree.max_element() == 6);
  ASSERT_TRUE(*tree.min_element() == 1);
  ASSERT_TRUE(*tree.min_greater_than(3) == 5);
  ostringstream expected;
  expected << "[ 1 2 3 5 6 ]";
  ostringstream actual;
  actual <<  tree;
  ASSERT_EQUAL(expected.str(), actual.str());

  ostringstream oss_preorder;
  tree.traverse_preorder(oss_preorder);
  ASSERT_TRUE(oss_preorder.str() == "5 3 2 1 6 ");

  ostringstream oss_inorder;
  tree.traverse_inorder(oss_inorder);
  ASSERT_TRUE(oss_inorder.str() == "1 2 3 5 6 ");
}

TEST(D2_3_4_5_6) {
  BinarySearchTree<int> tree;

  tree.insert(2);
  tree.insert(3);
  tree.insert(4);
  tree.insert(5);
  tree.insert(6);

  ASSERT_TRUE(tree.size() == 5);
  ASSERT_TRUE(tree.height() == 5);
//  BinarySearchTree<int>::Iterator it = tree.begin();
  ASSERT_TRUE(tree.check_sorting_invariant());
  ASSERT_TRUE(*tree.max_element() == 6);
  ASSERT_TRUE(*tree.min_element() == 2);
  ASSERT_TRUE(*tree.min_greater_than(3) == 4);
  ostringstream expected;
  expected << "[ 2 3 4 5 6 ]";
  ostringstream actual;
  actual <<  tree;
  ASSERT_EQUAL(expected.str(), actual.str());

  ostringstream oss_preorder;
  tree.traverse_preorder(oss_preorder);
  ASSERT_TRUE(oss_preorder.str() == "2 3 4 5 6 ");

  ostringstream oss_inorder;
  tree.traverse_inorder(oss_inorder);
  ASSERT_TRUE(oss_inorder.str() == "2 3 4 5 6 ");
}

TEST(E_find) {
  BinarySearchTree<int> tree;
  tree.insert(20);
  tree.insert(10);
  tree.insert(22);
  tree.insert(21);
  tree.insert(24);
  tree.insert(23);
  tree.insert(25);
  tree.insert(8);
  tree.insert(6);
  tree.insert(9);
  tree.insert(4);
  tree.insert(5);
  tree.insert(7);
  tree.insert(17);
  tree.insert(13);
  tree.insert(18);
  tree.insert(12);  
  tree.insert(11);
  tree.insert(14);
  tree.insert(15);
  BinarySearchTree<int>::Iterator it1 = tree.begin();  
  ASSERT_EQUAL(tree.min_element(),it1);
  for (int i = 0; i<9;i++){
    it1++;
  }
  BinarySearchTree<int>::Iterator itt1 = tree.find(13);
  ASSERT_TRUE(*itt1 == *it1);
  BinarySearchTree<int>::Iterator itt2 = tree.find(3);
  BinarySearchTree<int>::Iterator itt3 = tree.find(19);
  BinarySearchTree<int>::Iterator itt4 = tree.find(26);
  ASSERT_TRUE(itt2 == tree.end());
  ASSERT_TRUE(itt3 == tree.end());
  ASSERT_TRUE(itt4 == tree.end());
  BinarySearchTree<int>::Iterator itt5 = tree.find(4);
  ASSERT_TRUE(*itt5 == *tree.begin());
  BinarySearchTree<int>::Iterator itt6 = tree.find(25);
  BinarySearchTree<int>::Iterator it6 = tree.begin();  
  for (int i = 0; i<19;i++){
    it6++;
  }
  ASSERT_TRUE(*itt6 == *it6);
  ASSERT_TRUE(tree.size() == 20);
  ASSERT_TRUE(tree.height() == 6);
  ASSERT_TRUE(tree.check_sorting_invariant());
  ASSERT_TRUE(*tree.max_element() == 25);
  ASSERT_TRUE(*tree.min_element() == 4);
}

TEST(F_10){
  BinarySearchTree<int> tree;
  vector<int> numbers(10);
  numbers={1,4,5,7,2,3,8,10,6,9};


  cout << "current sequence: [ ";
  for (int i = 0; i < 10; ++i) {
      cout << numbers[i] << " ";
      tree.insert(numbers[i]);
  }
  cout << "]\n";
  ASSERT_TRUE(tree.size() == 10);

  ostringstream expected;
  expected << "[ 1 2 3 4 5 6 7 8 9 10 ]";
  ostringstream actual;
  actual <<  tree;
  ASSERT_EQUAL(expected.str(), actual.str());
  
  BinarySearchTree<int>::Iterator it = tree.begin(); 
  ASSERT_EQUAL(tree.min_element(),it);
  for (int i = 1; i < 11; ++i) {
      ASSERT_EQUAL(*it,*tree.find(i));
      ASSERT_EQUAL(*it,i);
      it++;
  }

  
  
  
}

TEST_MAIN()
