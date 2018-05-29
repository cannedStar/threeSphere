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
  int changeTheta, changePhi;

  int joystickModifier;

  Group group;

  GLVBinding gui;
  glv::Slider sTheta, sPhi, sEpsilon;
  glv::Table layout;

  HyperApp() :
    maker(Simulator::defaultBroadcastIP()) 
  {
    mNavSpeed = 0.2;
    state = new State;
    memset(state, 0, sizeof(State));

    theta = 0;
    phi = 0;
    epsilon = 0;

    changeTheta = 0;
    changePhi = 0;

    joystickModifier = 0;

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
    if(changeTheta > 0) theta += 0.01;
    else if(changeTheta < 0) theta -= 0.01;

    if(changePhi > 0) phi += 0.01;
    else if(changePhi < 0) phi -= 0.01;

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
      case 'r': nav().home(); break;
      case 'f': theta = 0.0; epsilon = 0.0; phi = 0.0; break;
      case 'g': theta -= -0.1; break;
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

  virtual void onMessage(osc::Message& m) {
    float x;
    m.print();
    if (m.addressPattern() == "/mx") {
      m >> x;
      nav().moveR(-x * mNavSpeed);
    } else if (m.addressPattern() == "/my") {
      m >> x;
      nav().moveU(x * mNavSpeed);
    } else if (m.addressPattern() == "/mz") {
      m >> x;
      nav().spinR(x * mNavTurnSpeed);
    } else if (m.addressPattern() == "/tx") {
      m >> x;
      nav().moveF(-x * mNavSpeed);
    } else if (m.addressPattern() == "/ty") {
      m >> x;
      nav().spinU(x * mNavTurnSpeed);
    } else if (m.addressPattern() == "/tz") {
      m >> x;
      nav().spinF(x * mNavTurnSpeed);
    } else if (m.addressPattern() == "/b1") {
      m >> x;
      if (x == 1) {
        changeTheta = 1;
      } else {
        changeTheta = 0;
      }
    } else if (m.addressPattern() == "/b2") {
      m >> x;
      if (x == 1) {
        changeTheta = -1;
      } else {
        changeTheta = 0;
      }
    } else if (m.addressPattern() == "/b3") {
      m >> x;
      if (x == 1) {
        changePhi = -1;
      } else {
        changePhi = 0;
      }
    } else if (m.addressPattern() == "/b4") {
      m >> x;
      if (x == 1) {
        changePhi = 1;
      } else {
        changePhi = 0;
      }
    } else if (m.addressPattern() == "/b5") {
      m >> x;
      if (x == 1) {
        joystickModifier |= 1;
      } else {
        joystickModifier ^= 1;
      }
    } else if (m.addressPattern() == "/b6") {
      m >> x;
      if (x == 1) {
        if (joystickModifier == 0) {
          ++state->depth;
        } else if (joystickModifier == 1) {
          state->activeGroup += 1; if (state->activeGroup >= group.size()) state->activeGroup = 0;
        } else if (joystickModifier == 2) {
          state->meshSize += 0.1;
        }
      }
    } else if (m.addressPattern() == "/b7") {
      m >> x;
      if (x == 1) {
        joystickModifier |= 2;
      } else {
        joystickModifier ^= 2;
      }
    } else if (m.addressPattern() == "/b8") {
      m >> x;
      if (x == 1) {
        if (joystickModifier == 0) {
          --state->depth;
        } else if (joystickModifier == 1) {
          state->activeGroup -= 1; if (state->activeGroup < 0) state->activeGroup = group.size() - 1;
        } else if (joystickModifier == 2) {
          state->meshSize -= 0.1;
        }
      }
    } else if (m.addressPattern() == "/b9") {
      m >> x;
      if (x == 1) {
        nav().home();
        theta = 0;
      }
    } else if (m.addressPattern() == "/b10") {
      m >> x;
      if (x == 1) {
        state->showOrigin = !state->showOrigin;
      }
    } else {
      // m.print();
    }
  }

};

int main(int argc, char* argv[]) {
  HyperApp hyperApp;
  hyperApp.maker.start();
  hyperApp.start();
  return 0;
}