#ifndef TREE
#define TREE

#include "dataset.hpp"

template <typename T> struct node {
  uint16_t axis;
  T pnt[2];
  node *left;
  node *right;

  ~node() {
    delete right;
    delete left;
  }

  // Given the first nodes appends recursively to it nodes until all the
  // points of the 'dataset' given are exhausted
  node *build(dataset<T> &data, size_t first, size_t last, uint8_t dir) {
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
    // Points on the hyperplane 'axis = point' (?)

#if defined(_OPENMP)
#pragma omp task shared(data) firstprivate(size, axis, first, mdn)
    {
      auto l = new node{};
      left = (size < 3) ? l : l->build(data, first, mdn - (mdn > 0), axis);
    }
#pragma omp task shared(data) firstprivate(axis, last, mdn)
    {
      auto r = new node{};
      right = r->build(data, mdn + (mdn < last), last, axis);
    }
#else
    auto l = new node{};
    auto r = new node{};
    left = (size < 3) ? l : l->build(data, first, mdn - (mdn > 0), axis);
    right = r->build(data, mdn + (mdn < last), last, axis);
#endif
    return this;
  }
};

template <typename T> struct tree {

  node<T> *head;

  ~tree() { delete head; }

  // Builds a 'tree', after the 'tree' is built the pointer '_head' will point
  // to the first node
  void build(dataset<T> &data) {
    head = new node<T>{};
#if defined(_OPENMP)
#pragma omp parallel shared(data) proc_bind(close)
    {
      info();
#pragma omp single
      head = head->build(data, 0, data.cardinality - 1, 2);
    }
#else
    info();
    head = head->build(data, 0, data.cardinality - 1, 2);
#endif
  }
};

#endif
