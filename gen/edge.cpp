/*
Randomly select an operation from the operation pool.
There will rarely be consecutive identical operations.

Parameters:
q preset_id
*/

#include <bits/stdc++.h>

/* I use pbds to store the set of keys. */
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>
using namespace __gnu_pbds;
template <typename T>
using ordered_set = tree<T, null_type, std::less<T>, rb_tree_tag,
                         tree_order_statistics_node_update>;

#include "const.h"
#include "testlib.h"
using namespace std;

typedef long long int ll;
typedef enum { RED, BLACK } Color;

typedef struct Node {
  int key;
  ll p;
  Color color;
  struct Node *parent;
  struct Node *left;
  struct Node *right;
} Node;

Node *NIL;

void initializeNIL() {
  NIL = (Node *)malloc(sizeof(Node));
  if (NIL == NULL) {
    fprintf(stderr, "Failed to allocate memory for NIL node\n");
    exit(EXIT_FAILURE);
  }
  NIL->color = BLACK;
  NIL->parent = NULL; // Or itself, CLRS varies, NULL is fine for checks
  NIL->left = NULL;
  NIL->right = NULL;
  NIL->p = 0;
  NIL->key = 0; // Arbitrary key for NIL
}

// Function to create a new node
Node *createNode(int key, int p) {
  Node *newNode = (Node *)malloc(sizeof(Node));
  if (newNode == NULL) {
    fprintf(stderr, "Failed to allocate memory for new node\n");
    exit(EXIT_FAILURE);
  }
  newNode->key = key;
  newNode->p = p;
  newNode->color = RED; // New nodes are initially RED (CLRS, Ch. 13.3)
  newNode->parent = NIL;
  newNode->left = NIL;
  newNode->right = NIL;
  return newNode;
}

// Left Rotate (CLRS, Ch. 13.2)
void leftRotate(Node **root, Node *x) {
  Node *y = x->right;
  x->right = y->left;

  if (y->left != NIL) {
    y->left->parent = x;
  }
  y->parent = x->parent;

  if (x->parent == NIL) {
    *root = y;
  } else if (x == x->parent->left) {
    x->parent->left = y;
  } else {
    x->parent->right = y;
  }
  y->left = x;
  x->parent = y;
}

// Right Rotate (CLRS, Ch. 13.2)
void rightRotate(Node **root, Node *y) {
  Node *x = y->left;
  y->left = x->right;

  if (x->right != NIL) {
    x->right->parent = y;
  }
  x->parent = y->parent;

  if (y->parent == NIL) {
    *root = x;
  } else if (y == y->parent->left) {
    y->parent->left = x;
  } else {
    y->parent->right = x;
  }
  x->right = y;
  y->parent = x;
}

// RB-INSERT-FIXUP (CLRS, Ch. 13.3)
void rbInsertFixup(Node **root, Node *z) {
  Node *y;
  while (z->parent->color == RED) {
    if (z->parent ==
        z->parent->parent->left) {  // Case 1, 2, 3: z.p is left child
      y = z->parent->parent->right; // Uncle y
      if (y->color == RED) {        // Case 1: Uncle y is RED
        z->parent->color = BLACK;
        y->color = BLACK;
        z->parent->parent->color = RED;
        z = z->parent->parent;
      } else {
        if (z ==
            z->parent
                ->right) { // Case 2: Uncle y is BLACK and z is a right child
          z = z->parent;
          leftRotate(root, z);
        }
        // Case 3: Uncle y is BLACK and z is a left child
        z->parent->color = BLACK;
        z->parent->parent->color = RED;
        rightRotate(root, z->parent->parent);
      }
    } else { // Case 4, 5, 6: z.p is right child (symmetric to 1, 2, 3)
      y = z->parent->parent->left; // Uncle y
      if (y->color == RED) {       // Case 4: Uncle y is RED
        z->parent->color = BLACK;
        y->color = BLACK;
        z->parent->parent->color = RED;
        z = z->parent->parent;
      } else {
        if (z ==
            z->parent->left) { // Case 5: Uncle y is BLACK and z is a left child
          z = z->parent;
          rightRotate(root, z);
        }
        // Case 6: Uncle y is BLACK and z is a right child
        z->parent->color = BLACK;
        z->parent->parent->color = RED;
        leftRotate(root, z->parent->parent);
      }
    }
  }
  (*root)->color = BLACK; // Ensure root is always BLACK (Property 2)
}

