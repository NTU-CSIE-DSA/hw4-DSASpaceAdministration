# DSA-HW4 DSA Space Administration
## Solution Sketch
### Expected Complexity
- **Time:** $O(Q\log^2{N})$ where $N$ is the number of greenhouses at any time ($N \le Q$).
- **Space:** $O(N\log{N})$

---

### Subtasks 1 & 2 (Small Q, All Operations)

For the initial subtasks where the number of queries $Q$ is small, a direct implementation combined with an efficient distance calculation will be sufficient.

First, implement a standard **Red-Black Tree** based on the provided textbook pseudocode. The complexities for the basic operations are:
- **Operation 1 (Connect):** Standard RBT insertion. Complexity: $O(\log N)$.
- **Operation 2 (Disconnect):** Standard RBT deletion. Complexity: $O(\log N)$.
- **Operation 3 (Relocate):** Find two nodes and update their population fields. Complexity: $O(\log N)$.
- **Operation 4 (Evacuate):** A naive approach involves finding the node and then traversing its entire subtree, which can take $O(N)$ time.
- **Operations 5 & 6 (Distance, Access Index):** The performance of these depends heavily on the distance calculation. But it's unlikely to be worse than O($N^2$)

**Optimized Distance Calculation ($O(1)$ after node lookup)**

The key insight is to leverage the "Same-Color Dimensional Channels." These channels provide significant shortcuts. Let's analyze the shortest distance $\text{dist}(u, v)$ between nodes $u$ and $v$ with depths $d_u$ and $d_v$.

1.  **Same-Color Nodes:** If `color(u) == color(v)`, a direct dimensional channel connects them. The cost of traversing this channel is defined as $|d_u - d_v|$. Since this is a single-step path, it is guaranteed to be the shortest.
    $$\text{dist}(u, v) = |d_u - d_v|$$

2.  **Different-Color Nodes:** Assume, without loss of generality, that node $u$ is **black** and node $v$ is **red**. No direct dimensional channel exists. However, we can use a black-colored neighbor of $v$ as an intermediary. By RBT Property 4, a red node's parent and non-NIL children must be black.
    - We can form a path from $u$ (black) to a black neighbor of $v$, and then take the final tree-structure edge to $v$.
    - **Path via parent:** `u` $\to$ `parent(v)` $\to$ `v`. Both $u$ and `parent(v)` are black. The total cost is
    $$|d_u - d_{\text{parent}(v)}| + |d_{\text{parent}(v)} - d_v| = |d_u - (d_v - 1)| + 1$$
    - **Path via a child:** `u` $\to$ `child(v)` $\to$ `v`. Both $u$ and `child(v)` are black. The cost is
    $$|d_u - d_{\text{child}(v)}| + |(d_{\text{child}(v)} - d_v)| = |d_u - (d_v + 1)| + 1$$

    The shortest path is the minimum of these options. Simplifying the expressions:
    - If $d_u > d_v$, the path through a child is shorter: $|d_u - (d_v + 1)| + 1 = (d_u - d_v - 1) + 1 = |d_u - d_v|$.
    - If $d_u < d_v$, the path through the parent is shorter: $|d_u - (d_v - 1)| + 1 = (-(d_u - d_v + 1)) + 1 = d_v - d_u = |d_u - d_v|$.
    - If $d_u = d_v$, both paths yield a cost of $|d_v - (d_v-1)| + 1 = 2$.

**Complexity Analysis for Subtasks 1 & 2**

- **Operation 5 (Distance):** Find nodes $u$, $v$ and their depths ($O(\log N)$) and then apply the $O(1)$ formula. Total: **$O(\log N)$**.
- **Operation 6 (Access Index):** Iterate through all $N$ nodes using DFS or BFS, calculate its depth to obtain the distance from each to $x$, and compute the weighted sum using above formula. Total: **$O(N)$**.

