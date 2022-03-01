#ifndef DATASET
#define DATASET

#include "info.hpp"
#include <cstdint>
#include <omp.h>
#include <utility>

using std::size_t;

template <typename T> struct dataset {
  size_t cardinality; // Number of points in the 'dataset'
  T *points;

  // Creates a 'dataset' with a number 'cardinality' of 2 dimensional points
  dataset(size_t c) : cardinality{c}, points{new T[2 * c]} {}
  ~dataset() { delete[] points; }

  // Subscripting operator for the 'dataset':
  //  - 'dataset[n]' returns a pointer to the 2 dimensional point number 'n'
  //  - the 'dataset[n][d]' syntax is also supported in this way and it gives
  //    the coordinate along the 'd' direction of the point number 'n'
  T *operator[](const size_t i) { return &points[2 * i]; }

  // Returns the direction in which the points from 'first' to 'last' are most
  // spreaded
  auto most_spreaded(const size_t first, const size_t last) {
    T length = 0;
    auto &ref = *this;
    uint8_t dir = 0;

    for (auto d = dir; d < 2; ++d) {
      auto min = ref[first][d], max = min;

      for (auto i = first + 1; i < last + 1; ++i) {
        auto tmp = ref[i][d];
        min = (min > tmp) ? tmp : min;
        max = (max < tmp) ? tmp : max;
      }

      auto tmp = max - min;
      tmp *= ((tmp > 0) - (tmp < 0));

      if (length < tmp) {
        length = tmp;
        dir = d;
      }
    }

    return dir;
  }

  // Swaps the 2 dimensional point number 'a' with the 2 dimensional point
  // number 'b'
  void swap(const size_t a, const size_t b) noexcept {
    auto &ref = *this;
    std::swap(ref[a][0], ref[b][0]);
    std::swap(ref[a][1], ref[b][1]);
  }

  // Sorts the points of the 'dataset' along the 'axis' given from the element
  // 'first' to the element 'last', and does it in place
  void sort(const uint8_t dir, const size_t first, const size_t last) {
    if (last <= first)
      return;

    auto &ref = *this;
    auto lower = first, upper = last, iter = lower;
    const auto pivot = ref[upper][dir];

    while (iter < upper + 1) {
      if (ref[iter][dir] > pivot)
        swap(iter, upper--);
      else if (ref[iter][dir] < pivot)
        swap(iter++, lower++);
      else
        ++iter;
    }

    sort(dir, first, lower - (lower > 0));
    sort(dir, iter, last);
  }
};
#endif