// RB-INSERT (CLRS, Ch. 13.3)
void rbInsert(Node **root, int key, int p) {
  Node *z = createNode(key, p);
  Node *y = NIL;
  Node *x = *root;

  while (x != NIL) {
    y = x;
    if (z->key < x->key) {
      x = x->left;
    } else {
      x = x->right;
    }
  }
  z->parent = y;

  if (y == NIL) { // Tree was empty
    *root = z;
  } else if (z->key < y->key) {
    y->left = z;
  } else {
    y->right = z;
  }

  z->left = NIL;
  z->right = NIL;
  z->color = RED; // New node is RED

  rbInsertFixup(root, z);
}

// RB-TRANSPLANT (CLRS, Ch. 13.4)
// Replaces subtree rooted at u with subtree rooted at v
void rbTransplant(Node **root, Node *u, Node *v) {
  if (u->parent == NIL) {
    *root = v;
  } else if (u == u->parent->left) {
    u->parent->left = v;
  } else {
    u->parent->right = v;
  }
  v->parent = u->parent;
}

// TREE-MINIMUM (CLRS, Ch. 12.2, adapted for RB Tree)
Node *treeMinimum(Node *x) {
  while (x->left != NIL) {
    x = x->left;
  }
  return x;
}

// RB-DELETE-FIXUP (CLRS, Ch. 13.4)
void rbDeleteFixup(Node **root, Node *x) {
  Node *w; // x's sibling
  while (x != *root && x->color == BLACK) {
    if (x == x->parent->left) {
      w = x->parent->right;
      if (w->color == RED) { // Case 1: x's sibling w is red
        w->color = BLACK;
        x->parent->color = RED;
        leftRotate(root, x->parent);
        w = x->parent->right;
      }
      // Case 2: x's sibling w is black, and both of w's children are black
      if (w->left->color == BLACK && w->right->color == BLACK) {
        w->color = RED;
        x = x->parent;
      } else {
        // Case 3: x's sibling w is black, w.left is red, and w.right is black
        if (w->right->color == BLACK) {
          w->left->color = BLACK;
          w->color = RED;
          rightRotate(root, w);
          w = x->parent->right;
        }
        // Case 4: x's sibling w is black, and w.right is red
        w->color = x->parent->color;
        x->parent->color = BLACK;
        w->right->color = BLACK;
        leftRotate(root, x->parent);
        x = *root; // Terminate loop
      }
    } else { // Symmetric: x is right child
      w = x->parent->left;
      if (w->color == RED) { // Case 1
        w->color = BLACK;
        x->parent->color = RED;
        rightRotate(root, x->parent);
        w = x->parent->left;
      }
      // Case 2
      if (w->right->color == BLACK && w->left->color == BLACK) {
        w->color = RED;
        x = x->parent;
      } else {
        // Case 3
        if (w->left->color == BLACK) {
          w->right->color = BLACK;
          w->color = RED;
          leftRotate(root, w);
          w = x->parent->left;
        }
        // Case 4
        w->color = x->parent->color;
        x->parent->color = BLACK;
        w->left->color = BLACK;
        rightRotate(root, x->parent);
        x = *root; // Terminate loop
      }
    }
  }
  x->color = BLACK;
}

// RB-DELETE (CLRS, Ch. 13.4)
// Deletes node z from the tree.
// Note: This implementation assumes z is a valid node in the tree.
// For a complete version, you'd typically search for the key first.
void rbDelete(Node **root, Node *z) {
  if (z == NIL)
    return; // Cannot delete NIL

  Node *y = z;
  Node *x;
  Color yOriginalColor = y->color;

  if (z->left == NIL) {
    x = z->right;
    rbTransplant(root, z, z->right);
  } else if (z->right == NIL) {
    x = z->left;
    rbTransplant(root, z, z->left);
  } else {
    y = treeMinimum(z->right); // y is z's successor
    yOriginalColor = y->color;
    x = y->right;
    if (y->parent == z) {
      x->parent = y; // Important if x is NIL
    } else {
      rbTransplant(root, y, y->right);
      y->right = z->right;
      y->right->parent = y;
    }
    rbTransplant(root, z, y);
    y->left = z->left;
    y->left->parent = y;
    y->color = z->color;
  }

  if (yOriginalColor == BLACK) {
    rbDeleteFixup(root, x);
  }
  free(z); // Free the memory of the deleted node
}

// Helper function to find a node (not part of CLRS RB-DELETE directly, but
// useful)
Node *treeSearch(Node *x, int key) {
  while (x != NIL && key != x->key) {
    if (key < x->key) {
      x = x->left;
    } else {
      x = x->right;
    }
  }
  return x;
}