This approach is efficient enough for the small constraints of Subtasks 1 and 2, and this approach can be used in the following subtasks.

---

### Subtasks 3 & 4 (Large Q, No Evacuate)

With large $Q$, the $O(N \log N)$ complexity for Operation 6 is too slow. We must optimize the calculation of the Accessibility Index $\mathcal{A}(x) = \sum_{ w \in G} p_{w} \times [\text{dist}(w, x)]^2$.

The distance formula shows that distance only depends on the **colors** and **depths** of the nodes. To support this efficiently, we augment each node in the RBT to maintain population maps by color and relative depth. Each node stores two maps:

- One for black nodes and another for red nodes, indexed by depth, each storing total population at that relative depth within the subtree. This allows us to pre-aggregate data. That is, store the sums of populations at each depth for both colors in the Red-Black Tree.

To update these maps efficiently in Red-Black tree operations, we can abstract a `pull(x)` operation thet merges the maps from `x.left` and `x.right` and adds `x`'s own `(depth, population)` contribution. Merging two maps (of size $O(\log N)$) takes $O(\log N)$ time.

It can be implemented as follows (it's from my solution, the naming conventions are different, but the logic is the same):
```c
void pull(Node *x) {
  if (x == NIL)
    return;
  for (int i = 1; i < MAXH; ++i) {
    x->cnt[0][i] = x->left->cnt[0][i - 1] + x->right->cnt[0][i - 1];
    x->cnt[1][i] = x->left->cnt[1][i - 1] + x->right->cnt[1][i - 1];
  }
  x->cnt[x->color == BLACK][0] = x->p;
  x->cnt[x->color != BLACK][0] = 0;
}
```

Any operation that alters the tree structure or node data requires updating the augmented information along the ancestor path to the root. As there are at most $O(\log N)$ nodes that their informations should be updated, and each `pull` costs $O(\log N)$ time. Hence, each operation (Insert, Delete, Relocate) will take **$O(\log^2 N)$**. These properties should be maintained carefully during the Red-Black Tree operations, such as rotations and recoloring, to ensure that the population sums remain correct.

After augmenting the Red-Black Tree, we can efficiently compute the Accessibility Index $\mathcal{A}(x)$ for any node $x$ by iterating through all the possible depths and using the precomputed population sums in the formula we derived earlier.

This augmented approach brings the overall time complexity to $O(Q\log^2 N)$.

---

### Subtask 5 (Large Q, All Operations)

The final step is to efficiently implement **Operation 4 (Evacuate)**. The naive $O(N)$ subtree traversal is too slow. We can solve this with **lazy propagation**. The `Evacuate` operation adds a fixed population amount to every descendant, which is a classic subtree update problem.

In this case, each node carries a lazy tag indicating a pending population increment (the populations that should be added to each node in the subtree). We also augment the tree further by maintaining counts of nodes per color and depth. When a node is accessed, the `push()` operation applies the pending updates to its subtree summary maps and propagates the lazy tag to its children.

This is the implementation of the `push` operation (from my solution):

```c
void push(Node *x) {
  if (x == NIL)
    return;
  if (x->lazy == 0)
    return;
  tag(x->left, x->lazy);
  tag(x->right, x->lazy);
  x->lazy = 0;
}
```

All operations that traverse the tree (search, insert, delete) must now call `push` on each node along the path. This adds an $O(\log N)$ factor to traversal, all of them now takes $O(\log^2 N)$ time amortized over all operations. Additionally, the lazy tag should also be maintained carefully during Red-Black Tree operations, such as rotations and recoloring, to ensure that the lazy updates are correctly propagated before modifying the tree structure.

With this lazy mechanism, the `Evacuate` operation's complexity becomes **$O(\log N)$** itself for putting only the lazy tags on the nodes without subtree traversal, allowing the solution to pass all subtasks within the time limit. The final complexity for the entire problem is $O(Q\log^2 N)$.
