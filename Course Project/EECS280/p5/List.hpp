#ifndef LIST_HPP
#define LIST_HPP
/* List.hpp
 *
 * doubly-linked, double-ended list with Iterator interface
 * EECS 280 List/Editor Project
 */

#include <iostream>
#include <iterator> //std::bidirectional_iterator_tag
#include <cassert>  //assert


template <typename T>
class List {
  //OVERVIEW: a doubly-linked, double-ended list with Iterator interface
public:
  // Default Constructor
List() : size_list(0), first(nullptr), last(nullptr) {}

// Destructor
~List() {
  clear();
}

// Copy Constructor
List(const List &other) : size_list(0), first(nullptr), last(nullptr) {
  copy_all(other);
}

// Assignment Operator
List& operator=(const List &other) {
  if (this != &other) {
    clear();
    copy_all(other);
  }
  return *this;
}
  //EFFECTS:  returns true if the list is empty
  bool empty() const{
    return (first == nullptr);
  };

  //EFFECTS: returns the number of elements in this List
  //HINT:    Traversing a list is really slow. Instead, keep track of the size
  //         with a private member variable. That's how std::list does it.
  int size() const{
    return size_list;
  };

  //REQUIRES: list is not empty
  //EFFECTS: Returns the first element in the list by reference
  T & front(){
    assert(!empty());
    return first->datum;
  };

  //REQUIRES: list is not empty
  //EFFECTS: Returns the last element in the list by reference
  T & back(){
    assert(!empty());
    return last->datum;
  };

  //EFFECTS:  inserts datum into the front of the list
  void push_front(const T &datum){
    Node *p = new Node;
    p->datum = datum;
    p->next = first;
    p->prev = nullptr;
    if (first) first->prev = p;
    first = p;
    if (!last) last = p; // If list was empty
    size_list ++;
  };

  //EFFECTS:  inserts datum into the back of the list
  void push_back(const T &datum){
    Node *p = new Node;
    p->datum = datum;
    p->next = nullptr;
    p->prev = last;
    if (last) last->next = p; 
    last = p;
    if (!first) first = p; // If list was empty
    size_list ++;
  };

  //REQUIRES: list is not empty
  //MODIFIES: may invalidate list iterators
  //EFFECTS:  removes the item at the front of the list
  void pop_front(){
    assert(!empty());
    Node *tobedeleted = first;
    first = first->next;
    delete tobedeleted;
    size_list --;
  };

  //REQUIRES: list is not empty
  //MODIFIES: may invalidate list iterators
  //EFFECTS:  removes the item at the back of the list
  void pop_back(){
    assert(!empty());
    Node *tobedeleted = last;
    last = last->prev;
    delete tobedeleted;
    size_list --;
  };

  //MODIFIES: may invalidate list iterators
  //EFFECTS:  removes all items from the list
  void clear(){
    while (!empty()){
      pop_front();
    }
  };

  // You should add in a default constructor, destructor, copy constructor,
  // and overloaded assignment operator, if appropriate. If these operations
  // will work correctly without defining these, you should omit them. A user
  // of the class must be able to create, copy, assign, and destroy Lists.

private:
  //a private type
  struct Node {
    Node *next;
    Node *prev;
    T datum;
  };

  int size_list;
  //REQUIRES: list is empty
  //EFFECTS:  copies all nodes from other to this
  void copy_all(const List<T> &other){
    for (Node *np = other.first; np; np = np->next) {
    push_back(np->datum);
}
  };

  Node *first;   // points to first Node in list, or nullptr if list is empty
  Node *last;    // points to last Node in list, or nullptr if list is empty

public:
  ////////////////////////////////////////
  class Iterator {
  friend class List;
  public:
    //OVERVIEW: Iterator interface to List

    // Add a default constructor here. The default constructor must set both
    // pointer members to null pointers.
    Iterator(){
    list_ptr = nullptr;
    node_ptr = nullptr; 
    }

    // Add custom implementations of the destructor, copy constructor, and
    // overloaded assignment operator, if appropriate. If these operations
    // will work correctly without defining these, you should omit them. A user
    // of the class must be able to copy, assign, and destroy Iterators.
    ~Iterator() = default;

    Iterator(const Iterator &other)
      : list_ptr(other.list_ptr), node_ptr(other.node_ptr) {}

    Iterator& operator=(const Iterator &other) {
      if (this != &other) {  // Avoid self-assignment
        list_ptr = other.list_ptr;
        node_ptr = other.node_ptr;
      }
      return *this;
    }

