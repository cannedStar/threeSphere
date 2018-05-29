#include "alloutil/al_OmniApp.hpp"
#include "alloGLV/al_ControlGLV.hpp"
#include "GLV/glv.h"
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

struct HyperApp : OmniApp {
  cuttlebone::Maker<State> maker;
  State* state;

  double theta, phi, epsilon;

  Group group;

  GLVBinding gui;
  glv::Slider sTheta, sPhi, sEpsilon;
  glv::Table layout;

  HyperApp() :
    maker(Simulator::defaultBroadcastIP()) 
  {
    state = new State;
    memset(state, 0, sizeof(State));

    theta = 0;
    phi = 0;
    epsilon = 0;

    state->init();

    nav().pos(0.0, 0.0, 5.0);
    pose.set(nav());

    initWindow();

    lens().eyeSep(0.03).far(200);

    group.init();

    gui.bindTo(InputEventHandler::window());
    gui.style().color.set(glv::Color(0.7), 0.5);

    layout.arrangement("><");

    sTheta.interval(-M_PI, M_PI);
    sTheta.attachVariable(theta);
    layout << sTheta;
    layout << new glv::Label("theta");

    sPhi.interval(-M_PI, M_PI);
    sPhi.attachVariable(phi);
    layout << sPhi;
    layout << new glv::Label("phi");

    sEpsilon.interval(-M_PI, M_PI);
    sEpsilon.attachVariable(epsilon);
    layout << sEpsilon;
    layout << new glv::Label("epsilon");

    layout.arrange();

    gui << layout;
  }

  ~HyperApp() {}

  void onAnimate(double dt) {
    updateCamera();
    state->pose = nav();

    maker.set(*state);
  }

  void onDraw(Graphics& g) {

  }

  void updateCamera() {
    state->camera.setIdentity();
    GroupType& type = group.generators[state->activeGroup].type;
    if (type == GroupType::HYPERBOLIC) {
      state->camera = rotateTheta(state->camera, theta);
      state->camera = rotateEpsilon(state->camera, epsilon);
      state->camera = rotatePhi(state->camera, phi);
      // state->camera = para(state->camera, epsilon, phi);
    } else if (type == GroupType::SPHERICAL) {
      state->camera = rotate3s(state->camera, theta, phi);
    }
  }
  // KEYBOARD commands local
  virtual bool onKeyDown(const Keyboard& k){
    switch (k.key()) {
      case 'r': nav().pos(0.0, 0.0, 0.0); break;
      case 'f': theta = 0.0; epsilon = 0.0; phi = 0.0; break;
      case 'g': theta -= 0.1; break;
      case 't': theta += 0.1; break;
      case 'h': epsilon -= 0.1; break;
      case 'y': epsilon += 0.1; break;
      case 'j': phi -= 0.1; break;
      case 'u': phi += 0.1; break;
      case 'i': ++state->depth; break;
      case 'k': --state->depth; break;
      case 'p': state->uhsProj = !state->uhsProj; break;
      case '[': state->activeGroup -= 1; if (state->activeGroup < 0) state->activeGroup = group.size() - 1; cout << state->activeGroup << endl; break;
      case ']': state->activeGroup += 1; if (state->activeGroup >= group.size()) state->activeGroup = 0; cout << state->activeGroup << endl; break;
      case '1': state->activeGroup = 0; break;
      case '2': state->activeGroup = 1; break;
      case '3': state->activeGroup = 2; break;
      case '4': state->activeGroup = 3; break;
      case ',': state->meshSize -= 0.1; break;
      case '.': state->meshSize += 0.1; break;
      case 'o': state->showOrigin = !state->showOrigin; break;

      // case '1': omni().mode(OmniStereo::DUAL).stereo(true); break;
      // case '2': omni().mode(OmniStereo::ANAGLYPH).stereo(true); break;
      default: break;
    }

    updateCamera();

    return true;
  } // onKeyDown

  virtual bool onMouseDrag(const Mouse& m) {
    updateCamera();

    return true;
  }

};

int main(int argc, char* argv[]) {
  HyperApp hyperApp;
  hyperApp.maker.start();
  hyperApp.start();
  return 0;
}