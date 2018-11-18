The goal of the programs of this repository---initially implemented for the lecture System concepts and programming at University of Stuttgart---is to demonstrate the impact of different memory order models onto the semantics of concurrent programs.

As example, Dekker's algorithm for synchronizing two threads is used. Note that Dekker's algorithm might not be correct under all memory order models! It is part of the exercise to find out, which memory order models ensure a correct execution of Dekker's algorithm.

Each implementation (using a different memory order model) is a single file, which can be compiled as follows:

```
$ clang++ dekker-mem_order_relaxed.cc -lpthread -std=c++11 -O0
```
