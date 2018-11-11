#pragma once
#include <opencv2/opencv.hpp>
#include <stdint.h>
#include "SeamCarver.h"


namespace ct {

  class SeamCarverKeepout : public SeamCarver {
  public:
    SeamCarverKeepout(double margin_energy) : SeamCarver(margin_energy) {}
    SeamCarverKeepout() {}
  protected:
  };
 
}
