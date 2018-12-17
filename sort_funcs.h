/*************************************************************************
* Title: Benchmark Sort Functions.
* File: sort_funcs.h
* Author: James Eli
* Date: 11/16/2018
*
* Collection of several sort functions:
*   STL sort (stlSort)
*   STL sort using parallel algorithm (stlpSort)
*   STL heap sort (heapSort)
*   insertion sort (insertionSort)
*   comb sort (combSort)
*   counting sort (countingSort)
*   quick sort (quickSort)
*   radix sort (radixSort)
*   parallel radix sort (pRadix)
*   parallel quicksort (psort)
*
* Notes:
*  (1) Insertion, comb, counting and quicksort adapted from the textbook 
*      "Data Structures and Algorithms in C++", 4th edition, by Drozdek.
*  (2) Uses C++17 parallel algorithm feature for sort(), otherwise 
*      substitutes a concurrent MSVC++ Parallel Patterns Library version.
*  (3) For most accurate timing results, use Release build.
*  (4) Compiled/tested with MS Visual Studio 2017 Community (v141), and
*      Windows SDK version 10.0.17134.0
*  (5) Compiled/tested with Eclipse Oxygen.3a Release (4.7.3a), using
*      CDT 9.4.3 and MinGw gcc-g++ (6.3.0-1).
*
* Submitted in partial fulfillment of the requirements of PCC CIS-269.
*************************************************************************
* Change Log:
*   11/29/2018: Add C++17 parallel algorithm sort. JME
*   11/16/2018: Initial release. JME
*************************************************************************/
#ifndef _SORT_FUNCTIONS_H_
#define _SORT_FUNCTIONS_H_

// Embedded timer.
using namespace std::literals::chrono_literals;

// Integral sort function prototypes.
template <typename T> void stlSort(T*, T*);       // STL sort algorithm.
template <typename T> void heapSort(T*, T*);      // STL heap sort algorithm.
template <typename T> void insertionSort(T*, T*); // My insertion sort.
template <typename T> void combSort(T*, T*);      // My comb sort algorithm.
template <typename T> void countingSort(T*, T*);  // My counting sort algorithm.
template <typename T> void quickSort(T*, T*);     // My quicksort algorithm.
template <typename T> void radixSort(T*, T*);     // My optimized radix sort (based on EASTL) algorith.
template <typename T> void pSort(T*, T*);         // PPL or STL C++17 parallel sort algorithm.
#if (_MSC_VER && __cpp_lib_parallel_algorithm < 201603)
inline float LogN(float);                         // Parallel quicksort helper function.
template <typename T> void pRadix(T*, T*);        // PPL parallel radix sort.
#endif

// Defines element order of benchmark array.
enum order { ORDERED, REVERSED, MOSTLY_ORDERED, RANDOM };

// Sort class with stl library sort algorithm set as default.
template <typename T, typename void(*S)(T*, T*) = stlSort>
struct Sort 
{
	// This functor allows calling sort without benchmarking.
	void operator() (T* start, T* last) { S(start, last); }
	
