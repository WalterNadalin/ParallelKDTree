#ifndef INFO
#define INFO

#include <iostream>
#include <omp.h>
#include <string>

using std::cout;
using std::endl;
using std::string;

void info() noexcept {
#if defined(_OPENMP)
  auto nthreads = omp_get_num_threads();

#pragma omp single
  {
    string places = getenv("OMP_PLACES");
    string names[] = {"false", "true", "master", "close", "spread"};
    auto binding = omp_get_proc_bind();

    cout << '\n'
         << nthreads << " threads in execution | The places are " << places
         << " with a " << names[binding] << " binding policy" << '\n'
         << "---------------------------------------------------------\n"
         << "Additional info for each thread: " << endl;
  }

  auto me = omp_get_thread_num(), place = omp_get_place_num();
  auto nprocs = omp_get_place_num_procs(place);

  int proc_ids[nprocs];
  omp_get_place_proc_ids(place, proc_ids);

#pragma omp barrier

#pragma omp for ordered
  for (int i = 0; i < nthreads; i++)
#pragma omp ordered
  {
    cout << "Thread number " << me << " in place number " << place
         << " | Processors here: ";

    for (int p = 0; p < nprocs; p++)
      cout << proc_ids[p] << ' ';

    cout << endl;
  }

#pragma omp single
  cout << "---------------------------------------------------------\n" << endl;
#else
  cout << '\n';
  cout << "Serial version: nothing to see here";
  cout << '\n' << endl;
#endif
}
#endif
