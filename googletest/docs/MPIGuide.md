This document provides a brief overview of the features added to googletest in order
to ensure its functionality in an MPI parallelized environment.

## Why do we need to extend the code for MPI? ##

We identify two scenarios that lead to problems with the basic googletest version.

1) Imagine the following block of Code

```
ASSERT_EQ (var, 0); // assert that the value of var is 0
MPI_Broadcast (...); // some collective MPI call

```

As long as the assertion is true or false on all processes, there is nothing to
be concerned about. However, suppose var == 0 only on some of the processes.
After calling ASSERT_EQ these processes will continue to call MPI_Broadcast.
The other processes will stop executing the code at the ASSERT_EQ statement and will not
call MPI_Broadcast. Thus only a subset of all processes calls a collective operation, resulting
in a deadlock.

2) A test may fail for one process but not for others. Nevertheless, the test should be seen as a failure.
This is particularly important if only one process is responsible for the output. This process must
know about the test failure.

## The new EXPECT/ASSERT macros ##

To adress issue 1) we provide the following new collective macros

EXPECT/ASSERT_EQ_MPI

EXPECT/ASSERT_NE_MPI

EXPECT/ASSERT_LE_MPI

EXPECT/ASSERT_LT_MPI

EXPECT/ASSERT_GE_MPI

EXPECT/ASSERT_GT_MPI

EXPECT/ASSERT_TRUE_MPI

EXPECT/ASSERT_FALSE_MPI

These macros work like their non _MPI counterpart with the addition that they
are MPI collective operations. In particular they need to be called by each process
in MPI_COMM_WORLD.
These macros guarantee to generate the same result on every process. Thus, if an assertion
fails on any process, the macro causes a failure on each process.

Example:

```
// Compute the rank of this process
int rank;
MPI_Comm_rank (MPI_COMM_WORLD, &rank);
// This assertion is true on process 0, but false
// on all other processes
ASSERT_EQ_MPI (rank, 0); // causes a failure, even on process 0
```

If we call this code with more than 1 process the ASSERT_EQ_MPI macro will cause
a failure on every process.

Whenever you need to ASSERT something and carry out parallel communication afterwards, 
you should use one of the ASSERT_*_MPI macros.

## Global Test result ##

To adress issue 2) the different results of a test on the different processes
are communicated at the end of a test.
If there was a failure on any process then the test will evaluate as a failure 
on all processes.