	// Benchmark the sort.
	void benchmark(int n, order type)  
	{
		if (n) 
		{
			// Create a filled array of size n.
			T* bmArray = constructTestArray(n, type);

			if (bmArray)
			{
				// Time sort function and report the duration.
				std::chrono::time_point<std::chrono::steady_clock> start= std::chrono::high_resolution_clock::now(); 
				S(bmArray, bmArray + n);
				std::chrono::duration<float> duration = std::chrono::high_resolution_clock::now() - start;
				std::cout << "elapsed time: " << duration.count()*1000.0f << "ms" << std::endl;

				// Confirm the sort worked.
				if (!std::is_sorted(bmArray, bmArray + n))
				{
					delete[] bmArray;
					throw std::runtime_error("sort failed");
				}

				delete[] bmArray;
			}
		}
	}

private:
	// Constructs a filled dynamic array of size n for benchmark test.
	T* constructTestArray(int n, order type)
	{
		T* a = new T[n];

		// Select order of array elements.
		switch (type)
		{
			case order::ORDERED:
			{
				// Sequentialize.  // std::iota(a, a+n, 1);
				std::generate(a, a + n, [i = 1]() mutable { return i++; });

				break;
			}

			case order::REVERSED:
			{
				// Sequentialize in reverse.
				std::generate(a, a + n, [i = n]() mutable { return i--; });
				//std::generate(a, a + n, [i = 1]() mutable { return i++; });
				//std::reverse(a, a + n);

				break;
			}

			case order::MOSTLY_ORDERED:
			{
				// Sequential.
				std::generate(a, a + n, [i = 1]() mutable { return i++; });

				// Ordered percent will actually be about 82-85%.
				const std::size_t percent = 80;

				// Random number distribution [0, n] [inclusive, inclusive].
				std::uniform_int_distribution<T> dist(0, n);
				std::random_device rd;
				// Non-deterministic 32-bit seed.
				std::mt19937 mt(rd());

				// Apply some order to array.
				for (std::size_t i = 0, s = n, end = ((s < (100 - percent)) ? 1 : (s / (100 - percent))); i < end; i++)
				{
					std::size_t j = dist(mt);
					std::size_t k = dist(mt);
					std::swap(a[j], a[k]);
				}

				break;
			}

			case order::RANDOM:
			default:
			{
				// Random number distribution [0, n] [inclusive, inclusive].
				std::uniform_int_distribution<T> dist(0, n);
				std::random_device rd;
				// Non-deterministic 32-bit seed.
				std::mt19937 mt(rd());

				// Fill with random numbers.
				std::generate(a, a + n, [&]() { return dist(mt); });

				break;
			}
		}

		return a;
	}
};

/*************************************************************************
 * C++ STL <algorithm> library sort function. sort() uses an introsort
 * (quicksort which switches to heapsort when the recursion reaches a
 * certain depth).
*************************************************************************/
template<typename T>
static void stlSort(T* begin, T* end) { std::sort(begin, end); }

/*************************************************************************
 * C++ STL <algorithm> library heap sort function.
*************************************************************************/
template<typename T>
static void heapSort(T* begin, T* end)
{
	std::make_heap(begin, end);
	std::sort_heap(begin, end);
}

/*************************************************************************
 * Implements the insertion sort algorithm.
 *************************************************************************/
template<class T>
static void insertionSort(T* begin, T* end)
{
	for (auto i = begin; i != end; i++)
	{
		for (auto j = i; j != begin; j--)
		{
			if (*j < *(j - 1))
				std::swap(*j, *(j - 1));
			else
				break;
		}
	}
}

/*************************************************************************
 * Implements a comb sort algorithm.
 *************************************************************************/
template <typename T>
static void combSort(T* begin, T* end)
{
	T *current, *next;
	int length = static_cast<int>(std::distance(begin, end)), space = length;

	for (bool swapped = false; (space > 1) || swapped;)
	{
		space = ((space*10) + 3)/13;

		if ((space == 9) || (space == 10))
			space = 11;

		current = next = begin;
		next += space;

		for (swapped = false; next != end; current++, next++)
			if (*next < *current)
			{
				std::swap(*current, *next);
				swapped = true;
			}
	}
}

/*************************************************************************
 * Implements a counting sort algorithm.
 *************************************************************************/
template<typename T>
void countingSort(T* begin, T* end)
{
#if (__cpp_lib_parallel_algorithm >= 201603)
	// Use parallel algorithm.
	T min = *std::min_element(std::execution::par_unseq, begin, end);
	T max = *std::max_element(std::execution::par_unseq, begin, end);
#else
	// Use standard algorithm.
	T min = *std::min_element(begin, end);
	T max = *std::max_element(begin, end);
#endif
	std::vector<unsigned> count((max - min) + 1, 0);

	for (T* i = begin; i != end; ++i)
		++count[*i - min];

	T* start = begin;

	for (T i = min; i <= max; ++i)
		for (unsigned j = 0; j < count[i - min]; ++j)
			*start++ = i;
}

