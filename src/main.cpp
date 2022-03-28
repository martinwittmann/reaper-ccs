#include "ccs/Ccs.h"
#include "ccs/Util.h"
#include <string>

#define MOCK_MIDI

namespace fse = std::experimental::filesystem;
using std::ofstream;
using std::string;

// A simple executable for debugging purposes.
int main() {
  string baseDir = "/home/martin/.config/REAPER";
  string result1 = CCS::Util::formatHexByte(1);
  int aa = 1;
  return 0;
}