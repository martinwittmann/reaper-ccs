//#include "CcsConfig.cpp"
//#include "CcsPage.cpp"
#include "CcsSession.cpp"
#include <fstream>

namespace fse = std::experimental::filesystem;
using std::ofstream;

// A simple executable for debugging purposes.
int main() {
  string baseDir = "/home/martin/.config/REAPER/ccs/";
  string pageFile = baseDir + "sessions/pcc/pages/page1.yml";
  string sessionPath = baseDir + "sessions/pcc";
  //auto session = new CcsSession(sessionPath);
  //auto page = new CcsPage(pageFile);
  int aa = 1;
  //delete session;
  return 0;
}