/*************************************************************************
 * Implements a basic quick sort algorithm.
 *************************************************************************/
template<class T>
void quick_sort(T* begin, int first, int last)
{
	int lower = first + 1, upper = last;

	//std::swap(*(begin + first), *(begin + (first + last)/2));
	T bound = *(begin + first);

	while (lower <= upper)
	{
		while (*(begin + lower) < bound)
			lower++;

		while (bound < *(begin + upper))
			upper--;

		if (lower < upper)
		{
			std::swap(*(begin + lower), *(begin + upper));
			upper--;
		}

		lower++;
	}

	std::swap(*(begin + upper), *(begin + first));

	if (first < upper - 1)
		quick_sort(begin, first, upper - 1);

	if (upper + 1 < last)
		quick_sort(begin, upper + 1, last);
}

template<class T>
void quickSort(T* begin, T* end)
{
	const int n = static_cast<int>(std::distance(begin, end));

	if (n < 2)
		return;

	int max = 0;

	// Find largest element and put it at the end.
	for (int i = 1; i < n; i++)
		if (*(begin + max) < *(begin + i))
			max = i;

	// Largest element is now in its final position.
	std::swap(*(begin + n - 1), *(begin + max));
	quick_sort(begin, 0, n - 2);
}

#if (_MSC_VER && __cpp_lib_parallel_algorithm < 201603)
/*************************************************************************
 Concurrent MSVC++ Parallel Patterns Library radix sort.
 *************************************************************************/
template <typename T>
void pRadix(T* begin, T* end) { Concurrency::parallel_radixsort(begin, end); }

/*************************************************************************
 * Implements a concurrent MSVC++ Parallel Patterns Library quicksort.
 *************************************************************************/
template <typename T>
void ParallelQuickSortWithSTL(T* begin, T* end, int depthRemaining, long threshold)
{
	if (std::distance(begin, end) <= threshold)
		std::sort(begin, end);

	else
	{
		T* pivot = std::partition(begin, end, std::bind2nd(std::less<T>(), *begin));

		if (depthRemaining > 0)
		{
			Concurrency::parallel_invoke(
				[begin, end, pivot, depthRemaining, threshold] {
				if (pivot != end)
					ParallelQuickSortWithSTL(begin, pivot, depthRemaining - 1, threshold);
			},
				[&pivot, begin, end, depthRemaining, threshold] {
				if (pivot == begin)
					++pivot;
				ParallelQuickSortWithSTL(pivot, end, depthRemaining - 1, threshold);
			});
		}

		else
		{
			T* pivot = std::partition(begin, end, std::bind2nd(std::less<T>(), *begin));

			if (pivot != end)
				std::sort(begin, pivot);

			if (pivot == begin)
				++pivot;
			
			std::sort(pivot, end);
		}
	}
}

template <typename T>
void pSort(T* begin, T* end)
{
	const int maxTasks = Concurrency::CurrentScheduler::Get()->GetNumberOfVirtualProcessors();
	//ParallelQuickSort(begin, end, (int)LogN(float(maxTasks), 2.0f) + 4, 256);
	ParallelQuickSortWithSTL(begin, end, (int)LogN((float)maxTasks) + 4, 256);
}

// To generate logarithms for base 2. Log base b of a == natural log (a) / natural log (b).
inline float LogN(float value) { return logf(value) / logf(2.0f); }
#endif

#if (__cpp_lib_parallel_algorithm >= 201603)
/*************************************************************************
 * C++ STL <algorithm> library sort function with C++17 parallel option.
 * Defaults to sequential version if C++17 features not available.
 *************************************************************************/
template <typename T>
// Use parallel algorithm.
void pSort(T* begin, T* end) { std::sort(std::execution::par_unseq, begin, end); }
#endif

