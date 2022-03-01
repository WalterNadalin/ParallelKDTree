#include "dataset.hpp"
#include "info.hpp"
#include <chrono>
#include <random>
#include <cstring>

using std::cout;
using std::default_random_engine;
using std::endl;
using std::uniform_real_distribution;
using std::chrono::duration_cast;
using std::chrono::microseconds;
using std::chrono::steady_clock;

template <typename T> struct tree {
  struct node {
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

  node *head;

  ~tree() { delete head; }

  // Builds a 'tree', after the 'tree' is built the pointer '_head' will point
  // to the first node
  void build(dataset<T> &data) {
    head = new node{};
#if defined(_OPENMP)
#pragma omp parallel shared(data) proc_bind(close)
#pragma omp single
    head = head->build(data, 0, data.cardinality - 1, 2);
#else
    head = head->build(data, 0, data.cardinality - 1, 2);
#endif
  }
};

#if defined(_OPENMP)
int main(int argc, char **argv) {
#else
int main() {
#endif
  using type = double;
  size_t num = 1 << 22;

  // Generating an uniform distribution along the 2 directions in a 'dataset'
  // and measuting the time needed to do it
  default_random_engine gen;
  uniform_real_distribution<type> x(-10.0, 0.0);
  uniform_real_distribution<type> y(0.0, 8.0);

  dataset<type> data(num);
  for (size_t i = 0; i < num; ++i) {
    data[i][0] = x(gen);
    data[i][1] = y(gen);
  }

  tree<type> kdtree;

  // Building a k-d tree using the 'dataset' constructed and measuring the
  // time needed to do it
#if defined(_OPENMP)
  auto nthreads = (argc > 1) ? atoi(argv[1]) : 4;
  omp_set_num_threads(nthreads);

  if(argc > 2) {
    if(strcmp(argv[2], "info") == 0)
#pragma omp parallel shared(data) proc_bind(close)
      info();
    else
      std::cout << "\nUnknown command passed\n" << std::endl;
    
  }
  auto begin = omp_get_wtime();
  kdtree.build(data);
  auto end = omp_get_wtime();
  cout << "Parallel tree: " << end - begin << " [s]" << endl;
#else
  auto begin = steady_clock::now();
  kdtree.build(data);
  auto end = steady_clock::now();
  auto time = duration_cast<microseconds>(end - begin).count();
  cout << "Serial tree: " << time / 1e+06 << " [s]" << endl;
#endif

  auto tmp = kdtree.head;
  auto cnt = 0;
  while (tmp) {
    cout << '(' << tmp->pnt[0] << ", " << tmp->pnt[1] << ')' << endl;

    if (cnt % 2)
      tmp = tmp->left;
    else
      tmp = tmp->right;

    ++cnt;
  }
  cout << cnt << endl;
  return 0;
}
