#include "reaper/ReaperEventSubscriber.h"
#include <iostream>

namespace CCS {

  class Test : public ReaperEventSubscriber {

    void onFxParameterChanged(
      MediaTrack *track,
      int fxId,
      int paramId,
      double value
    ) {
      std::cout << "parameter changed!\n";
    }
  };

}