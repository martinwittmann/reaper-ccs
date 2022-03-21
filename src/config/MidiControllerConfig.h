#ifndef MIDI_CONTROLLER_CONFIG_H
#define MIDI_CONTROLLER_CONFIG_H

#include <string>
#include <vector>
#include "BaseConfig.h"

namespace CCS {

  class MidiControllerConfig : public BaseConfig {
  public:
    explicit MidiControllerConfig(std::string baseDir);
  };
}

#endif