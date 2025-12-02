#include "bst.h"
#include "../../test_framework/test_framework.h"
#include <stdbool.h> // Added for 'bool' type
#include <stdio.h>
#include <stdlib.h>

// ============================================================================
// Test Helpers
// ============================================================================

// Simple integer comparison function for testing
int compare_ints(void *a, void *b, void *context) {
  int val_a = *(int *)a;
  int val_b = *(int *)b;

  if (val_a < val_b)
    return -1;
  if (val_a > val_b)
    return 1;
  return 0;
}

// ============================================================================
// Test Cases
// ============================================================================

bool test_bst_create() {
  BST tree = bst_create(compare_ints, NULL);

  ASSERT_NOT_NULL(tree);
  ASSERT_TRUE(bst_is_empty(tree));
  ASSERT_EQUAL(bst_size(tree), 0);

  bst_destroy(tree, NULL);
  return true;
}

bool test_bst_insert_single() {
  BST tree = bst_create(compare_ints, NULL);
  int value = 42;

  BSTNode node = bst_insert(tree, &value);

  ASSERT_NOT_NULL(node);
  ASSERT_FALSE(bst_is_empty(tree));
  ASSERT_EQUAL(bst_size(tree), 1);

  bst_destroy(tree, NULL);
  return true;
}

bool test_bst_insert_multiple() {
  BST tree = bst_create(compare_ints, NULL);
  int values[] = {50, 30, 70, 20, 40, 60, 80};
  int count = 7;

  for (int i = 0; i < count; i++) {
    BSTNode node = bst_insert(tree, &values[i]);
    ASSERT_NOT_NULL(node);
  }

  ASSERT_EQUAL(bst_size(tree), count);
  ASSERT_FALSE(bst_is_empty(tree));

  bst_destroy(tree, NULL);
  return true;
}

bool test_bst_find_min() {
  BST tree = bst_create(compare_ints, NULL);
  int values[] = {50, 30, 70, 20, 40, 60, 80};
  int count = 7;

  for (int i = 0; i < count; i++) {
    bst_insert(tree, &values[i]);
  }

  int *min = (int *)bst_find_min(tree);
  ASSERT_NOT_NULL(min);
  ASSERT_EQUAL(*min, 20);

  bst_destroy(tree, NULL);
  return true;
}

bool test_bst_find_min_empty() {
  BST tree = bst_create(compare_ints, NULL);

  void *min = bst_find_min(tree);
  ASSERT_NULL(min);

  bst_destroy(tree, NULL);
  return true;
}

bool test_bst_remove_by_data() {
  BST tree = bst_create(compare_ints, NULL);
  int values[] = {50, 30, 70, 20, 40, 60, 80};
  int count = 7;

  for (int i = 0; i < count; i++) {
    bst_insert(tree, &values[i]);
  }

  // Remove middle value
  int to_remove = 50;
  bool removed = bst_remove(tree, &to_remove);

  ASSERT_TRUE(removed);
  ASSERT_EQUAL(bst_size(tree), count - 1);

  bst_destroy(tree, NULL);
  return true;
}

bool test_bst_remove_node_direct() {
  BST tree = bst_create(compare_ints, NULL);
  int values[] = {50, 30, 70, 20, 40, 60, 80};
  BSTNode nodes[7];
  int count = 7;

  // Insert and keep node references
  for (int i = 0; i < count; i++) {
    nodes[i] = bst_insert(tree, &values[i]);
  }

  // Remove using direct node reference (O(log n) operation)
  bst_remove_node(tree, nodes[3]); // Remove 20

  ASSERT_EQUAL(bst_size(tree), count - 1);

  // Verify minimum changed
  int *min = (int *)bst_find_min(tree);
  ASSERT_NOT_NULL(min);
  ASSERT_EQUAL(*min, 30);

  bst_destroy(tree, NULL);
  return true;
}

bool test_bst_remove_min_updates() {
  BST tree = bst_create(compare_ints, NULL);
  int values[] = {50, 30, 70, 20, 40};
  int count = 5;

  for (int i = 0; i < count; i++) {
    bst_insert(tree, &values[i]);
  }

  // Remove minimum
  BSTNode min_node = bst_find_min_node(tree);
  ASSERT_NOT_NULL(min_node);

  int *min_val = (int *)bst_node_get_data(min_node);
  ASSERT_EQUAL(*min_val, 20);

  bst_remove_node(tree, min_node);

  // Check new minimum
  min_val = (int *)bst_find_min(tree);
  ASSERT_NOT_NULL(min_val);
  ASSERT_EQUAL(*min_val, 30);

  bst_destroy(tree, NULL);
  return true;
}

bool test_bst_clear() {
  BST tree = bst_create(compare_ints, NULL);
  int values[] = {50, 30, 70, 20, 40, 60, 80};
  int count = 7;

  for (int i = 0; i < count; i++) {
    bst_insert(tree, &values[i]);
  }

  bst_clear(tree, NULL);

  ASSERT_TRUE(bst_is_empty(tree));
  ASSERT_EQUAL(bst_size(tree), 0);
  ASSERT_NULL(bst_find_min(tree));

  bst_destroy(tree, NULL);
  return true;
}

bool test_bst_node_get_data() {
  BST tree = bst_create(compare_ints, NULL);
  int value = 42;

  BSTNode node = bst_insert(tree, &value);
  int *data = (int *)bst_node_get_data(node);

  ASSERT_NOT_NULL(data);
  ASSERT_EQUAL(*data, 42);

  bst_destroy(tree, NULL);
  return true;
}

// ============================================================================
// Test Runner
// ============================================================================

int main() {
  // Initialize test framework
  test_framework_init();

  // Register tests
  test_print_section("Testing bst_create()");
  test_register("test_bst_create", test_bst_create);

  test_print_section("Testing bst_insert()");
  test_register("test_bst_insert_single", test_bst_insert_single);
  test_register("test_bst_insert_multiple", test_bst_insert_multiple);

  test_print_section("Testing bst_find_min()");
  test_register("test_bst_find_min", test_bst_find_min);
  test_register("test_bst_find_min_empty", test_bst_find_min_empty);

  test_print_section("Testing bst_remove()");
  test_register("test_bst_remove_by_data", test_bst_remove_by_data);
  test_register("test_bst_remove_node_direct", test_bst_remove_node_direct);
  test_register("test_bst_remove_min_updates", test_bst_remove_min_updates);

  test_print_section("Testing bst_clear()");
  test_register("test_bst_clear", test_bst_clear);

  test_print_section("Testing bst_node_get_data()");
  test_register("test_bst_node_get_data", test_bst_node_get_data);

  // Run all tests
  int result = test_run_all();

  // Cleanup
  test_framework_cleanup();

  return result;
}
