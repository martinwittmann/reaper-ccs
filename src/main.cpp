#include "ccs/Ccs.h"
#include "ccs/Util.h"
#include <string>

#define MOCK_MIDI

namespace fse = std::experimental::filesystem;
using std::ofstream;
using std::string;

// A simple executable for debugging purposes.
int main() {
  char a = 0x00;
  char b = 0x7F;
  char c = a + b;
  c -= 1;
  if (c == 0x7F) {
    int g = 1;
  }
  int aa = 1;
  return 0;
}