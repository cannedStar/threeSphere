#include "alloutil/al_OmniStereoGraphicsRenderer.hpp"
#include "Cuttlebone/Cuttlebone.hpp"
#include "alloutil/al_Simulator.hpp"

#include <iostream>

#include "common.hpp"

using namespace al;
using namespace std;

struct HyperApp : OmniStereoGraphicsRenderer {
  cuttlebone::Maker<State> maker;
  State* state;

  HyperApp() {
    nav().pos(0.0, 0.0, 5.0);

    initWindow();

    lens().eyeSep(0.03).far(200);
  }

  ~HyperApp() {}

  void onAnimate(double dt) {

  }

  void onDraw(Graphics& g) {

  }

};

int main(int argc, char* argv[]) {
  HyperApp().start();
  return 0;
}