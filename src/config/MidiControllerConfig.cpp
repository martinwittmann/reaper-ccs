#include <string>
#include <vector>
#include "MidiControllerConfig.h"

namespace CCS {

  MidiControllerConfig::MidiControllerConfig(std::string baseDir) : BaseConfig(
    baseDir,
    true,
    "extends"
  ) {
    replaceVariables();
  }
}