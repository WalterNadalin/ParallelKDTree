#include "dataset.hpp"
#include "info.hpp"
#include "node.hpp"
#include <chrono>
#include <random>
#include <mpi.h>

using std::default_random_engine;
using std::uniform_real_distribution;
using std::chrono::duration_cast;
using std::chrono::microseconds;
using std::chrono::steady_clock;

int main(int argc, char **argv) {
  using type = double;
  using node_type = node<type>;
  int rank, size;
  int master = 0, slave = 1;
  int send_data = 0;
  size_t num = 15, mdn = num / 2;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  dataset<type> data{};
  unique_ptr<node_type> tree{new node_type{}};

// #if defined(_OPENMP)
if (rank == master) {
    // Generating an uniform distribution along the 2 directions in a 'dataset'
    default_random_engine gen;
    uniform_real_distribution<type> x(-10.0, 0.0);
    uniform_real_distribution<type> y(0.0, 20.0);
    dataset<type> tmp(num);

    for (size_t i = 0; i < num; ++i) {
      tmp[i][0] = x(gen);
      tmp[i][1] = y(gen);
    }

    data = std::move(tmp);
    cout << data;
    cout << endl;
    auto axis = data.most_spreaded(0, num - 1);
    data.sort(axis, 0, num - 1);
    tree->pnt[0] = data[mdn][0];
    tree->pnt[1] = data[mdn][1];

    dataset<type> upper = data.split(mdn);
    MPI_Send(upper.points.get(), 2 * upper.cardinality, MPI_DOUBLE, slave,
    send_data, MPI_COMM_WORLD);

    // Building a k-d tree using the 'dataset' constructed and measuring the
    // time needed to do it
    // auto begin = omp_get_wtime();
    cout << "Processor " << rank << endl;
    tree->left_ptr.reset(build(data));
    // auto end = omp_get_wtime();
    // cout << "Parallel tree time: " << end - begin << " [s]" << endl;
    // info("print", tree);
  } else {
     int length = num - mdn - 1;
     MPI_Status status;
     type *data_ptr = new type[2 * length];
     MPI_Recv(data_ptr, 2 * length, MPI_DOUBLE, master, send_data, 
     MPI_COMM_WORLD, &status);
     dataset<type> data(length, data_ptr);
     tree->right_ptr.reset(build(data));
     cout << "Processor " << rank << endl;
     print(tree);
  }
//#else
  /* auto begin = steady_clock::now();
  unique_ptr<node_type> tree{build(data)};
  auto end = steady_clock::now();
  auto time = duration_cast<microseconds>(end - begin).count();
  cout << "Serial tree time: " << time / 1e+06 << " [s]" << endl;
  info("print", tree); */
//#endif

  MPI_Finalize();
  return 0;
}
