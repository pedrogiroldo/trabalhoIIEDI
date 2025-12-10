/**
 * @file sorting.spec.c
 * @brief Unit tests for sorting module
 */

#include "sorting.h"
#include "../../test_framework/test_framework.h"

// ============================================================================
// Helper comparison function
// ============================================================================

static int compare_int(const void *a, const void *b) {
  int ia = *(const int *)a;
  int ib = *(const int *)b;
  return (ia > ib) - (ia < ib);
}

static int compare_int_desc(const void *a, const void *b) {
  int ia = *(const int *)a;
  int ib = *(const int *)b;
  return (ib > ia) - (ib < ia);
}

// ============================================================================
// Tests for InsertionSort
// ============================================================================

bool test_insertionsort_empty_array(void) {
  int arr[] = {};
  sorting_insertionsort(arr, 0, sizeof(int), compare_int);
  return true; // Should not crash
}

bool test_insertionsort_single_element(void) {
  int arr[] = {42};
  sorting_insertionsort(arr, 1, sizeof(int), compare_int);
  ASSERT_EQUAL(arr[0], 42);
  return true;
}

bool test_insertionsort_sorted_array(void) {
  int arr[] = {1, 2, 3, 4, 5};
  sorting_insertionsort(arr, 5, sizeof(int), compare_int);
  ASSERT_EQUAL(arr[0], 1);
  ASSERT_EQUAL(arr[1], 2);
  ASSERT_EQUAL(arr[2], 3);
  ASSERT_EQUAL(arr[3], 4);
  ASSERT_EQUAL(arr[4], 5);
  return true;
}

bool test_insertionsort_reverse_array(void) {
  int arr[] = {5, 4, 3, 2, 1};
  sorting_insertionsort(arr, 5, sizeof(int), compare_int);
  ASSERT_EQUAL(arr[0], 1);
  ASSERT_EQUAL(arr[1], 2);
  ASSERT_EQUAL(arr[2], 3);
  ASSERT_EQUAL(arr[3], 4);
  ASSERT_EQUAL(arr[4], 5);
  return true;
}

bool test_insertionsort_random_array(void) {
  int arr[] = {3, 1, 4, 1, 5, 9, 2, 6};
  sorting_insertionsort(arr, 8, sizeof(int), compare_int);
  ASSERT_EQUAL(arr[0], 1);
  ASSERT_EQUAL(arr[1], 1);
  ASSERT_EQUAL(arr[2], 2);
  ASSERT_EQUAL(arr[3], 3);
  ASSERT_EQUAL(arr[4], 4);
  ASSERT_EQUAL(arr[5], 5);
  ASSERT_EQUAL(arr[6], 6);
  ASSERT_EQUAL(arr[7], 9);
  return true;
}

// ============================================================================
// Tests for MergeSort
// ============================================================================

bool test_mergesort_empty_array(void) {
  int arr[] = {};
  sorting_mergesort(arr, 0, sizeof(int), compare_int, 10);
  return true; // Should not crash
}

bool test_mergesort_single_element(void) {
  int arr[] = {42};
  sorting_mergesort(arr, 1, sizeof(int), compare_int, 10);
  ASSERT_EQUAL(arr[0], 42);
  return true;
}

bool test_mergesort_sorted_array(void) {
  int arr[] = {1, 2, 3, 4, 5};
  sorting_mergesort(arr, 5, sizeof(int), compare_int, 10);
  ASSERT_EQUAL(arr[0], 1);
  ASSERT_EQUAL(arr[1], 2);
  ASSERT_EQUAL(arr[2], 3);
  ASSERT_EQUAL(arr[3], 4);
  ASSERT_EQUAL(arr[4], 5);
  return true;
}

bool test_mergesort_reverse_array(void) {
  int arr[] = {5, 4, 3, 2, 1};
  sorting_mergesort(arr, 5, sizeof(int), compare_int, 10);
  ASSERT_EQUAL(arr[0], 1);
  ASSERT_EQUAL(arr[1], 2);
  ASSERT_EQUAL(arr[2], 3);
  ASSERT_EQUAL(arr[3], 4);
  ASSERT_EQUAL(arr[4], 5);
  return true;
}

