#ifndef BST_H
#define BST_H

#include <stdbool.h>

// Opaque pointer definitions
typedef void *BST;
typedef void *BSTNode;

/**
 * Comparison function type for BST elements.
 * @param a First element
 * @param b Second element
 * @param context Additional context for comparison (e.g., source point)
 * @return Negative if a < b, 0 if a == b, positive if a > b
 */
typedef int (*BSTCompareFunc)(void *a, void *b, void *context);

/**
 * Destructor function type for BST elements.
 * @param data Element to destroy
 */
typedef void (*BSTDestroyFunc)(void *data);

/**
 * Creates a new BST instance.
 * @param compare_func Comparison function for ordering elements
 * @param context Additional context passed to comparison function (e.g., source
 * point)
 * @return Pointer to the new BST object, or NULL on allocation failure
 */
BST bst_create(BSTCompareFunc compare_func, void *context);

/**
 * Inserts an element into the BST.
 * @param tree BST handle
 * @param data Element to insert
 * @return Pointer to the created node (can be used as helper for fast
 * deletion), or NULL on failure
 */
BSTNode bst_insert(BST tree, void *data);

/**
 * Removes an element from the BST using direct node reference.
 * This is O(log n) operation when using the helper pointer.
 * @param tree BST handle
 * @param node Node to remove (obtained from bst_insert)
 */
void bst_remove_node(BST tree, BSTNode node);

/**
 * Removes an element from the BST by searching for it.
 * This is O(log n) operation.
 * @param tree BST handle
 * @param data Element to remove
 * @return true if element was found and removed, false otherwise
 */
bool bst_remove(BST tree, void *data);

/**
 * Finds the minimum element in the BST (leftmost node).
 * This represents the closest segment to the source point in the visibility
 * algorithm.
 * @param tree BST handle
 * @return Pointer to the minimum element, or NULL if tree is empty
 */
void *bst_find_min(BST tree);

/**
 * Finds the minimum node in the BST (leftmost node).
 * @param tree BST handle
 * @return Pointer to the minimum node, or NULL if tree is empty
 */
BSTNode bst_find_min_node(BST tree);

/**
 * Gets the data stored in a node.
 * @param node Node handle
 * @return Pointer to the data, or NULL if node is NULL
 */
void *bst_node_get_data(BSTNode node);

/**
 * Checks if the BST is empty.
 * @param tree BST handle
 * @return true if empty, false otherwise
 */
bool bst_is_empty(BST tree);

/**
 * Gets the number of elements in the BST.
 * @param tree BST handle
 * @return Number of elements
 */
int bst_size(BST tree);

/**
 * Clears all elements from the BST.
 * @param tree BST handle
 * @param destroy_func Optional function to destroy element data (can be NULL)
 */
void bst_clear(BST tree, BSTDestroyFunc destroy_func);

/**
 * Destroys the BST and frees all memory.
 * @param tree BST handle
 * @param destroy_func Optional function to destroy element data (can be NULL)
 */
void bst_destroy(BST tree, BSTDestroyFunc destroy_func);

/**
 * Callback function type for BST traversal.
 * @param data Element data
 * @param user_data User-provided context
 */
typedef void (*BSTForeachFunc)(void *data, void *user_data);

/**
 * Iterates through all elements in the BST (in-order traversal).
 * @param tree BST handle
 * @param func Callback function to call for each element
 * @param user_data User-provided context passed to callback
 */
void bst_foreach(BST tree, BSTForeachFunc func, void *user_data);

#endif // BST_H
