#include <chrono>
#include <iostream>
#include <random>
#include "dataset.hpp"

using std::cout;
using std::endl;
using std::default_random_engine;
using std::uniform_real_distribution;
using std::chrono::steady_clock;
using std::chrono::duration_cast;
using std::chrono::microseconds;

template <typename T> struct tree {
  struct node {
    uint16_t axis;
    T *pnt;
    node *left;
    node *right;

    ~node() {
      delete right;
      delete left;
    }

    // Given the first nodes appends recursively to it nodes until all the
    // points of the 'dataset' given are exhausted
    node *build(dataset<T> &data, size_t first, size_t last, uint16_t dir) {
      size_t size = last - first + 1, mdn = 0;

      if (size < 2) {
        pnt = data[first];
        return this;
      }

      axis = data.most_spreaded(first, last);
 
      if (axis != dir) { // Big optimization
        data.sort(axis, first, last);
      }

      mdn = first + size / 2 - (size < 3);
      pnt = data[mdn];
      // Points on the hyperplane 'axis = point' (?)

      auto tmp = new node{};
      left = (size < 3) ? tmp : tmp->build(data, first, mdn - (mdn > 0), axis);
                                
      tmp = new node{};
      right = tmp->build(data, mdn + (mdn < last), last, axis);

      return this;
    }
  };

  node *head;

  ~tree() { delete head; }

  // Builds a 'tree', after the 'tree' is built the pointer '_head' will point
  // to the first node
  void build(dataset<T> &data) {
    head = new node{};
    head = head->build(data, 0, data.cardinality - 1, 2);
  }
};

int main() {
  using type = double;
  size_t num = 1 << 22;

  // Generating an uniform distribution along the 2 directions in a 'dataset'
  // and measuting the time needed to do it
  default_random_engine gen;
  uniform_real_distribution<type> x(-16.0, 4.0);
  uniform_real_distribution<type> y(-4.0, 4.0);

  dataset<type> data(num);
  for (size_t i = 0; i < num; ++i) {
    data[i][0] = x(gen);
    data[i][1] = y(gen);
  }

  // Building a k-d tree using the 'dataset' constructed and measuring the
  // time needed to do it
  auto begin = steady_clock::now();
  tree<type> kd_tree{};
  kd_tree.build(data);
  auto end = steady_clock::now();

  auto time = duration_cast<microseconds>(end - begin).count();
  cout << "Time to build the serial tree: " << time / 1e+06 << " [s]" << endl;
            
  return 0;
}