bool test_mergesort_large_array(void) {
  int arr[100];
  for (int i = 0; i < 100; i++) {
    arr[i] = 100 - i;
  }
  sorting_mergesort(arr, 100, sizeof(int), compare_int, 10);
  for (int i = 0; i < 100; i++) {
    ASSERT_EQUAL(arr[i], i + 1);
  }
  return true;
}

bool test_mergesort_with_threshold_1(void) {
  int arr[] = {5, 4, 3, 2, 1, 10, 9, 8, 7, 6};
  sorting_mergesort(arr, 10, sizeof(int), compare_int, 1);
  for (int i = 0; i < 10; i++) {
    ASSERT_EQUAL(arr[i], i + 1);
  }
  return true;
}

bool test_mergesort_with_threshold_5(void) {
  int arr[] = {5, 4, 3, 2, 1, 10, 9, 8, 7, 6};
  sorting_mergesort(arr, 10, sizeof(int), compare_int, 5);
  for (int i = 0; i < 10; i++) {
    ASSERT_EQUAL(arr[i], i + 1);
  }
  return true;
}

// ============================================================================
// Tests for unified sort interface
// ============================================================================

bool test_sorting_sort_qsort(void) {
  int arr[] = {5, 4, 3, 2, 1};
  sorting_sort(arr, 5, sizeof(int), compare_int, SORT_QSORT, 10);
  ASSERT_EQUAL(arr[0], 1);
  ASSERT_EQUAL(arr[1], 2);
  ASSERT_EQUAL(arr[2], 3);
  ASSERT_EQUAL(arr[3], 4);
  ASSERT_EQUAL(arr[4], 5);
  return true;
}

bool test_sorting_sort_mergesort(void) {
  int arr[] = {5, 4, 3, 2, 1};
  sorting_sort(arr, 5, sizeof(int), compare_int, SORT_MERGESORT, 10);
  ASSERT_EQUAL(arr[0], 1);
  ASSERT_EQUAL(arr[1], 2);
  ASSERT_EQUAL(arr[2], 3);
  ASSERT_EQUAL(arr[3], 4);
  ASSERT_EQUAL(arr[4], 5);
  return true;
}

bool test_sorting_descending(void) {
  int arr[] = {1, 2, 3, 4, 5};
  sorting_sort(arr, 5, sizeof(int), compare_int_desc, SORT_MERGESORT, 10);
  ASSERT_EQUAL(arr[0], 5);
  ASSERT_EQUAL(arr[1], 4);
  ASSERT_EQUAL(arr[2], 3);
  ASSERT_EQUAL(arr[3], 2);
  ASSERT_EQUAL(arr[4], 1);
  return true;
}

// ============================================================================
// Main
// ============================================================================

int main(void) {
  test_framework_init();

  test_print_section("InsertionSort Tests");
  test_register("insertionsort_empty_array", test_insertionsort_empty_array);
  test_register("insertionsort_single_element",
                test_insertionsort_single_element);
  test_register("insertionsort_sorted_array", test_insertionsort_sorted_array);
  test_register("insertionsort_reverse_array",
                test_insertionsort_reverse_array);
  test_register("insertionsort_random_array", test_insertionsort_random_array);

  test_print_section("MergeSort Tests");
  test_register("mergesort_empty_array", test_mergesort_empty_array);
  test_register("mergesort_single_element", test_mergesort_single_element);
  test_register("mergesort_sorted_array", test_mergesort_sorted_array);
  test_register("mergesort_reverse_array", test_mergesort_reverse_array);
  test_register("mergesort_large_array", test_mergesort_large_array);
  test_register("mergesort_with_threshold_1", test_mergesort_with_threshold_1);
  test_register("mergesort_with_threshold_5", test_mergesort_with_threshold_5);

  test_print_section("Unified Sort Interface Tests");
  test_register("sorting_sort_qsort", test_sorting_sort_qsort);
  test_register("sorting_sort_mergesort", test_sorting_sort_mergesort);
  test_register("sorting_descending", test_sorting_descending);

  int result = test_run_all();
  test_framework_cleanup();
  return result;
}
