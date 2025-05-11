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
      avail_keys.insert(x);
      int p = rnd.next(mninitp, mxinitp);
      cout << type << ' ' << x << ' ' << p << '\n';
      break;
    }
    case 2: { // disconnect x
      int x = valid ? get_exist_key() : get_notexist_key();
      avail_keys.erase(x);
      cout << type << ' ' << x << '\n';
      break;
    }
    case 3: { // relocate u v p
      int u, v;
      if (valid) {
        u = get_exist_key();
        v = get_exist_key();
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
        x = get_exist_key();
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
