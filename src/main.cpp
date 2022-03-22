#include "Ccs.h"
#include "Util.h"
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
  //string test = "\"[set_screen_layout:$SCREEN1:$LAYOUT_KNOB]\"";
  //string result = CCS::Util::regexReplace(test, "^[\"\\[]+", "");
  //result = CCS::Util::regexReplace(result, "[\"\\]]+$", "");
  int aa = 1;
  delete ccs;
  return 0;
}