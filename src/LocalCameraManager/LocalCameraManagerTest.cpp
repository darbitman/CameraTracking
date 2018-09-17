#include "LocalCameraManager.h"
#include <iostream>

using namespace std;

int main() {
  ct::LocalCameraManager lc;
  cout << lc.getCameraCount();
  return 0;
}