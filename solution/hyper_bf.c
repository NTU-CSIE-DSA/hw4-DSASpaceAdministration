#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#define int long long

// === RB Tree ===

typedef enum { RED, BLACK } Color;

typedef struct RBNode {
    int key, pop;
    Color c;
    struct RBNode *l, *r, *p;
} RBNode;

RBNode* root;
RBNode* NIL;

RBNode* rb_create_node(int key, int pop) {
    RBNode* node = (RBNode*)malloc(sizeof(RBNode));
    node->key = key;
    node->pop = pop;
    node->c = RED;
    node->l = node->r = node->p = NIL;
    return node;
}

void left_rotate(RBNode* x) {
    RBNode* y = x->r;
    x->r = y->l;
    if (y->l != NIL) y->l->p = x;
    y->p = x->p;
    if (x->p == NIL)
        root = y;
    else if (x == x->p->l)
        x->p->l = y;
    else
        x->p->r = y;
    y->l = x;
    x->p = y;
}

void right_rotate(RBNode* y) {
    RBNode* x = y->l;
    y->l = x->r;
    if (x->r != NIL) x->r->p = y;
    x->p = y->p;
    if (y->p == NIL)
        root = x;
    else if (y == y->p->l)
        y->p->l = x;
    else
        y->p->r = x;
    x->r = y;
    y->p = x;
}

RBNode* find_node(int data) {
    RBNode* x = root;
    while (x != NIL) {
        if (data == x->key)
            return x;
        else if (data < x->key)
            x = x->l;
        else
            x = x->r;
    }
    return NIL;
}

RBNode* minimum(RBNode* node) {
    while (node->l != NIL) {
        node = node->l;
    }
    return node;
}

void rb_fix_insert(RBNode* z) {
    while (z->p->c == RED) {
        if (z->p == z->p->p->l) {
            RBNode* y = z->p->p->r;
            if (y->c == RED) {
                z->p->c = BLACK;
                y->c = BLACK;
                z->p->p->c = RED;
                z = z->p->p;
            } else {
                if (z == z->p->r) {
                    z = z->p;
                    left_rotate(z);
                }
                z->p->c = BLACK;
                z->p->p->c = RED;
                right_rotate(z->p->p);
            }
        } else {
            RBNode* y = z->p->p->l;
            if (y->c == RED) {
                z->p->c = BLACK;
                y->c = BLACK;
                z->p->p->c = RED;
                z = z->p->p;
            } else {
                if (z == z->p->l) {
                    z = z->p;
                    right_rotate(z);
                }
                z->p->c = BLACK;
                z->p->p->c = RED;
                left_rotate(z->p->p);
            }
        }
    }
    root->c = BLACK;
}

void rb_insert(RBNode* z) {
    RBNode* y = NIL;
    RBNode* x = root;
    while (x != NIL) {
        y = x;
        if (z->key < x->key)
            x = x->l;
        else
            x = x->r;
    }
    z->p = y;
    if (y == NIL)
        root = z;
    else if (z->key < y->key)
        y->l = z;
    else
        y->r = z;
    z->l = NIL;
    z->r = NIL;
    z->c = RED;
    rb_fix_insert(z);
}

void rb_transplant(RBNode* u, RBNode* v) {
    if (u->p == NIL)
        root = v;
    else if (u == u->p->l)
        u->p->l = v;
    else
        u->p->r = v;
    v->p = u->p;
}

void rb_fix_delete(RBNode* x) {
    RBNode* w;
    while (x != root && x->c == BLACK) {
        if (x == x->p->l) {
            w = x->p->r;
            if (w->c == RED) {
                w->c = BLACK;
                x->p->c = RED;
                left_rotate(x->p);
                w = x->p->r;
            }
            if (w->l->c == BLACK && w->r->c == BLACK) {
                w->c = RED;
                x = x->p;
            } else {
                if (w->r->c == BLACK) {
                    w->l->c = BLACK;
                    w->c = RED;
                    right_rotate(w);
                    w = x->p->r;
                }
                w->c = x->p->c;
                x->p->c = BLACK;
                w->r->c = BLACK;
                left_rotate(x->p);
                x = root;
            }
        } else {
            w = x->p->l;
            if (w->c == RED) {
                w->c = BLACK;
                x->p->c = RED;
                right_rotate(x->p);
                w = x->p->l;
            }
            if (w->r->c == BLACK && w->l->c == BLACK) {
                w->c = RED;
                x = x->p;
            } else {
                if (w->l->c == BLACK) {
                    w->r->c = BLACK;
                    w->c = RED;
                    left_rotate(w);
                    w = x->p->l;
                }
                w->c = x->p->c;
                x->p->c = BLACK;
                w->l->c = BLACK;
                right_rotate(x->p);
                x = root;
            }
        }
    }
    x->c = BLACK;
}

