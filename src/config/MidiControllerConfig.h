#ifndef MIDI_CONTROLLER_CONFIG_H
#define MIDI_CONTROLLER_CONFIG_H

#include <string>
#include <vector>
#include "BaseConfig.h"

namespace CCS {

  class MidiControllerConfig : public BaseConfig {
  public:
    MidiControllerConfig(std::string baseDir) : BaseConfig(
      baseDir,
      true,
      "extends"
    ) {
      replaceVariables();
    }
  };
}

#endif