int getDepth(Node *x) {
  int depth = 0;
  while (x->parent != NIL) {
    ++depth;
    x = x->parent;
  }
  return depth;
}

vector<int> pool, querypool, modpool;
ordered_set<int> avail_keys;
int rnd_op(int idx, int q) {
  if (idx < q / 100)
    return modpool[rnd.next((int)modpool.size())];
  if (q - idx < q / 100)
    return querypool[rnd.next((int)querypool.size())];
  return pool[rnd.next((int)pool.size())];
}
int get_exist_key() {
  if (avail_keys.size() == 0) {
    return rnd.next(MINKEY, MAXKEY);
  }
  auto p = avail_keys.find_by_order(rnd.next(avail_keys.size()));
  return *p;
}
int get_notexist_key() {
  int key = rnd.next(MINKEY, MAXKEY);
  while (avail_keys.find(key) != avail_keys.end()) {
    key = rnd.next(MINKEY, MAXKEY);
  }
  return key;
}

int main(int argc, char *argv[]) {
  registerGen(argc, argv, 1);
  initializeNIL();
  Node *root = NIL;

  int q = opt<int>("q");              // number of queries
  int preset_id = opt<int>("preset"); // preset id
  int invalid_percentage = opt<int>("invalid", (int)1e9);
  int mninitp = opt<int>("min-init", 1);
  int mxinitp = opt<int>("max-init", (int)1e9);
  int mnmovep = opt<int>("min-move", 1);
  int mxmovep = opt<int>("max-move", (int)1e9);

  vector<int> distri = PRESET[preset_id];
  for (int t = 0; t < 6; ++t) { // add operations to pool
    if (t >= 4) {
      for (int i = 0; i < distri[t]; ++i) {
        querypool.push_back(t + 1);
      }
    } else {
      for (int i = 0; i < distri[t]; ++i) {
        modpool.push_back(t + 1);
      }
    }
    for (int i = 0; i < distri[t]; ++i) {
      pool.push_back(t + 1);
    }
  }

  cout << q << "\n";

  for (int i = 0; i < q; ++i) {
    int type = rnd_op(i, q);
    bool valid = rnd.next(invalid_percentage) != 0;
    if (valid && avail_keys.size() == 0) {
      type = 1;
    }
    switch (type) {
    case 1: { // connect x p
      int x = valid ? get_notexist_key() : get_exist_key();
      rbInsert(&root, x, 0);
      avail_keys.insert(x);
      int p = rnd.next(mninitp, mxinitp);
      cout << type << ' ' << x << ' ' << p << '\n';
      break;
    }
    case 2: { // disconnect x
      int x = valid ? get_exist_key() : get_notexist_key();
      Node *node = treeSearch(root, x);
      rbDelete(&root, node);
      avail_keys.erase(x);
      cout << type << ' ' << x << '\n';
      break;
    }
    case 3: { // relocate u v p
      int u, v;
      if (valid) {
        u = get_exist_key();
        v = root->key;
      } else {
        u = get_notexist_key();
        v = get_notexist_key();
        if (rnd.next(2) == 0) {
          int temp = get_exist_key();
          if (rnd.next(2) == 0) {
            swap(u, temp);
          } else {
            swap(v, temp);
          }
        }
      }
      int p = rnd.next(mnmovep, mxmovep);
      cout << type << ' ' << u << ' ' << v << ' ' << p << '\n';
      break;
    }
    case 4: { // evacuate x
      int x;
      if (valid) {
        x = root->key;
        cout << type << ' ' << x << '\n';
      } else {
        x = get_notexist_key();
        cout << type << ' ' << x << '\n';
      }
      break;
    }
    case 5: { // distance u v
      int u, v;
      if (valid) {
        u = get_exist_key();
        v = get_exist_key();
        cout << type << ' ' << u << ' ' << v << '\n';
      } else {
        u = get_notexist_key();
        v = get_notexist_key();
        if (rnd.next(2) == 0) {
          int temp = get_exist_key();
          if (rnd.next(2) == 0) {
            swap(u, temp);
          } else {
            swap(v, temp);
          }
        }
        cout << type << ' ' << u << ' ' << v << '\n';
      }
      break;
    }
    case 6: { // access_index x
      int x = valid ? get_exist_key() : get_notexist_key();
      cout << type << ' ' << x << '\n';
      break;
    }
    }
  }
  return 0;
}
