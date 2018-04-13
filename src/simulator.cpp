#include "alloutil/al_OmniStereoGraphicsRenderer.hpp"
#include "Cuttlebone/Cuttlebone.hpp"
#include "alloutil/al_Simulator.hpp"

#include <iostream>

#include "common.hpp"
#include "transformations.hpp"
#include "particles.hpp"
#include "mesh4D.hpp"
#include "generator.hpp"

using namespace al;
using namespace std;

struct HyperApp : OmniStereoGraphicsRenderer {
  cuttlebone::Maker<State> maker;
  State* state;

  Group group;


  HyperApp() :
    maker(Simulator::defaultBroadcastIP()) 
  {
    state = new State;
    memset(state, 0, sizeof(State));

    state->theta = 0;
    state->epsilon = 0;
    state->phi = 0;
    state->camera.setIdentity();

    nav().pos(0.0, 0.0, 5.0);

    initWindow();

    lens().eyeSep(0.03).far(200);

    group.init();

  }

  ~HyperApp() {}

  void onAnimate(double dt) {
    state->pose = nav();

    maker.set(*state);
  }

  void onDraw(Graphics& g) {

  }

  // KEYBOARD commands local
  virtual bool onKeyDown(const Keyboard& k){
    switch (k.key()) {
      case 'r': state->theta = 0.0; state->epsilon = 0.0; state->phi = 0.0; break;
      case 'g': state->theta -= 0.1; break;
      case 't': state->theta += 0.1; break;
      case 'h': state->epsilon -= 0.1; break;
      case 'y': state->epsilon += 0.1; break;
      case 'j': state->phi -= 0.1; break;
      case 'u': state->phi += 0.1; break;
      case 'i': ++state->depth; break;
      case 'k': --state->depth; break;
      case '1': state->activeGroup = 0; state->projType = 0; break;
      case '2': state->activeGroup = 0; state->projType = 1; break;
      case '3': state->activeGroup = 1; state->projType = 2; break;

      // case '1': omni().mode(OmniStereo::DUAL).stereo(true); break;
      // case '2': omni().mode(OmniStereo::ANAGLYPH).stereo(true); break;
      default: break;
    }

    state->camera.setIdentity();
    GroupType type = group.generators[state->activeGroup].type;
    if (type == GroupType::HYPERBOLIC) {
      state->camera = rotateTheta(state->camera, state->theta);
      state->camera = rotateEpsilon(state->camera, state->epsilon);
      state->camera = rotatePhi(state->camera, state->phi);
      // state->camera = para(state->camera, state->epsilon, state->phi);
    } else if (type == GroupType::SPHERICAL) {
      state->camera = rotate3s(state->camera, state->theta, state->phi);
    }

    return true;
  } // onKeyDown

};

int main(int argc, char* argv[]) {
  HyperApp().start();
  return 0;
}