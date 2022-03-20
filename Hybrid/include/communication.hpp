#ifndef __TREECOMMUNICATION__
#define __TREECOMMUNICATION__

#include "node.hpp"
#include <cmath>

#define TAG_PNT 0
#define TAG_DAT 1
#define TAG_DIR 2
#define TAG_LEN 3
#define TAG_SIZ 4

using std::move;

template <typename T, typename N>
N *distribute(int master, int slave, int size, unsigned short &axis,
              unique_ptr<N> &tree, dataset<T> &data) {
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == master) {
    size_t crd = data.cardinality, mdn = crd / 2, length = crd - mdn - 1;
    auto dir = data.most_spreaded(0, crd - 1);

    if (axis != dir) {
      axis = dir;
      data.sort(axis, 0, crd - 1);
    }
    
    tree->pnt[0] = data[mdn][0];
    tree->pnt[1] = data[mdn][1];
    tree->axis = axis;
    // Sending useful informations to the slave
    MPI_Send(&size, 1, MPI_INT, slave, TAG_SIZ, MPI_COMM_WORLD);
    MPI_Send(&length, 1, MPI_UNSIGNED_LONG, slave, TAG_LEN, MPI_COMM_WORLD);
    MPI_Send(&axis, 1, MPI_UNSIGNED_SHORT, slave, TAG_DIR, MPI_COMM_WORLD);
    MPI_Send(data[mdn], 2, MPI_DOUBLE, slave, TAG_PNT, MPI_COMM_WORLD);

    // Splitting the data in half: the master keeps the lower half for
    // itself and send the other half to the slave
    dataset<T> upper = data.split(mdn);
    MPI_Send(upper.points.get(), 2 * upper.cardinality, MPI_DOUBLE, slave,
             TAG_DAT, MPI_COMM_WORLD);

    // Recursion going to the left branch
    N *head = tree.get();
    head->left_prc = rank;
    head->right_prc = rank + size;
    if (size / 2 > 0) {
      tree->left_ptr.reset(new N{});
      head = distribute(rank, rank + size / 2, size / 2, axis, tree->left_ptr,
                        data);
    }

    return head;
  } else {
    MPI_Status status;
    size_t length;
    T *pnt_ptr = new T[2];

    // Receveing the information from the master
    MPI_Recv(&size, 1, MPI_INT, MPI_ANY_SOURCE, TAG_SIZ, MPI_COMM_WORLD,
             &status);
    MPI_Recv(&length, 1, MPI_UNSIGNED_LONG, MPI_ANY_SOURCE, TAG_LEN,
             MPI_COMM_WORLD, &status);
    MPI_Recv(&axis, 1, MPI_UNSIGNED_SHORT, MPI_ANY_SOURCE, TAG_DIR,
             MPI_COMM_WORLD, &status);
    MPI_Recv(pnt_ptr, 2, MPI_DOUBLE, MPI_ANY_SOURCE, TAG_PNT, MPI_COMM_WORLD,
             &status);

    // Exploiting the information receive
    tree->pnt[0] = pnt_ptr[0];
    tree->pnt[1] = pnt_ptr[1];
    tree->axis = axis;
    delete pnt_ptr;
    T *data_ptr = new T[2 * length];

    // Receiving the lower part of the dataset
    MPI_Recv(data_ptr, 2 * length, MPI_DOUBLE, MPI_ANY_SOURCE, TAG_DAT,
             MPI_COMM_WORLD, &status);
    dataset<T> tmp(length, data_ptr);
    data = move(tmp);

    // Recursion going to the right branch
    N *head = tree.get();
    head->left_prc = status.MPI_SOURCE;
    head->right_prc = rank;
    if (size / 2 > 0) {
      tree->right_ptr.reset(new N{});
      head = distribute(rank, rank + size / 2, size / 2, axis, tree->right_ptr,
                        data);
    }

    return head;
  }
}
#endif
