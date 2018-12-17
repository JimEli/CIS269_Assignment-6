/*************************************************************************
* Title: Assignment 6 - Sorting Complexity
* File: sortTimer.h
* Author: James Eli
* Date: 11/16/2018
*
* Part 1: Measure times of sort algorithms (50 pts)
*   The provided code (sortTimer.cpp) has the STL sort algorithms sort 
*   (merge sort and quick sort) and sort_heap (heap sort) implemented in 
*   the code. Make a table (excel or in word) of the run times of the sorts 
*   for 1000 elements, 10,000 elements, 100,000 elements, 1,000,000 elements
*   and 10,000,000 elements. Record how long the sort takes for each of the 
*   instances.
*   1. Both algorithms should have O(n log n) performance, do they?
*   2. How do you know? (Answer these questions in your documentation)
* Part 2: Add another sort algorithm (25 pts)
*   Create your own sort algorithm (insert sort, bubble sort, selection sort 
*   or cocktail sort) and measure the run time of the algorithm using the 
*   same number as elements as you did for part 1. Answer the following 
*   questions:
*   1. What algorithm did you pick?
*   2. What is the complexity of the algorithm you picked?
*   3. Does the performance seem to match that performance?
* Extra Credit (5 points per algorithm)
*   Add addition sorting algorithms you created (you may also want to try 
*   some exotic sorts like bogosort and stoogesort). Measure the performance 
*   and calculate the complexity.
* Part 3: Ordered sort values (25 pts)
*   The quick sort has poor performance when the list is nearly ordered.
*   Instead of passing sort() a random list pass it an ordered list and see 
*   what the run time performance is compared to the other algorithms 
*   implemented in parts 1 and 2. 
*   1. Is the performance significantly different than before? 
*   2. Why or why not?
* What to turn in: Your documentation of the performance and questions 
* answered, and final C++ file. Due Date: Dec 6, 2018
*
* Notes:
*  (1) This driver code adapted from the provided, sortTimer.cpp file.
*  (2) Insertion, selection, comb, counting, and quicksort
*      adapted from the textbook "Data Structures and Algorithms in C++",
*      4th edition, by Drozdek.
*  (3) Uses C++17 parallel algorithm feature for sort().
*  (4) Benchmark function confirms result is sorted.
*  (5) Compiled for x86 with optimization set for speed: /O2
*  (6) For most accurate timing results, use Release build.
*  (7) Option to redirect output to file by defining: REDIRECT_TO_FILE.
*  (8) Compiled/tested with MS Visual Studio 2017 Community (v141), and
*      Windows SDK version 10.0.17134.0
*  (9) Compiled/tested with Eclipse Oxygen.3a Release (4.7.3a), using
*      CDT 9.4.3 and MinGw gcc-g++ (6.3.0-1).
*
* Submitted in partial fulfillment of the requirements of PCC CIS-269.
*************************************************************************
* Change Log:
*   11/29/2018: Add C++17 parallel algorithm sort. JME
*   11/16/2018: Initial release. JME
*************************************************************************/
#define _CRT_SECURE_NO_WARNINGS

#include <cstdio>     // freopen
#include <cstdlib>    // exit
#include <iostream>   // cout
#include <algorithm>  // sort, make_heap, sort_heap, swap, distance, minmax
#include <iostream>   // cout
#include <cstring>    // memset, memcpy
#include <chrono>     // chrono functions
#include <vector>     // used by counting sort
#include <random>     // mercenne twister prng
#include <exception>  // runtime exception
#if (_MSC_VER && __cpp_lib_parallel_algorithm < 201603)
#include <functional> // bind2nd
#include <ppl.h>      // concurrency 
#include <concrtrm.h> // concurrency
#endif
#if (__cpp_lib_parallel_algorithm >= 201603)
#include <execution>  // par_unseq
#endif

//#include "pch.h"
#include "sort_funcs.h" // sort and benchmark functions

// Uncomment to redirect output to file.
//#define REDIRECT_TO_FILE

// Min aand max values for benchmarking.
constexpr int MIN_ELEMENTS{ 1000 };
constexpr int MAX_ELEMENTS{ 10000000 };

int main(int argc, char** argv)
{
#ifndef REDIRECT_TO_FILE
	// input message
	std::cout << "Please enter the number of elements you want to inserted into an array: " << std::endl;
	int numOfElements = 0;
	std::cin >> numOfElements;
#else
	// Redircts stdout to a file and runs prescribed benchmark test.
	freopen("output.txt", "w", stdout);
	for (int numOfElements = MIN_ELEMENTS; numOfElements <= MAX_ELEMENTS; numOfElements *= 10)
	{
		std::cout << "n = " << numOfElements << std::endl;
#endif
		try {

			{
				std::cout << "STL sort() ";
				Sort<int, stlSort> stl;
				stl.benchmark(numOfElements, order::RANDOM);
			}

			{
				std::cout << "STL heap_sort() ";
				Sort<int, heapSort> heap;
				heap.benchmark(numOfElements, order::RANDOM);
			}

			//
			// Part 2: Add another sort algorithm.
			//
			{
				std::cout << "Insertion ";
				Sort<int, insertionSort> insertion;
				insertion.benchmark(numOfElements, order::RANDOM);
			}

			//
			// Extra Credit Add addition sorting algorithms.
			//
			{
				std::cout << "Comb ";
				Sort<int, combSort> comb;
				comb.benchmark(numOfElements, order::RANDOM);
			}

			{
				std::cout << "Counting ";
				Sort<int, countingSort> count;
				count.benchmark(numOfElements, order::RANDOM);
			}

			{
				std::cout << "Radix ";
				Sort<int, radixSort> radix;
				radix.benchmark(numOfElements, order::RANDOM);
			}

#if (_MSC_VER && __cpp_lib_parallel_algorithm < 201603)
			{
				std::cout << "PPL Parallel Quick ";
				Sort<int, pSort> psort;
				psort.benchmark(numOfElements, order::RANDOM);
			}

			{
				std::cout << "PPL Parallel Radix ";
				Sort<int, pRadix> pRadx;
				pRadx.benchmark(numOfElements, order::RANDOM);
			}
#endif
#if (__cpp_lib_parallel_algorithm >= 201603)
			{
				std::cout << "Parallel C++17 STL sort() ";
				Sort<int, pSort> psort;
				psort.benchmark(numOfElements, order::RANDOM);
			}
#endif
			{
				std::cout << "Quick ";
				Sort<int, quickSort> quick;
				quick.benchmark(numOfElements, order::RANDOM);
			}

			//
			// Part 3: Ordered sort values.
			//

			{
				std::cout << "Quick on an ordered array ";
				Sort<int, quickSort> quick;
				quick.benchmark(numOfElements, order::ORDERED);
			}

		}
		catch (std::exception& e)
		{
			// Report exception and exit with failure code.
			std::cout << "Encountered exception: " << e.what() << std::endl;
			std::exit(EXIT_FAILURE);
		}

#ifndef REDIRECT_TO_FILE
	// Stop
	std::cout << "You have reached the end of the program. Press any key..." << std::endl;
	std::cin.get();
#else
	}
	fclose(stdout);
#endif

	return 0;
}

