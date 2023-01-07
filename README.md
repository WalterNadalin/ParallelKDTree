# Parallel *k*-d tree
This is an education project. Details follow.

## Goal: build a *k*-d tree for 2-dimensional data
**Remark**: read `report.pdf` for a detailed explanation of the project and the results.

*k*-d trees are a data structures presented originally by Friedman, Bentley and Finkel in 1977 to represent a set of *k*-dimensional data in order to make them efficiently searchable.

In spite of its age, or more likely thanks to it and to the large amount of research and improvements accumulated in time, *k*-d trees are still a good pragmatical choice to perform knn (*k*-nearest neighbours) operations in many cases.

In this assignment you are required to write a parallel code that builds a *k*-d tree for *k=2*. **You must
implement both the MPI and the OpenMP version**.  Alternatively, you could opt for a single hybrid MPI-OpenMP version.

In order to simplify the task, the following 2 assumptions hold:

  1. the data set, and hence the related *k*-d tree, can be assumed immutable, i.e. you can neglect the insertion/deletion operations;
  2. the data points can be assumed to be homogeneously distributed in all the *k* dimensions.

More info at https://github.com/WalterNadalin/Foundations_of_HPC_2021/tree/main/Assignment2.
