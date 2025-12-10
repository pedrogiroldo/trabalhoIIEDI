/**
 * @file sorting.c
 * @brief Implementation of sorting algorithms
 *
 * Implements MergeSort with InsertionSort optimization for small subarrays.
 */

#include "sorting.h"
#include <stdlib.h>
#include <string.h>

/**
 * Swaps two elements in memory
 */
static void swap_elements(void *a, void *b, size_t size) {
  unsigned char *pa = (unsigned char *)a;
  unsigned char *pb = (unsigned char *)b;

  for (size_t i = 0; i < size; i++) {
    unsigned char tmp = pa[i];
    pa[i] = pb[i];
    pb[i] = tmp;
  }
}

/**
 * Gets pointer to element at index
 */
static void *get_element(void *base, size_t index, size_t size) {
  return (unsigned char *)base + (index * size);
}

void sorting_insertionsort(void *base, size_t nmemb, size_t size,
                           SortCompareFunc compar) {
  if (nmemb <= 1) {
    return;
  }

  unsigned char *arr = (unsigned char *)base;
  unsigned char *key = malloc(size);
  if (!key) {
    return;
  }

  for (size_t i = 1; i < nmemb; i++) {
    memcpy(key, arr + i * size, size);

    size_t j = i;
    while (j > 0 && compar(arr + (j - 1) * size, key) > 0) {
      memcpy(arr + j * size, arr + (j - 1) * size, size);
      j--;
    }

    memcpy(arr + j * size, key, size);
  }

  free(key);
}

/**
 * Merges two sorted subarrays into one
 */
static void merge(void *base, size_t left, size_t mid, size_t right,
                  size_t size, SortCompareFunc compar, void *temp) {
  unsigned char *arr = (unsigned char *)base;
  unsigned char *tmp = (unsigned char *)temp;

  size_t left_size = mid - left + 1;
  size_t right_size = right - mid;

  // Copy data to temporary arrays
  memcpy(tmp, arr + left * size, left_size * size);
  memcpy(tmp + left_size * size, arr + (mid + 1) * size, right_size * size);

  size_t i = 0;    // Index for left subarray
  size_t j = 0;    // Index for right subarray
  size_t k = left; // Index for merged array

  unsigned char *left_arr = tmp;
  unsigned char *right_arr = tmp + left_size * size;

  while (i < left_size && j < right_size) {
    if (compar(left_arr + i * size, right_arr + j * size) <= 0) {
      memcpy(arr + k * size, left_arr + i * size, size);
      i++;
    } else {
      memcpy(arr + k * size, right_arr + j * size, size);
      j++;
    }
    k++;
  }

  // Copy remaining elements from left subarray
  while (i < left_size) {
    memcpy(arr + k * size, left_arr + i * size, size);
    i++;
    k++;
  }

  // Copy remaining elements from right subarray
  while (j < right_size) {
    memcpy(arr + k * size, right_arr + j * size, size);
    j++;
    k++;
  }
}

/**
 * Recursive MergeSort with InsertionSort optimization
 */
static void mergesort_recursive(void *base, size_t left, size_t right,
                                size_t size, SortCompareFunc compar,
                                int threshold, void *temp) {
  if (left >= right) {
    return;
  }

  size_t subarray_size = right - left + 1;

  // Use InsertionSort for small subarrays
  if (subarray_size <= (size_t)threshold) {
    sorting_insertionsort(get_element(base, left, size), subarray_size, size,
                          compar);
    return;
  }

  size_t mid = left + (right - left) / 2;

  mergesort_recursive(base, left, mid, size, compar, threshold, temp);
  mergesort_recursive(base, mid + 1, right, size, compar, threshold, temp);

  merge(base, left, mid, right, size, compar, temp);
}

void sorting_mergesort(void *base, size_t nmemb, size_t size,
                       SortCompareFunc compar, int threshold) {
  if (nmemb <= 1) {
    return;
  }

  if (threshold <= 0) {
    threshold = 10; // Default threshold
  }

  // Allocate temporary buffer for merging
  void *temp = malloc(nmemb * size);
  if (!temp) {
    return;
  }

  mergesort_recursive(base, 0, nmemb - 1, size, compar, threshold, temp);

  free(temp);
}

void sorting_sort(void *base, size_t nmemb, size_t size, SortCompareFunc compar,
                  SortType sort_type, int threshold) {
  if (nmemb <= 1) {
    return;
  }

  switch (sort_type) {
  case SORT_QSORT:
    qsort(base, nmemb, size, compar);
    break;

  case SORT_MERGESORT:
    sorting_mergesort(base, nmemb, size, compar, threshold);
    break;

  default:
    // Fallback to qsort
    qsort(base, nmemb, size, compar);
    break;
  }
}
