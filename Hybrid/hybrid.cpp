#include "dataset.hpp"
#include "info.hpp"
#include "tree.hpp"
#include <chrono>
#include <cstring>
#include <random>
#include <mpi.h>

using std::default_random_engine;
using std::uniform_real_distribution;
using std::chrono::duration_cast;
using std::chrono::microseconds;
using std::chrono::steady_clock;

#if defined(_OPENMP)
int main(int argc, char **argv) {
#else
int main() {
#endif
  using type = double;
  size_t num = 1 << 22, mdn = num / 2, lenght = num - mdn - 1;
  int rank, size, master = 0, slave = 1, send_data = 2, send_pnt = 3;
  node<type> head;
  MPI_Status status;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  cout << "I am " << rank << " of " << size << endl;

  if (rank == master) {
    default_random_engine gen;
    uniform_real_distribution<type> x(-10.0, 0.0);
    uniform_real_distribution<type> y(0.0, 8.0);

    auto *ptr = new type[2 * num];
    dataset<type> data(num, ptr);

    for (size_t i = 0; i < num; ++i) {
      data[i][0] = x(gen);
      data[i][1] = y(gen);
    }

    auto axis = data.most_spreaded(0, num - 1);
    data.sort(axis, 0, num - 1);
    head.pnt[0] = data[mdn][0];
    head.pnt[1] = data[mdn][1];

    MPI_Send(&head.pnt, 2, MPI_DOUBLE, slave, send_pnt, MPI_COMM_WORLD);

    auto *ptr_2 = new type[2 * length];

    for (size_t i = 0; i < length; i++) {
      ptr_2[i * 2] = data[i + mdn + 1][0];
      ptr_2[i * 2 + 1] = data[i + mdn + 1][1];
    }
    
    MPI_Send(&ptr_2, 2 * length, MPI_DOUBLE, slave, send_data, MPI_COMM_WORLD);
    
    delete[] ptr_2;
    
    auto *ptr_1 = new type[2 * mdn];

    for (size_t i = 0; i < mdn; i++) {
      ptr_1[i * 2] = data[i][0];
      ptr_1[i * 2 + 1] = data[i][1];
    }
    
    delete[] ptr;
  } else {
    MPI_Recv(&head.pnt, 2, MPI_DOUBLE, master, send_pnt, MPI_COMM_WORLD, &status);
    auto *ptr_1 = new type[2 * length];
    MPI_Recv(&ptr_1, 2, MPI_DOUBLE, master, send_data, MPI_COMM_WORLD, &status);
  }
  /*    using type = double;
      size_t num = 1 << 22;
      type *pdata = new type[2 * num];

      // Generating an uniform distribution along the 2 directions in a
    'dataset'
      // and measuting the time needed to do it
      default_random_engine gen;
      uniform_real_distribution<type> x(-10.0, 0.0);
      uniform_real_distribution<type> y(0.0, 8.0);

      dataset<type> data(num, pdata);

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

      delete[] pdata; */

  delete[] ptr_1;
  MPI_Finalize();
  return 0;
}
