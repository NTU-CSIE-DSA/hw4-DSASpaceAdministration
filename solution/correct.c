#include <stdio.h>
#include <stdlib.h>
#define MAXH 38
typedef long long ll;
typedef enum { RED, BLACK } Color;

typedef struct Node {
  int key;
  ll p;
  ll lazy;
  int nodecnt[2][MAXH];
  ll pcnt[2][MAXH];
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
  NIL->p = 0;
  NIL->lazy = 0;
  for (int i = 0; i < MAXH; ++i) {
    NIL->pcnt[0][i] = NIL->pcnt[1][i] = 0;       // 0: red, 1: black
    NIL->nodecnt[0][i] = NIL->nodecnt[1][i] = 0; // 0: red, 1: black
  }
  NIL->left = NULL;
  NIL->right = NULL;
  NIL->key = 0; // Arbitrary key for NIL
}

// Function to create a new node
Node *createNode(int key, ll p) {
  Node *newNode = (Node *)malloc(sizeof(Node));
  if (newNode == NULL) {
    fprintf(stderr, "Failed to allocate memory for new node\n");
    exit(EXIT_FAILURE);
  }
  newNode->key = key;
  newNode->lazy = 0;
  newNode->p = p;
  for (int i = 0; i < MAXH; ++i) {
    newNode->pcnt[0][i] = newNode->pcnt[1][i] = 0;
    newNode->nodecnt[0][i] = newNode->nodecnt[1][i] = 0;
  }
  newNode->color = RED; // New nodes are initially RED (CLRS, Ch. 13.3)
  newNode->pcnt[newNode->color == BLACK][0] = p;
  newNode->nodecnt[newNode->color == BLACK][0] = 1;
  newNode->parent = NIL;
  newNode->left = NIL;
  newNode->right = NIL;
  return newNode;
}

void setColor(Node *x, Color color) {
  if (x == NIL)
    return;
  if (x->color == color)
    return;
  x->color = color;
  x->nodecnt[x->color == BLACK][0] = 1;
  x->nodecnt[x->color != BLACK][0] = 0;
  x->pcnt[x->color == BLACK][0] = x->p;
  x->pcnt[x->color != BLACK][0] = 0;
  return;
}

void tag(Node *x, ll p) {
  if (x == NIL)
    return;
  x->lazy += p;
  x->p += p;
  for (int i = 0; i < MAXH; ++i) {
    x->pcnt[0][i] += p * x->nodecnt[0][i];
    x->pcnt[1][i] += p * x->nodecnt[1][i];
  }
}
void push(Node *x) {
  if (x == NIL)
    return;
  if (x->lazy == 0)
    return;
  tag(x->left, x->lazy);
  tag(x->right, x->lazy);
  x->lazy = 0;
}
void pull(Node *x) {
  if (x == NIL)
    return;
  push(x);
  for (int i = 1; i < MAXH; ++i) {
    x->pcnt[0][i] = x->left->pcnt[0][i - 1] + x->right->pcnt[0][i - 1];
    x->pcnt[1][i] = x->left->pcnt[1][i - 1] + x->right->pcnt[1][i - 1];
    x->nodecnt[0][i] = x->left->nodecnt[0][i - 1] + x->right->nodecnt[0][i - 1];
    x->nodecnt[1][i] = x->left->nodecnt[1][i - 1] + x->right->nodecnt[1][i - 1];
  }
  x->pcnt[x->color == BLACK][0] = x->p;
  x->pcnt[x->color != BLACK][0] = 0;
  x->nodecnt[x->color == BLACK][0] = 1;
  x->nodecnt[x->color != BLACK][0] = 0;
}
void pull_layers(Node *x, int layers) {
  while (x != NIL && layers) {
    pull(x);
    x = x->parent;
    --layers;
  }
}
void pull_til_root(Node *x) {
  while (x != NIL) {
    pull(x);
    x = x->parent;
  }
}

// Left Rotate (CLRS, Ch. 13.2)
void leftRotate(Node **root, Node *x) {
  push(x);
  Node *y = x->right;
  push(y);
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
  pull_til_root(x);
}

