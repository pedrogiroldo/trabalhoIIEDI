/**
 * @file sorting.h
 * @brief Sorting algorithms module
 *
 * This module provides sorting algorithms including MergeSort with
 * InsertionSort optimization for small subarrays, and a unified interface
 * for selecting between qsort and custom mergesort.
 */

#ifndef SORTING_H
#define SORTING_H

#include <stddef.h>

/**
 * @brief Comparison function type (same signature as qsort)
 * @param a First element
 * @param b Second element
 * @return Negative if a < b, 0 if a == b, positive if a > b
 */
typedef int (*SortCompareFunc)(const void *a, const void *b);

/**
 * @brief Sorting algorithm type
 */
typedef enum {
  SORT_QSORT,    /**< Use standard library qsort */
  SORT_MERGESORT /**< Use custom mergesort with insertionsort optimization */
} SortType;

/**
 * @brief Sorts an array using InsertionSort algorithm
 *
 * Efficient for small arrays (typically < 15 elements).
 *
 * @param base Pointer to the first element of the array
 * @param nmemb Number of elements in the array
 * @param size Size of each element in bytes
 * @param compar Comparison function
 */
void sorting_insertionsort(void *base, size_t nmemb, size_t size,
                           SortCompareFunc compar);

/**
 * @brief Sorts an array using MergeSort algorithm with InsertionSort
 * optimization
 *
 * Uses InsertionSort for subarrays smaller than the threshold.
 *
 * @param base Pointer to the first element of the array
 * @param nmemb Number of elements in the array
 * @param size Size of each element in bytes
 * @param compar Comparison function
 * @param threshold Threshold for switching to InsertionSort (default: 10)
 */
void sorting_mergesort(void *base, size_t nmemb, size_t size,
                       SortCompareFunc compar, int threshold);

/**
 * @brief Unified sorting interface - selects algorithm based on type
 *
 * @param base Pointer to the first element of the array
 * @param nmemb Number of elements in the array
 * @param size Size of each element in bytes
 * @param compar Comparison function
 * @param sort_type Type of sorting algorithm to use
 * @param threshold InsertionSort threshold (only used for SORT_MERGESORT)
 */
void sorting_sort(void *base, size_t nmemb, size_t size, SortCompareFunc compar,
                  SortType sort_type, int threshold);

#endif // SORTING_H
