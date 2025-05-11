#include "testlib.h"
using namespace std;
const int MAXQ = 200000;
const int MINP = 1;
const int MAXP = 1e9;
const int MINKEY = 1;
const int MAXKEY = 1e9;

int main(int argc, char *argv[]) {
  registerValidation(argc, argv);
  int q = inf.readInt(1, MAXQ, "q");
  inf.readEoln();
  for (int i = 0; i < q; ++i) {
    int op = inf.readInt(1, 6, "op[" + to_string(i) + "]");
    inf.readSpace();
    switch (op) {
    case 1: {
      inf.readInt(MINKEY, MAXKEY, "x[" + to_string(i) + "]"); // x
      inf.readSpace();
      inf.readInt(MINP, MAXP, "p[" + to_string(i) + "]"); // p
      break;
    }
    case 2: {
      inf.readInt(MINKEY, MAXKEY, "x[" + to_string(i) + "]"); // x
      break;
    }
    case 3: {
      inf.readInt(MINKEY, MAXKEY, "u[" + to_string(i) + "]"); // u
      inf.readSpace();
      inf.readInt(MINKEY, MAXKEY, "v[" + to_string(i) + "]"); // v
      inf.readSpace();
      inf.readInt(MINP, MAXP, "p[" + to_string(i) + "]"); // p
      break;
    }
    case 5: {
      inf.readInt(MINKEY, MAXKEY, "u[" + to_string(i) + "]"); // u
      inf.readSpace();
      inf.readInt(MINKEY, MAXKEY, "v[" + to_string(i) + "]"); // v
      break;
    }
    case 6: {
      inf.readInt(MINKEY, MAXKEY, "x[" + to_string(i) + "]"); // x
      break;
    }
    default: {
      inf.quitf(_fail, "Unknown operation %d", op);
      break;
    }
    }
    inf.readEoln();
  }
  inf.readEof();
  return 0;
}
