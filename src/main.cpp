//#include "CcsConfig.cpp"
#include "CcsPage.cpp"
#include <fstream>

namespace fse = std::experimental::filesystem;
using std::ofstream;

// A simple executable for debugging purposes.
int main() {
  string baseDir = "/home/martin/.config/REAPER/ccs/";
  string pageFile = baseDir + "sessions/pcc/pages/page1.yml";
  CcsPage* page = new CcsPage(pageFile);
  int aa = 1;
  delete page;
  return 0;
}