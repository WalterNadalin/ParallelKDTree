# SecondAssignment
*Second Foundation of High Performance Computing assignment - University of Trieste - Academic Year 2021/2022*

## Build a $k$-d tree for 2-dimensional data
$k$-d trees are a data structures presented originally by Friedman, Bentley and Finkel in 1977 1 to represent a set of $k$-dimensional data in order to make them efficiently searchable.

In spite of its age, or more likely thanks to it and to the large amount of research and improvements accumulated in time, $k$-d trees are still a good pragmatical choice to perform $k$nn ($k$-nearest neighbours) operations in many cases.

In this assignment you are required to write a parallel code that builds a $k$-d tree for $k=2$. **You must
implement both the MPI and the OpenMP version**.
In order to simplify the task, the following 2 assumptions hold:

  1. the data set, and hence the related kd-tree, can be assumed immutable, i.e. you can neglect the insertion/deletion operations;
  2. the data points can be assumed to be homogeneously distributed in all the k dimensions.
