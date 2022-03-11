#ifndef NODE
#define NODE

#include "dataset.hpp"
#include <array>

#define COUNT 20

using std::array;

template <typename T> struct node {
  unsigned short axis;
  array<T, 2> pnt;
  unique_ptr<node> left_ptr;
  unique_ptr<node> right_ptr;
#if defined(_OPENMP)
  unsigned short prc;
#endif

  // Constructor and destructor: again, since all members implement the RAII we
  // don't have to take care of allocating or deallocating the memory by hand
  node() = default;
  ~node() = default;

  // Given the first nodes appends recursively to it nodes until all the
  // points of the 'dataset' given are exhausted
  node *build(dataset<T> &data, size_t first, size_t last, unsigned short dir) {
    size_t size = last - first + 1, mdn = 0;

    if (size < 2) {
      pnt[0] = data[first][0];
      pnt[1] = data[first][1];
      return this;
    }

    axis = data.most_spreaded(first, last);

    if (axis != dir) { // Big optimization
      data.sort(axis, first, last);
    }

    mdn = first + size / 2 - (size < 3);

    pnt[0] = data[mdn][0];
    pnt[1] = data[mdn][1];

#if defined(_OPENMP)
#pragma omp task shared(data) firstprivate(size, axis, first, mdn)
#endif
    if (size > 2) {
      auto tmp = new node{};
      auto tmp_ptr = tmp->build(data, first, mdn - (mdn > 0), axis);
      left_ptr.reset(tmp_ptr);
    }

#if defined(_OPENMP)
#pragma omp task shared(data) firstprivate(axis, last, mdn)
#endif
    {
      auto tmp = new node{};
      auto tmp_ptr = tmp->build(data, mdn + (mdn < last), last, axis);
      right_ptr.reset(tmp_ptr);
    }

    return this;
  }

  // Operator '<<' overloading: in this way the syntax 'cout << node->left_ptr'
  // is supported
  friend ostream &operator<<(ostream &os, const unique_ptr<node> &x) {
    os << x->axis << ' ' << x->pnt[0] << ' ' << x->pnt[1];
    return os;
  }
};

// Builds the binary tree and returns its root as a pointer to a node
template <typename T> node<T> *build(dataset<T> &data, unsigned short dir) {
  auto tmp = new node<T>{};

#if defined(_OPENMP)
#pragma omp parallel shared(data) proc_bind(close)
#pragma omp single
#endif
  tmp = tmp->build(data, 0, data.cardinality - 1, dir);

  return tmp;
}

// Prints the binary tree on terminal given a node as root
template <typename T> void print(const unique_ptr<T> &root) {
  print(root, 0);
}

template <typename T>
void print(const unique_ptr<T> &root, unsigned short space) {
  if (root == nullptr)
    return;

  space += COUNT;
  print(root->right_ptr, space);
  cout << endl;

  for (unsigned short i = COUNT; i < space; i++)
    cout << ' ';

  cout << root << '\n' << endl;

  print(root->left_ptr, space);
}

#endif