    // Your iterator should implement the following public operators:
    // *, ++ (both prefix and postfix), == and !=.
    // Equality comparisons must satisfy the following rules:
    // - two default-constructed iterators must compare equal
    // - a default-constructed iterator must compare unequal to an
    //   iterator obtained from a list, even if it is the end iterator
    // - two iterators to the same location in the same list must
    //   compare equal
    // - two iterators to different locations in the same list must
    //   compare unequal
    // - comparing iterators obtained from different lists results in
    //   undefined behavior
    //   - Your implementation can handle this however you like, but
    //     don't assume any particular behavior in your test cases --
    //     comparing iterators from different lists essentially
    //     violates the REQURES clause.
    // Note: comparing both the list and node pointers should be
    // sufficient to meet these requirements.



    // Type aliases required to work with STL algorithms. Do not modify these.
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;

    // This operator will be used to test your code. Do not modify it.
    // REQUIRES: Iterator is decrementable. All iterators associated with a
    //           list are decrementable, including end iterators, except for
    //           begin iterators (those equal to begin() on the list)
    // EFFECTS:  moves this Iterator to point to the previous element
    //           and returns a reference to this Iterator
    Iterator& operator--() { // prefix -- (e.g. --it)
      assert(list_ptr);
      assert(*this != list_ptr->begin());
      if (node_ptr) {
        node_ptr = node_ptr->prev;
      } else { // decrementing an end Iterator moves it to the last element
        node_ptr = list_ptr->last;
      }
      return *this;
    }

    T& operator*() const {
      assert(node_ptr);
      return node_ptr->datum;
    }

    Iterator& operator++() {
      assert(node_ptr);
      node_ptr = node_ptr->next;
      return *this;
    }

    bool operator==(const Iterator &other) const {
      return list_ptr == other.list_ptr && node_ptr == other.node_ptr;
    }

    bool operator!=(const Iterator &other) const {
      return !(*this == other);
    }


    // This operator will be used to test your code. Do not modify it.
    // REQUIRES: Iterator is decrementable. All iterators associated with a
    //           list are decrementable, including end iterators, except for
    //           begin iterators (those equal to begin() on the list)
    // EFFECTS:  moves this Iterator to point to the previous element
    //           and returns a copy of the original Iterator
    Iterator operator--(int /*dummy*/) { // postfix -- (e.g. it--)
      Iterator copy = *this;
      operator--();
      return copy;
    }

    // REQUIRES: Iterator is dereferenceable
    // EFFECTS: returns the underlying element by pointer
    // NOTE: T must be a class type to use the syntax it->. If T has a
    //       member variable f, then it->f accesses f on the
    //       underlying T element.
    T* operator->() const {
      return &operator*();
    }

  private:
    const List *list_ptr; //pointer to the List associated with this Iterator
    Node *node_ptr; //current Iterator position is a List node
    // add any additional necessary member variables here


    // add any friend declarations here


    // construct an Iterator at a specific position in the given List
    Iterator(const List *lp, Node *np):list_ptr(lp),node_ptr(np){};

  };//List::Iterator
  ////////////////////////////////////////

  // return an Iterator pointing to the first element
  Iterator begin() const{
    return Iterator(this, first);
  };

  // return an Iterator pointing to "past the end"
  Iterator end() const{
    return Iterator(this, nullptr);
  };

  //REQUIRES: i is a valid, dereferenceable iterator associated with this list
  //MODIFIES: may invalidate other list iterators
  //EFFECTS: Removes a single element from the list container.
  //         Returns An iterator pointing to the element that followed the
  //         element erased by the function call

  List<T>::Iterator erase(Iterator i){
    assert(i.node_ptr);

    Node *to_delete = i.node_ptr;
    Node *next_node = to_delete->next;
    Node *prev_node = to_delete->prev;

    if (prev_node) prev_node->next = next_node;
    else first = next_node; // Erasing first node

    if (next_node) next_node->prev = prev_node;
    else last = prev_node; // Erasing last node

    delete to_delete;
    size_list--;

    return Iterator(this, next_node);
};

  //REQUIRES: i is a valid iterator associated with this list
  //EFFECTS: Inserts datum before the element at the specified position.
  //         Returns an iterator to the the newly inserted element.
  Iterator insert(Iterator i, const T &datum) {
    Node *next_node = i.node_ptr;
    Node *prev_node;
    if (next_node) {
      prev_node = next_node->prev;
    } else {
      prev_node = last;
    }
    Node *new_node = new Node{datum, next_node, prev_node};
    if (prev_node) prev_node->next = new_node;
    else first = new_node; // Inserting at the front
    if (next_node) next_node->prev = new_node;
    else last = new_node; // Inserting at the back
    size_list++;
    return Iterator(new_node);
  }

};//List


////////////////////////////////////////////////////////////////////////////////
// Add your member function implementations below or in the class above
// (your choice). Do not change the public interface of List, although you
// may add the Big Three if needed.  Do add the public member functions for
// Iterator.


#endif // Do not remove this. Write all your code above this line.
