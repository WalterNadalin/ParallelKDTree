#include "dataset.hpp"
#include "info.hpp"
#include "node.hpp"
#include <chrono>
#include <mpi.h>
#include <random>
#include <thread>

using std::default_random_engine;
using std::move;
using std::uniform_real_distribution;
using std::chrono::duration_cast;
using std::chrono::microseconds;
using std::chrono::seconds;
using std::chrono::steady_clock;
using std::this_thread::sleep_for;

int main(int argc, char **argv) {
  using type = double;
  using node_type = node<type>;
  size_t num = 15;

#if defined(_OPENMP)
  double end, begin;
  int rank, size;
  int master = 0, slave = 1;
  int send_data = 0, send_pnt = 1;
  size_t mdn = num / 2;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  dataset<type> data{};
  unique_ptr<node_type> tree{new node_type{}};

  if (argc > 1) {
    auto nthreads = atoi(argv[1]);
    omp_set_num_threads(nthreads);
  }

  // Generating an uniform distribution along the 2 directions in a 'dataset'
  if (rank == master) {
    default_random_engine gen;
    uniform_real_distribution<type> x(-10.0, 0.0);
    uniform_real_distribution<type> y(0.0, 20.0);
    dataset<type> tmp(num);

    for (size_t i = 0; i < num; ++i) {
      tmp[i][0] = x(gen);
      tmp[i][1] = y(gen);
    }

    data = move(tmp);
  }

  // Building the tree
  auto start = MPI_Wtime();
  if (rank == master) {
    auto axis = data.most_spreaded(0, num - 1);
    data.sort(axis, 0, num - 1);
    tree->pnt[0] = data[mdn][0];
    tree->pnt[1] = data[mdn][1];
    MPI_Send(data[mdn], 2, MPI_DOUBLE, slave, send_pnt, MPI_COMM_WORLD);

    dataset<type> upper = data.split(mdn);
    MPI_Send(upper.points.get(), 2 * upper.cardinality, MPI_DOUBLE, slave,
             send_data, MPI_COMM_WORLD);

    // Building a k-d tree using the 'dataset' constructed and measuring the
    // time needed to do it
    begin = omp_get_wtime();
    tree->right_ptr.reset(build(data));
    end = omp_get_wtime();
  } else {
    MPI_Status status;

    type *pnt_ptr = new type[2];
    MPI_Recv(pnt_ptr, 2, MPI_DOUBLE, master, send_pnt, MPI_COMM_WORLD, &status);
    tree->pnt[0] = pnt_ptr[0];
    tree->pnt[1] = pnt_ptr[1];
    delete[] pnt_ptr;

    int length = num - mdn - 1;
    type *data_ptr = new type[2 * length];
    MPI_Recv(data_ptr, 2 * length, MPI_DOUBLE, master, send_data,
             MPI_COMM_WORLD, &status);

    dataset<type> data(length, data_ptr);
    begin = omp_get_wtime();
    tree->left_ptr.reset(build(data));
    end = omp_get_wtime();
  }
  auto finish = MPI_Wtime();

  // Printing some info if the user requires it
  if (argc > 2) {
    MPI_Barrier(MPI_COMM_WORLD);

    for (int i = 0; i < size; ++i) {
      if (rank == i) {
        cout << "\nProcess " << rank;
        info(argv[2], tree);
        cout << "Construction time: " << end - begin
             << " | Communication and construction time: " << finish - start
             << "\n____________________________________________________________"
                "______________________"
             << endl;
      }

      sleep_for(seconds(1));
    }
  }
  //#else
  /* auto begin = steady_clock::now();
  unique_ptr<node_type> tree{build(data)};
  auto end = steady_clock::now();
  auto time = duration_cast<microseconds>(end - begin).count();
  cout << "Serial tree time: " << time / 1e+06 << " [s]" << endl;
  info("print", tree); */
#endif

  MPI_Finalize();
  return 0;
}
