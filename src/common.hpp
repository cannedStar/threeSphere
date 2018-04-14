#ifndef INCLUDE_COMMON_HPP
#define INCLUDE_COMMON_HPP

#include "allocore/al_Allocore.hpp"

using namespace al;
using namespace std;

struct State {
  Pose pose;
  Mat4d camera;
  int depth;
  int activeGroup;
  int projType;

  State() {
    pose.pos(0.0, 0.0, 5.0);
    camera.setIdentity();
    depth = 0;
    activeGroup = 0;
    projType = 0;
  }
};

#endif
