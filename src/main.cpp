#define REAPERAPI_WANT_DB2SLIDER

#include "../WDL/db2val.h"
#include "reaper/reaper_plugin_functions.h"
#include "reaper/reaper_plugin.h"
#include <string>
#include <iostream>

using std::ofstream;
using std::string;

// A simple executable for debugging purposes.
int main() {
  double ss = 10 ^ (0 / 20);
  std::cout << std::to_string(ss) << "\n";
  ss = 10 ^ (1 / 20);
  std::cout << std::to_string(ss) << "\n";

  return 0;
}