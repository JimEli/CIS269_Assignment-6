# Selected Sort Complexity
## CIS269 Assignment 6

### Part 1: Measure times of sort algorithms. 
The provided code (sortTimer.cpp) has the STL sort algorithms sort  (merge sort and quick sort) and sort_heap (heap sort) implemented in the code. Make a table (excel or in word) of the run times of the sorts for 1000 elements, 10,000 elements, 100,000 elements, 1,000,000 elements and 10,000,000 elements. Record how long the sort takes for each of the instances.
  1. Both algorithms should have O(n log n) performance, do they?
  2. How do you know? (Answer these questions in your documentation).

### Part 2: Add another sort algorithm. 
Create your own sort algorithm (insert sort, bubble sort, selection sort or cocktail sort) and measure the run time of the algorithm using the same number as elements as you did for part 1. Answer the following questions:
  1. What algorithm did you pick?
  2. What is the complexity of the algorithm you picked?
  3. Does the performance seem to match that performance?

Extra Credit. Add addition sorting algorithms you created (you may also want to try some exotic sorts like bogosort and stoogesort). Measure the performance and calculate the complexity.

### Part 3: Ordered sort values. 
The quick sort has poor performance when the list is nearly ordered. Instead of passing sort() a random list pass it an ordered list and see what the run time performance is compared to the other algorithms implemented in parts 1 and 2. 
  1. Is the performance significantly different than before? 
  2. Why or why not?

Notes:
* Driver code adapted from the provided "sortTimer.cpp" file.
* Insertion, selection, comb, counting, and quicksort adapted from the textbook "Data Structures and Algorithms in C++", 4th edition, by Drozdek.
* Program uses C++17 parallel algorithm feature for sort(), and/or MSVC parallel patterns library.
* Benchmark function confirms result is sorted.
* Was compiled for x86 with optimization set for speed: /O2
* For most accurate timing results, use a Release build.
* Option to redirect output to file by defining: REDIRECT_TO_FILE.
* Compiled/tested with MS Visual Studio 2017 Community (v141), and Windows SDK version 10.0.17134.0
* Compiled/tested with Eclipse Oxygen.3a Release (4.7.3a), using CDT 9.4.3 and MinGw gcc-g++ (6.3.0-1).
* Submitted in partial fulfillment of the requirements of PCC CIS-269.
