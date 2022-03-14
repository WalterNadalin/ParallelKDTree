#ifndef INFO
#define INFO

#include "node.hpp"
#include <map> # std::map:
#include <string>

#if defined(_OPENMP)
#define TIME time(c, cc)
#else
#define TIME time(c)
#endif

using std::map;
using std::string;

enum class values { nd, all, info, print, time };

template <typename T> void initialize(T &m) {
  m["all"] = values::all;
  m["print"] = values::print;
  m["info"] = values::info;
  m["time"] = values::time;
}

// Prints some information about the threads running
void info() noexcept {
#if defined(_OPENMP)
#pragma omp parallel proc_bind(close)
  {
    auto nthreads = omp_get_num_threads();

#pragma omp single
    {
      string places = getenv("OMP_PLACES");
      string names[] = {"false", "true", "master", "close", "spread"};
      auto binding = omp_get_proc_bind();

      cout << '\n'
           << nthreads << " threads in execution | The places are " << places
           << " with a " << names[binding] << " binding policy" << '\n'
           << "----------------------------------------"
           << "----------------------------------------\n"
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
    cout << "----------------------------------------"
         << "----------------------------------------" << endl;
  }
#else
  cout << '\n';
  cout << "Serial version: only one thread spawned by one process is running";
  cout << endl;
#endif
}

#if defined(_OPENMP)
template <typename T, typename R> void time(T c, R cc) {
  cout << "\nConstruction time: " << c
       << " | Communication and construction time: " << cc << endl;
}
#else
template <typename T> void time(T c) {
  cout << "\nConstruction time: " << c << endl;
}
#endif

// Prints required information evaluating the commands passed throught the
// command line
#if defined(_OPENMP)
template <typename T, typename R, typename S>
void info(string x, const T &tree, R c, S cc) noexcept {
#else
template <typename T, typename R>
void info(string x, const T &tree, R c) noexcept {
#endif
  map<string, values> mapped_strings;
  initialize(mapped_strings);

  switch (mapped_strings[x]) {
  case values::all:
    info();
    TIME;
    break;
  case values::info:
    info();
    break;
  case values::print:
    print(tree);
    break;
  case values::time:
    TIME;
    break;
  default:
    cout << "\nUnknown command passed" << endl;
    break;
  }

#if defined(_OPENMP)
  cout << "________________________________________"
       << "________________________________________" << endl;
#endif
}

#endif