void rb_delete(RBNode* z) {
    RBNode* x;
    RBNode* y = z;
    Color y_orig_color = y->c;
    if (z->l == NIL) {
        x = z->r;
        rb_transplant(z, z->r);
    } else if (z->r == NIL) {
        x = z->l;
        rb_transplant(z, z->l);
    } else {
        y = minimum(z->r);
        y_orig_color = y->c;
        x = y->r;
        if (y->p == z) {
            x->p = y;
        } else {
            rb_transplant(y, y->r);
            y->r = z->r;
            y->r->p = y;
        }
        rb_transplant(z, y);
        y->l = z->l;
        y->l->p = y;
        y->c = z->c;
    }
    if (y_orig_color == BLACK) rb_fix_delete(x);
    free(z);
}

void rb_inorder(RBNode* node) {
    if (node == NIL) return;
    rb_inorder(node->l);
    printf("%lld (%s)  ", node->key, node->c == RED ? "R" : "B");
    rb_inorder(node->r);
}

int get_depth(RBNode* node) {
    int depth = 0;
    while (node != NIL) {
        depth++;
        node = node->p;
    }
    return depth;
}

int tree_size(RBNode* x) {
    if (x == NIL) return 0;
    return tree_size(x->l) + tree_size(x->r) + 1;
}

void connect(int x, int p) {
    RBNode* node_x = find_node(x);
    if (node_x != NIL) return;
    node_x = rb_create_node(x, p);
    rb_insert(node_x);
}

void disconnect(int x) {
    RBNode* node_x = find_node(x);
    if (node_x == NIL) return;
    rb_delete(node_x);
}

void relocate(int u, int v, int p) {
    if (u == v) return;
    RBNode *node_u = find_node(u), *node_v = find_node(v);
    if (node_u == NIL || node_v == NIL) return;
    if (node_u->pop < p) p = node_u->pop;
    node_u->pop -= p;
    node_v->pop += p;
}

void tree_add(RBNode* x, int p) {
    if (x == NIL) return;
    x->pop += p;
    tree_add(x->l, p);
    tree_add(x->r, p);
}

void evacuate(int x) {
    RBNode* node_x = find_node(x);
    if (node_x == NIL) return;
    int size = tree_size(node_x);
    int p = node_x->pop / size;
    int r = node_x->pop - p * (size - 1);
    tree_add(node_x, p);
    if (node_x != root) node_x->p->pop += r;
    node_x->pop = 0;
}

int dis(int x, int y) {
    RBNode *node_x = find_node(x), *node_y = find_node(y);
    if (node_x == NIL || node_y == NIL) return -1;
    int depth_x = get_depth(node_x), depth_y = get_depth(node_y);
    if (depth_x == depth_y && node_x->c != node_y->c) return 2;
    return (int)abs(depth_x - depth_y);
}

void distance(int x, int y) { printf("%lld\n", dis(x, y)); }

int calc(RBNode* w, int depth_x, Color c) {
    if (w == NIL) return 0;
    int depth_w = get_depth(w);
    int d = (int)abs(depth_w - depth_x);
    if (depth_w == depth_x && w->c != c) d = 2;
    int val = w->pop * d * d;
    return calc(w->l, depth_x, c) + calc(w->r, depth_x, c) + val;
}

void access_index(int x) {
    RBNode* node_x = find_node(x);
    if (node_x == NIL) {
        printf("-1\n");
        return;
    }
    int depth_x = get_depth(node_x);
    printf("%lld\n", calc(root, depth_x, node_x->c));
}

signed main() {
    NIL = rb_create_node(0, 0);
    NIL->c = BLACK;
    root = NIL;

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
        } else if (op == 3) {
            assert(scanf("%lld %lld %lld", &x, &y, &z) == 3);
            relocate(x, y, z);
        } else if (op == 4) {
            assert(scanf("%lld", &x) == 1);
            evacuate(x);
        } else if (op == 5) {
            assert(scanf("%lld %lld", &x, &y) == 2);
            distance(x, y);
        } else if (op == 6) {
            assert(scanf("%lld", &x) == 1);
            access_index(x);
        }
    }
}