// Right Rotate (CLRS, Ch. 13.2)
void rightRotate(Node **root, Node *y) {
  push(y);
  Node *x = y->left;
  push(x);
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
  pull_til_root(y);
}

// RB-INSERT-FIXUP (CLRS, Ch. 13.3)
void rbInsertFixup(Node **root, Node *z) {
  Node *y;
  while (z->parent->color == RED) {
    // push(z->parent->parent->parent);
    // push(z->parent->parent);
    // push(z->parent);
    // push(z);
    if (z->parent ==
        z->parent->parent->left) {  // Case 1, 2, 3: z.p is left child
      y = z->parent->parent->right; // Uncle y
      if (y->color == RED) {        // Case 1: Uncle y is RED
        setColor(z->parent, BLACK);
        setColor(y, BLACK);
        setColor(z->parent->parent, RED);
        // pull_til_root(z->parent->parent);
        pull_layers(z->parent->parent, 3);
        z = z->parent->parent;
      } else {
        if (z ==
            z->parent
                ->right) { // Case 2: Uncle y is BLACK and z is a right child
          z = z->parent;
          leftRotate(root, z);
        }
        // Case 3: Uncle y is BLACK and z is a left child
        setColor(z->parent, BLACK);
        setColor(z->parent->parent, RED);
        // pull_til_root(z->parent->parent);
        pull_layers(z->parent->parent, 3);
        rightRotate(root, z->parent->parent);
      }
    } else { // Case 4, 5, 6: z.p is right child (symmetric to 1, 2, 3)
      y = z->parent->parent->left; // Uncle y
      if (y->color == RED) {       // Case 4: Uncle y is RED
        setColor(z->parent, BLACK);
        setColor(y, BLACK);
        setColor(z->parent->parent, RED);
        pull_layers(z->parent->parent, 3);
        z = z->parent->parent;
      } else {
        if (z ==
            z->parent->left) { // Case 5: Uncle y is BLACK and z is a left child
          z = z->parent;
          rightRotate(root, z);
        }
        // Case 6: Uncle y is BLACK and z is a right child
        setColor(z->parent, BLACK);
        setColor(z->parent->parent, RED);
        pull_layers(z->parent->parent, 3);
        leftRotate(root, z->parent->parent);
      }
    }
  }
  setColor(*root, BLACK); // Ensure root is always BLACK (Property 2)
  pull_til_root(z);
}

// RB-INSERT (CLRS, Ch. 13.3)
void rbInsert(Node **root, int key, int p) {
  Node *z = createNode(key, p);
  Node *y = NIL;
  Node *x = *root;

  while (x != NIL) {
    push(x);
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
  setColor(z, RED);
  pull_til_root(z->parent); // z->parent = y

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
    push(x);
    x = x->left;
  }
  push(x);
  return x;
}

// RB-DELETE-FIXUP (CLRS, Ch. 13.4)
void rbDeleteFixup(Node **root, Node *x) {
  Node *w; // x's sibling
  while (x != *root && x->color == BLACK) {
    if (x == x->parent->left) {
      w = x->parent->right;
      push(w);
      if (w->color == RED) { // Case 1: x's sibling w is red
        setColor(w, BLACK);
        setColor(x->parent, RED);
        pull(x->parent);
        leftRotate(root, x->parent);
        w = x->parent->right;
        push(w);
      }
      // Case 2: x's sibling w is black, and both of w's children are black
      if (w->left->color == BLACK && w->right->color == BLACK) {
        setColor(w, RED);
        pull(x->parent);
        x = x->parent;
      } else {
        // Case 3: x's sibling w is black, w.left is red, and w.right is black
        if (w->right->color == BLACK) {
          setColor(w->left, BLACK);
          setColor(w, RED);
          rightRotate(root, w);
          w = x->parent->right;
          push(w);
        }
        // Case 4: x's sibling w is black, and w.right is red
        setColor(w, x->parent->color);
        setColor(x->parent, BLACK);
        setColor(w->right, BLACK);
        pull(w);
        pull(x->parent);
        leftRotate(root, x->parent);
        x = *root; // Terminate loop
      }
    } else { // Symmetric: x is right child
      w = x->parent->left;
      push(w);
      if (w->color == RED) { // Case 1
        setColor(w, BLACK);
        setColor(x->parent, RED);
        rightRotate(root, x->parent);
        w = x->parent->left;
        push(w);
      }
      // Case 2
      if (w->right->color == BLACK && w->left->color == BLACK) {
        setColor(w, RED);
        pull(x->parent);
        x = x->parent;
      } else {
        // Case 3
        if (w->left->color == BLACK) {
          setColor(w->right, BLACK);
          setColor(w, RED);
          leftRotate(root, w);
          w = x->parent->left;
          push(w);
        }
        // Case 4
        setColor(w, x->parent->color);
        setColor(x->parent, BLACK);
        setColor(w->left, BLACK);
        pull(w);
        pull(x->parent);
        rightRotate(root, x->parent);
        x = *root; // Terminate loop
      }
    }
  }
  setColor(x, BLACK);
  pull_til_root(x);
}

