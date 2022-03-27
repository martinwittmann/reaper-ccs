#include "ccs/Ccs.h"
#include "ccs/Util.h"
#include <fstream>
#include <string>

#define MOCK_MIDI

namespace fse = std::experimental::filesystem;
using std::ofstream;
using std::string;

// A simple executable for debugging purposes.
int main() {
  string baseDir = "/home/martin/.config/REAPER";
  auto ccs = new CCS::Ccs(baseDir, nullptr);
  int aa = 1;
  delete ccs;
  return 0;
}