#include "bst.h"
#include <stdio.h>
#include <stdlib.h>

// Internal node structure
typedef struct BSTNodeImpl {
  void *data;                 // Pointer to the stored element (e.g., Segment)
  struct BSTNodeImpl *left;   // Left child
  struct BSTNodeImpl *right;  // Right child
  struct BSTNodeImpl *parent; // Parent node (for efficient deletion)
} BSTNodeImpl;

// Internal BST structure 
typedef struct {
  BSTNodeImpl *root;      // Root of the tree
  BSTCompareFunc compare; // Comparison function
  void *context;          // Context for comparison (e.g., source point)
  int size;               // Number of elements
} BSTImpl;

// ============================================================================
// Private Helper Functions
// ============================================================================

/**
 * Creates a new BST node.
 */
static BSTNodeImpl *create_node(void *data) {
  BSTNodeImpl *node = malloc(sizeof(BSTNodeImpl));
  if (node == NULL) {
    return NULL;
  }

  node->data = data;
  node->left = NULL;
  node->right = NULL;
  node->parent = NULL;

  return node;
}

/**
 * Finds the minimum node in a subtree.
 */
static BSTNodeImpl *find_min_node_internal(BSTNodeImpl *node) {
  if (node == NULL) {
    return NULL;
  }

  while (node->left != NULL) {
    node = node->left;
  }

  return node;
}

/**
 * Finds the maximum node in a subtree.
 */
static BSTNodeImpl *find_max_node_internal(BSTNodeImpl *node) {
  if (node == NULL) {
    return NULL;
  }

  while (node->right != NULL) {
    node = node->right;
  }

  return node;
}

/**
 * Replaces one subtree as a child of its parent with another subtree.
 */
static void transplant(BSTImpl *tree, BSTNodeImpl *u, BSTNodeImpl *v) {
  if (u->parent == NULL) {
    tree->root = v;
  } else if (u == u->parent->left) {
    u->parent->left = v;
  } else {
    u->parent->right = v;
  }

  if (v != NULL) {
    v->parent = u->parent;
  }
}

/**
 * Recursively clears a subtree.
 */
static void clear_subtree(BSTNodeImpl *node, BSTDestroyFunc destroy_func) {
  if (node == NULL) {
    return;
  }

  clear_subtree(node->left, destroy_func);
  clear_subtree(node->right, destroy_func);

  if (destroy_func != NULL && node->data != NULL) {
    destroy_func(node->data);
  }

  free(node);
}

// ============================================================================
// Public API Implementation
// ============================================================================

BST bst_create(BSTCompareFunc compare_func, void *context) {
  if (compare_func == NULL) {
    return NULL;
  }

  BSTImpl *tree = malloc(sizeof(BSTImpl));
  if (tree == NULL) {
    return NULL;
  }

  tree->root = NULL;
  tree->compare = compare_func;
  tree->context = context;
  tree->size = 0;

  return (BST)tree;
}

BSTNode bst_insert(BST tree, void *data) {
  if (tree == NULL || data == NULL) {
    return NULL;
  }

  BSTImpl *impl = (BSTImpl *)tree;
  BSTNodeImpl *new_node = create_node(data);
  if (new_node == NULL) {
    return NULL;
  }

  // Empty tree case
  if (impl->root == NULL) {
    impl->root = new_node;
    impl->size++;
    return (BSTNode)new_node;
  }

  // Find insertion point
  BSTNodeImpl *current = impl->root;
  BSTNodeImpl *parent = NULL;

  while (current != NULL) {
    parent = current;
    int cmp = impl->compare(data, current->data, impl->context);

    if (cmp < 0) {
      current = current->left;
    } else {
      current = current->right;
    }
  }

  // Insert as child of parent
  new_node->parent = parent;
  int cmp = impl->compare(data, parent->data, impl->context);

  if (cmp < 0) {
    parent->left = new_node;
  } else {
    parent->right = new_node;
  }

  impl->size++;
  return (BSTNode)new_node;
}

void bst_remove_node(BST tree, BSTNode node) {
  if (tree == NULL || node == NULL) {
    return;
  }

  BSTImpl *impl = (BSTImpl *)tree;
  BSTNodeImpl *z = (BSTNodeImpl *)node;

  // Case 1: Node has no left child
  if (z->left == NULL) {
    transplant(impl, z, z->right);
  }
  // Case 2: Node has no right child
  else if (z->right == NULL) {
    transplant(impl, z, z->left);
  }
  // Case 3: Node has two children
  else {
    // Find successor (minimum in right subtree)
    BSTNodeImpl *y = find_min_node_internal(z->right);

    // If successor is not the immediate right child
    if (y->parent != z) {
      transplant(impl, y, y->right);
      y->right = z->right;
      y->right->parent = y;
    }

    transplant(impl, z, y);
    y->left = z->left;
    y->left->parent = y;
  }

  free(z);
  impl->size--;
}

bool bst_remove(BST tree, void *data) {
  if (tree == NULL || data == NULL) {
    return false;
  }

  BSTImpl *impl = (BSTImpl *)tree;
  BSTNodeImpl *current = impl->root;

  // Search for the node
  while (current != NULL) {
    int cmp = impl->compare(data, current->data, impl->context);

    if (cmp == 0) {
      // Found it
      bst_remove_node(tree, (BSTNode)current);
      return true;
    } else if (cmp < 0) {
      current = current->left;
    } else {
      current = current->right;
    }
  }

  return false;
}

void *bst_find_min(BST tree) {
  BSTNode node = bst_find_min_node(tree);
  return bst_node_get_data(node);
}

BSTNode bst_find_min_node(BST tree) {
  if (tree == NULL) {
    return NULL;
  }

  BSTImpl *impl = (BSTImpl *)tree;
  BSTNodeImpl *min_node = find_min_node_internal(impl->root);

  return (BSTNode)min_node;
}

void *bst_node_get_data(BSTNode node) {
  if (node == NULL) {
    return NULL;
  }

  BSTNodeImpl *impl = (BSTNodeImpl *)node;
  return impl->data;
}

bool bst_is_empty(BST tree) {
  if (tree == NULL) {
    return true;
  }

  BSTImpl *impl = (BSTImpl *)tree;
  return impl->root == NULL;
}

int bst_size(BST tree) {
  if (tree == NULL) {
    return 0;
  }

  BSTImpl *impl = (BSTImpl *)tree;
  return impl->size;
}

void bst_clear(BST tree, BSTDestroyFunc destroy_func) {
  if (tree == NULL) {
    return;
  }

  BSTImpl *impl = (BSTImpl *)tree;
  clear_subtree(impl->root, destroy_func);
  impl->root = NULL;
  impl->size = 0;
}

void bst_destroy(BST tree, BSTDestroyFunc destroy_func) {
  if (tree == NULL) {
    return;
  }

  bst_clear(tree, destroy_func);
  free(tree);
}
