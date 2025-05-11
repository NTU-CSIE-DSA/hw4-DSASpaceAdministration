#include <stdio.h>
#include <stdlib.h>

typedef long long int ll;
typedef enum { RED, BLACK } Color;

typedef struct Node {
  int key;
  ll p;
  int depth; // only for last query
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

int getSize(Node *x) {
  if (x == NIL)
    return 0;
  return 1 + getSize(x->left) + getSize(x->right);
}

ll access_index(Node *x, int xdep, Node *cur, int dep) {
  if (cur == NIL)
    return 0;
  ll dis = dep == xdep && x->color != cur->color ? 2 : abs(xdep - dep);
  return cur->p * (dis * dis) + access_index(x, xdep, cur->left, dep + 1) +
         access_index(x, xdep, cur->right, dep + 1);
}

void addPeople(Node *x, ll p) {
  if (x == NIL)
    return;
  x->p += p;
  addPeople(x->left, p);
  addPeople(x->right, p);
}

void freeTree(Node *x) {
  if (x == NIL) {
    return;
  }
  freeTree(x->left);
  freeTree(x->right);
  free(x);
}

int main() {
  initializeNIL();
  Node *root = NIL;
  int Q;
  scanf("%d", &Q);
  while (Q--) {
    int op;
    scanf("%d", &op);
    switch (op) {
    case 1: {
      int x, p;
      scanf("%d %d", &x, &p);
      Node *node = treeSearch(root, x);
      if (node != NIL)
        break;
      rbInsert(&root, x, p);
      break;
    }
    case 2: {
      int x;
      scanf("%d", &x);
      Node *delnode = treeSearch(root, x);
      if (delnode == NIL)
        break;
      rbDelete(&root, delnode);
      break;
    }
    case 3: {
      int u, v, p;
      scanf("%d %d %d", &u, &v, &p);
      Node *unode = treeSearch(root, u);
      Node *vnode = treeSearch(root, v);
      if (unode == NIL || vnode == NIL)
        break;
      ll move = p > unode->p ? unode->p : p;
      unode->p -= move;
      vnode->p += move;
      break;
    }
    case 4: {
      int x;
      scanf("%d", &x);
      Node *xnode = treeSearch(root, x);
      if (xnode == NIL)
        break;
      int sz = (getSize(xnode) - 1) + 1;
      ll distri = xnode->p / sz;
      addPeople(xnode->left, distri);
      addPeople(xnode->right, distri);
      ll r = xnode->p - (distri * (sz - 1));
      if (xnode->parent != NIL) { // if not root
        xnode->parent->p += r;
      }
      xnode->p = 0;
      break;
    }
    case 5: {
      int u, v;
      scanf("%d %d", &u, &v);
      Node *unode = treeSearch(root, u);
      Node *vnode = treeSearch(root, v);
      if (unode == NIL || vnode == NIL) {
        printf("-1\n");
        break;
      }
      int ud = getDepth(unode), vd = getDepth(vnode);
      if (ud == vd && unode->color != vnode->color) {
        printf("2\n");
      } else {
        printf("%d\n", abs(ud - vd));
      }
      break;
    }
    case 6: {
      int x;
      scanf("%d", &x);
      Node *xnode = treeSearch(root, x);
      if (xnode == NIL) {
        printf("-1\n");
        break;
      }
      int xdep = getDepth(xnode);
      ll ans = access_index(xnode, xdep, root, 0);
      printf("%lld\n", ans);
      break;
    }
    }
  }
  return 0;
}
