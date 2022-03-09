#ifndef DATASET
#define DATASET

#include <iostream>
#include <memory>  // std::unique_ptr
#include <utility> // std::swap

#define COUNT 20
using std::array;
using std::cout;
using std::endl;
using std::ostream;
using std::size_t;
using std::swap;
using std::unique_ptr;

template <typename T> struct dataset {
  size_t cardinality; // Number of points in the 'dataset'
  unique_ptr<T[]> points;

  // Constructor and destructor: in this way we do not have to take care every
  // time of the deallocation of the memory allocated
  dataset() = default;
  ~dataset() = default;

  // Custom constructor: creates a 'dataset' with a number 'c' of 2 dimensional
  // points
  dataset(size_t c) : cardinality{c}, points{new T[2 * c]} {}

  // Custom constructor: creates a 'dataset' with a number 'c' of 2 dimensional
  // points given a pointer to the data and then puts the pointer to 'nullptr'
  dataset(size_t c, T *&p) : cardinality{c}, points{p} { p = nullptr; }

  // Move semantics
  dataset(dataset &&v) = default;
  dataset &operator=(dataset &&v) = default;

  // Copy semantics
  dataset(const dataset &d)
      : cardinality{d.cardinality}, points{new T[2 * d.cardinality]} {
    std::copy(d.begin(), d.end(), begin());
  }

  dataset &operator=(const dataset &d) {
    points.reset();
    auto tmp = d;
    *this = std::move(tmp);
    return *this;
  }

  // Custom copy constructor: construct a new dataset given an existing one
  // copying only its points from 'f' to 'l'
  dataset(const dataset &d, const size_t first, const size_t last)
      : cardinality{last - first + 1}, points{new T[2 * cardinality]} {
    std::copy(d.that(first), d.that(last + 1), begin());
  }

  // Subscripting operator for the 'dataset':
  //  - 'dataset[n]' returns a pointer to the 2 dimensional point number 'n'
  //  - the 'dataset[n][d]' syntax is also supported in this way and it gives
  //    the coordinate along the 'd' direction of the point number 'n'
  const T *operator[](const size_t i) const { return &points[2 * i]; }
  T *operator[](const size_t i) { return &points[2 * i]; }

  // Begin and end of the data, and also that data
  const T *begin() const { return &points[0]; }
  T *begin() { return &points[0]; }

  const T *end() const { return &points[2 * cardinality]; }
  T *end() { return &points[2 * cardinality]; }

  const T *that(const size_t i) const { return &points[2 * i]; }
  T *that(const size_t i) { return &points[2 * i]; }

  // Returns the direction in which the points from 'first' to 'last' are most
  // spreaded
  auto most_spreaded(const size_t first, const size_t last) {
    T length = 0;
    auto &ref = *this;
    unsigned short dir = 0;

    for (auto d = dir; d < 2; ++d) {
      auto min = ref[first][d], max = min;

      for (auto i = first + 1; i < last + 1; ++i) {
        auto tmp = ref[i][d];
        min = (min > tmp) ? tmp : min;
        max = (max < tmp) ? tmp : max;
      }

      auto tmp = max - min;
      tmp *= ((tmp > 0) - (tmp < 0)); // Absolute value

      if (length < tmp) {
        length = tmp;
        dir = d;
      }
    }

    return dir;
  }

  // Swaps the 2 dimensional point number 'a' with the 2 dimensional point
  // number 'b'
  void swapping(const size_t a, const size_t b) noexcept {
    auto &ref = *this;
    swap(ref[a][0], ref[b][0]);
    swap(ref[a][1], ref[b][1]);
  }

  // Sorts the points of the 'dataset' along the 'axis' given from the element
  // 'first' to the element 'last', and does it in place
  void sort(const unsigned short dir, const size_t first, const size_t last) {
    if (last <= first)
      return;

    auto &ref = *this;
    auto lower = first, upper = last, iter = lower;
    const auto pivot = ref[upper][dir];

    // Dutch flag problem
    while (iter < upper + 1) {
      if (ref[iter][dir] > pivot)
        swapping(iter, upper--);
      else if (ref[iter][dir] < pivot)
        swapping(iter++, lower++);
      else
        ++iter;
    }

    sort(dir, first, lower - (lower > 0));
    sort(dir, iter, last);
  }

  // Splits the dataset in the half before the pivot and the half after the
  // pivot the half before is saved in the current dataset which is properly
  // updated while the half after is returned by the function as a dataset
  dataset split(const size_t pivot) {
    dataset a{*this, 0, pivot - 1}, b{*this, pivot + 1, cardinality - 1};
    *this = std::move(a);
    return b;
  }

  friend ostream &operator<<(ostream &os, const dataset<T> &x) {
    for (size_t i = 0; i < x.cardinality; ++i)
      os << x[i][0] << ' ' << x[i][1] << endl;

    return os;
  }
};

#endif
