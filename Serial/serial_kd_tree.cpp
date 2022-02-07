#include <iostream>

/* DATASET ------------------------------------------------------------------ */
template <typename T, short unsigned D = 2, typename I = std::size_t>
class dataset {
  std::size_t _cardinality;
  T *points;

public:
  dataset(std::size_t cardinality)
      : _cardinality{cardinality}, points{new T[_cardinality * D]} {}

  ~dataset() { delete[] points; }

  // Subscripting operator for the 'dataset', it supports the 'dataset[][]'
  // syntax (remark: only the 2-dimensional case is supported for now)
  T *operator[](const std::size_t ind) { return &points[ind * D]; }

  // Number of points in the 'dataset'
  auto cardinality() const noexcept { return _cardinality; };

  // Returns the direction in which the points from 'first' to 'last' are most
  // spreaded
  auto most_spreaded(const I first, const I last) noexcept {
    T length{0};
    I dim{0};

    for (I axis = 0; axis < D; ++axis) {
      auto min = (*this)[first][axis];
      auto max = (*this)[first][axis];

      for (I ind = first; ind < last + 1; ++ind) {
        auto tmp = (*this)[ind][axis];

        min = (min > tmp) ? tmp : min;
        max = (max < tmp) ? tmp : max;
      }

      T tmp = max - min;
      length = length * ((length > 0) - (length < 0));
      tmp = tmp * ((tmp > 0) - (tmp < 0));

      if (length < tmp) {
        length = tmp;
        dim = axis;
      }
    }

    return dim;
  }

  // Swaps the multi-dimensional point number 'a' with the multi-dimensional
  // point number 'b'
  void swap(const I a, const I b) noexcept {
    for (I i = 0; i < D; ++i)
      std::swap((*this)[a][i], (*this)[b][i]);
  }

  // Sorts the points of the 'dataset' along the 'axis' given from the element
  // 'first' to the element 'last', and does it in place
  void quick_sort(const I axis, const I first, const I last) noexcept {
    if (last <= first)
      return;

    I lower{first}, upper{last}, iterator{first};
    const T pivot = (*this)[upper][axis];

    while (iterator < upper + 1) {
      if ((*this)[iterator][axis] > pivot)
        swap(iterator, upper--);
      else if ((*this)[iterator][axis] < pivot)
        swap(iterator++, lower++);
      else
        ++iterator;
    }

    quick_sort(axis, first, lower - (lower != 0));
    quick_sort(axis, iterator, last);
  }
};

/* TREE --------------------------------------------------------------------- */
template <typename T, short unsigned D = 2, typename I = std::size_t>
class tree {
  class node {
    I _axis{0};
    T *_point{nullptr};
    node *_left{nullptr};
    node *_right{nullptr};

  public:
    ~node() {
      delete _right;
      delete _left;
    }

    auto axis() const noexcept { return _axis; }
    auto left() const noexcept { return _left; }
    auto right() const noexcept { return _right; }
    auto point() const noexcept { return _point; }

    // Given the first nodes appends recursively to it nodes until all the
    // points of the 'dataset' given are exhausted
    node *build_nodes(dataset<T> &data, I first, I last) {
      I size{last - first + 1};
      I median{0};

      if (size == 1) {
        _point = data[first];
        return this;
      }

      _axis = data.most_spreaded(first, last);
      data.quick_sort(_axis, first, last);
      median = first + size / 2;
      _point = data[median];
      // Points on the hyperplane 'axis = point' (?)

      if (size != 2) {
        auto left_node = new node{};
        _left = left_node->build_nodes(data, first, median - (median != 0));
      }

      auto right_node = new node{};
      _right = right_node->build_nodes(data, median + (median != last), last);

      return this;
    }
  };

  node *_head{nullptr};

public:
  ~tree() { delete _head; }

  // Builds a 'tree', after the 'tree' is built the pointer '_head' will point
  // to the first node
  void build_tree(dataset<T> &data) {
    _head = new node{};
    _head = _head->build_nodes(data, 0, data.cardinality() - 1);
  }

  auto head() const noexcept { return _head; }
};

/* MAIN --------------------------------------------------------------------- */
int main() { return 0; }