/*************************************************************************
 * Implements a radix sort algorithm.
*************************************************************************/
// Implements a classic LSD (least significant digit) radix sort.
// See http://en.wikipedia.org/wiki/Radix_sort.
// To consider: A static linked-list implementation may be faster than the version here.
// The radix_sort implementation uses two optimizations that are not part of a typical radix sort implementation.
// 1. Computing a histogram (i.e. finding the number of elements per bucket) for the next pass is done in parallel with the loop that "scatters"
//    elements in the current pass.  The advantage is that it avoids the memory traffic / cache pressure of reading keys in a separate operation.
//    Note: It would also be possible to compute all histograms in a single pass.  However, that would increase the amount of stack space used and
//    also increase cache pressure slightly.  However, it could still be faster under some situations.
// 2. If all elements are mapped to a single bucket, then there is no need to perform a scatter operation.  Instead the elements are left in place
//    and only copied if they need to be copied to the final output buffer.
template <typename T>
void radixSort(T* begin, T* end)
{
	T* temp, *first = begin;
	T* buffer = new T[std::distance(begin, end)], *pBuffer = buffer;
	constexpr int DIGIT_BITS = 8;                   // Bits per digit.
	constexpr size_t NUM_BUCKETS = 1 << DIGIT_BITS; // Number of buckets.
	constexpr T BUCKET_MASK = NUM_BUCKETS - 1;      // Mask all bits.
	uint32_t bucketSize[NUM_BUCKETS];
	uint32_t bucketPos[NUM_BUCKETS];
	bool doSeparateHistogramCalculation = true;

	for (uint32_t j = 0; j < (8 * sizeof(T)); j += DIGIT_BITS)
	{
		if (doSeparateHistogramCalculation)
		{
			memset(bucketSize, 0, sizeof(bucketSize));
			// Calculate histogram for the first scatter operation
			for (temp = first; temp != end; ++temp)
				++bucketSize[(*temp >> j) & BUCKET_MASK];
		}

		// If a single bucket contains all of the elements, then don't 
		// bother redistributing all elements to the same bucket.
		if (bucketSize[((*first >> j) & BUCKET_MASK)] == uint32_t(end - first))
		{
			// Set flag to ensure histogram is computed for next digit position.
			doSeparateHistogramCalculation = true;
		}
		else
		{
			// The histogram is either not needed or it will be calculated in parallel 
			// with the scatter operation below for better cache efficiency.
			doSeparateHistogramCalculation = false;

			// If this is the last digit position, then don't calculate a histogram
			if (j == (8 * sizeof(T) - DIGIT_BITS))
			{
				bucketPos[0] = 0;

				for (uint32_t i = 0; i < NUM_BUCKETS - 1; i++)
					bucketPos[i + 1] = bucketPos[i] + bucketSize[i];

				for (temp = first; temp != end; ++temp)
				{
					const size_t digit = (*temp >> j) & BUCKET_MASK;
					*(pBuffer + bucketPos[digit]++) = *temp;
				}
			}
			else
			{
				bucketPos[0] = 0;

				for (uint32_t i = 0; i < NUM_BUCKETS - 1; i++)
				{
					bucketPos[i + 1] = bucketPos[i] + bucketSize[i];
					bucketSize[i] = 0;
				}

				uint32_t next = j + DIGIT_BITS;

				for (temp = first; temp != end; ++temp)
				{
					const size_t digit = (*temp >> j) & BUCKET_MASK;
					*(pBuffer + bucketPos[digit]++) = *temp;
					++bucketSize[(*temp >> next) & BUCKET_MASK];
				}
			}

			end = pBuffer + (end - first);
			temp = first;
			first = pBuffer;
			pBuffer = temp;
		}
	}

	// Copy buffer to original array.
	if (first != begin)
		for (temp = first; temp != end; ++temp)
			*pBuffer++ = *temp;

	delete[] buffer;
}
#endif

