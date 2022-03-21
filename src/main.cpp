#include "Ccs.h"
#include <fstream>
#include <string>

namespace fse = std::experimental::filesystem;
using std::ofstream;
using std::string;

// A simple executable for debugging purposes.
int main() {
  string baseDir = "/home/martin/.config/REAPER/ccs/";
  //string pageFile = baseDir + "sessions/pcc/pages/page1.yml";
  //string sessionPath = baseDir + "sessions/pcc";
  auto ccs = new CCS::Ccs(baseDir);
  int aa = 1;
  delete ccs;
  return 0;
}