// RB-DELETE (CLRS, Ch. 13.4)
// Deletes node z from the tree.
// Note: This implementation assumes z is a valid node in the tree.
// For a complete version, you'd typically search for the key first.
// Should run treeSearch first or lazy tag won't be updated
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
      rbTransplant(root, y, y->right); // remove y from current place
      y->right = z->right;
      y->right->parent = y;
    }
    rbTransplant(root, z, y);
    y->left = z->left;
    y->left->parent = y;
    setColor(y, z->color);
  }
  pull_til_root(x->parent);

  if (yOriginalColor == BLACK) {
    rbDeleteFixup(root, x);
  }
  free(z); // Free the memory of the deleted node
}

// Helper function to find a node (not part of CLRS RB-DELETE directly, but
// useful)
Node *treeSearch(Node *x, int key) {
  while (x != NIL && key != x->key) {
    push(x);
    if (key < x->key) {
      x = x->left;
    } else {
      x = x->right;
    }
  }
  push(x);
  return x;
}

int getSize(Node *x) {
  int sz = 0;
  for (int i = 0; i < MAXH; ++i) {
    sz += x->nodecnt[0][i] + x->nodecnt[1][i];
  }
  return sz;
}
int getDepth(Node *x) {
  int depth = 0;
  while (x->parent != NIL) {
    ++depth;
    x = x->parent;
  }
  return depth;
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
      Node *node = treeSearch(root, x);
      if (node == NIL)
        break;
      rbDelete(&root, node);
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
      unode->pcnt[unode->color == BLACK][0] -= move;
      vnode->p += move;
      vnode->pcnt[vnode->color == BLACK][0] += move;
      pull_til_root(unode);
      pull_til_root(vnode);
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
      tag(xnode->left, distri);
      tag(xnode->right, distri);
      ll r = xnode->p - (distri * (sz - 1));
      if (xnode->parent != NIL) {
        xnode->parent->p += r;
      }
      xnode->p = 0;
      pull_til_root(xnode);
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
      // fprintf(stderr, "cnt:\n");
      // fprintf(stderr, "red: ");
      // for (int i = 0; i < MAXH; ++i) {
      //   fprintf(stderr, "%lld ", root->cnt[0][i]);
      // }
      // fprintf(stderr, "\n");
      // fprintf(stderr, "black: ");
      // for (int i = 0; i < MAXH; ++i) {
      //   fprintf(stderr, "%lld ", root->cnt[1][i]);
      // }
      // fprintf(stderr, "\n");
      if (xnode == NIL) {
        printf("-1\n");
        break;
      }
      int xd = getDepth(xnode);
      int xc = xnode->color == BLACK;
      ll ans = 0;
      for (int i = 0; i < MAXH; ++i) {
        ans += root->pcnt[xc][i] * (abs(i - xd) * abs(i - xd));
        if (i != xd) {
          ans += root->pcnt[!xc][i] * (abs(i - xd) * abs(i - xd));
        } else {
          ans += root->pcnt[!xc][i] * 4;
        }
      }
      printf("%lld\n", ans);
      break;
    }
    }
  }
  freeTree(root);
  return 0;
}
