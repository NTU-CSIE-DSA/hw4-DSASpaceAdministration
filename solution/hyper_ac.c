#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#define int long long

typedef enum { RED, BLACK } Color;

typedef struct RBNode {
    int data;
    int population;
    Color color;
    struct RBNode *left, *right, *parent;
} RBNode;

typedef struct RBTree {
    RBNode* root;
    RBNode* null;
} RBTree;

RBNode* rb_create_node(RBTree* T, int data, int population) {
    RBNode* node = (RBNode*)malloc(sizeof(RBNode));
    node->data = data;
    node->population = population;
    node->color = RED;
    node->left = node->right = node->parent = T->null;
    return node;
}

void left_rotate(RBTree* T, RBNode* x) {
    RBNode* y = x->right;
    x->right = y->left;
    if (y->left != T->null) y->left->parent = x;
    y->parent = x->parent;
    if (x->parent == T->null)
        T->root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;
    y->left = x;
    x->parent = y;
}

void right_rotate(RBTree* T, RBNode* y) {
    RBNode* x = y->left;
    y->left = x->right;
    if (x->right != T->null) x->right->parent = y;
    x->parent = y->parent;
    if (y->parent == T->null)
        T->root = x;
    else if (y == y->parent->left)
        y->parent->left = x;
    else
        y->parent->right = x;
    x->right = y;
    y->parent = x;
}

void rb_fix_insert(RBTree* T, RBNode* z) {
    while (z->parent->color == RED) {
        if (z->parent == z->parent->parent->left) {
            RBNode* y = z->parent->parent->right;
            if (y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->right) {
                    z = z->parent;
                    left_rotate(T, z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                right_rotate(T, z->parent->parent);
            }
        } else {
            RBNode* y = z->parent->parent->left;
            if (y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    right_rotate(T, z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                left_rotate(T, z->parent->parent);
            }
        }
    }
    T->root->color = BLACK;
}

void rb_insert(RBTree* T, RBNode* z) {
    RBNode* y = T->null;
    RBNode* x = T->root;
    while (x != T->null) {
        y = x;
        if (z->data < x->data)
            x = x->left;
        else
            x = x->right;
    }
    z->parent = y;
    if (y == T->null)
        T->root = z;
    else if (z->data < y->data)
        y->left = z;
    else
        y->right = z;
    z->left = T->null;
    z->right = T->null;
    z->color = RED;
    rb_fix_insert(T, z);
}

void rb_transplant(RBTree* T, RBNode* u, RBNode* v) {
    if (u->parent == T->null)
        T->root = v;
    else if (u == u->parent->left)
        u->parent->left = v;
    else
        u->parent->right = v;
    v->parent = u->parent;
}

void rb_fix_delete(RBTree* T, RBNode* x) {
    RBNode* w;
    while (x != T->root && x->color == BLACK) {
        if (x == x->parent->left) {
            w = x->parent->right;
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                left_rotate(T, x->parent);
                w = x->parent->right;
            }
            if (w->left->color == BLACK && w->right->color == BLACK) {
                w->color = RED;
                x = x->parent;
            } else {
                if (w->right->color == BLACK) {
                    w->left->color = BLACK;
                    w->color = RED;
                    right_rotate(T, w);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->right->color = BLACK;
                left_rotate(T, x->parent);
                x = T->root;
            }
        } else {
            w = x->parent->left;
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                right_rotate(T, x->parent);
                w = x->parent->left;
            }
            if (w->right->color == BLACK && w->left->color == BLACK) {
                w->color = RED;
                x = x->parent;
            } else {
                if (w->left->color == BLACK) {
                    w->right->color = BLACK;
                    w->color = RED;
                    left_rotate(T, w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->left->color = BLACK;
                right_rotate(T, x->parent);
                x = T->root;
            }
        }
    }
    x->color = BLACK;
}

RBNode* find_node(RBTree* T, int data) {
    RBNode* x = T->root;
    while (x != T->null) {
        if (data == x->data)
            return x;
        else if (data < x->data)
            x = x->left;
        else
            x = x->right;
    }
    return T->null;
}

RBNode* minimum(RBTree* T, RBNode* node) {
    while (node->left != T->null) {
        node = node->left;
    }
    return node;
}

void rb_delete(RBTree* T, RBNode* z) {
    RBNode* x;
    RBNode* y = z;
    Color y_orig_color = y->color;
    if (z->left == T->null) {
        x = z->right;
        rb_transplant(T, z, z->right);
    } else if (z->right == T->null) {
        x = z->left;
        rb_transplant(T, z, z->left);
    } else {
        y = minimum(T, z->right);
        y_orig_color = y->color;
        x = y->right;
        if (y->parent == z) {
            x->parent = y;
        } else {
            rb_transplant(T, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        rb_transplant(T, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }
    if (y_orig_color == BLACK) rb_fix_delete(T, x);
    free(z);
}

void rb_inorder(RBTree* T, RBNode* node) {
    if (node == T->null) return;
    rb_inorder(T, node->left);
    printf("%lld (%s)  ", node->data, node->color == RED ? "R" : "B");
    rb_inorder(T, node->right);
}

RBTree* rb_create_tree() {
    RBTree* T = (RBTree*)malloc(sizeof(RBTree));
    T->null = (RBNode*)malloc(sizeof(RBNode));
    T->null->color = BLACK;
    T->root = T->null;
    return T;
}

int get_depth(RBTree* T, RBNode* node) {
    int depth = 0;
    while (node != T->null) {
        depth++;
        node = node->parent;
    }
    return depth;
}

RBTree* T;

void connect(int x, int p) {
    RBNode* z = find_node(T, x);
    if (z != T->null) return;
    z = rb_create_node(T, x, p);
    // printf("[DEBUG] inserting (%lld, %lld)\n", x, p);
    rb_insert(T, z);
}

void disconnect(int x) {
    RBNode* z = find_node(T, x);
    if (z == T->null) return;
    // printf("[DEBUG] deleting (%lld, %lld)\n", z->data, z->population);
    rb_delete(T, z);
}

void distance(int x, int y) {
    // printf("[DEBUG] distance(%lld, %lld) = ", x, y);
    RBNode* node_x = find_node(T, x);
    RBNode* node_y = find_node(T, y);
    if (node_x == T->null || node_y == T->null) {
        printf("-1\n");
        return;
    }
    int depth_x = get_depth(T, node_x);
    int depth_y = get_depth(T, node_y);
    if (depth_x == depth_y && node_x->color != node_y->color) {
        printf("2\n");
        return;
    }
    printf("%d\n", abs(depth_x - depth_y));
}

signed main() {
    T = rb_create_tree();
    int q;
    assert(scanf("%lld", &q) == 1);
    while (q--) {
        int op, x, y, z;
        assert(scanf("%lld", &op) == 1);
        if (op == 1) {
            assert(scanf("%lld %lld", &x, &y) == 2);
            connect(x, y);
        } else if (op == 2) {
            assert(scanf("%lld", &x) == 1);
            disconnect(x);
        } else if (op == 5) {
            assert(scanf("%lld %lld", &x, &y) == 2);
            distance(x, y);
        }
        // printf("\n[DEBUG] tree: \n");
        // rb_inorder(T, T->root);
        // printf("\n\n");
    }
}
