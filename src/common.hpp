#ifndef INCLUDE_COMMON_HPP
#define INCLUDE_COMMON_HPP

#include "allocore/al_Allocore.hpp"

using namespace al;
using namespace std;

struct State {
  Pose pose;
  double time;
  
  float epsilon;
  float mandel_power;

  int maxIterations;
  int maxRaySteps;
  int multisample;
  
  float minRadius2;
  float fixedRadius2;
  float foldingLimit;
  float scale;
  float time_mult;
  float d_factor;
  float d_julia;
  float d_rotate;
  
  bool toggleAA;
  bool toggleBox;
  bool toggleSphere;

  void set(const State* _s) {
    epsilon = _s->epsilon;
    mandel_power = _s->mandel_power;
    maxIterations = _s->maxIterations;
    maxRaySteps = _s->maxRaySteps;
    multisample = _s->multisample;
    minRadius2 = _s->minRadius2;
    fixedRadius2 = _s->fixedRadius2;
    foldingLimit = _s->foldingLimit;
    scale = _s->scale;
    time_mult = _s->time_mult;
    d_factor = _s->d_factor;
    d_julia = _s->d_julia;
    d_rotate = _s->d_rotate;
    toggleAA = _s->toggleAA;
    toggleBox = _s->toggleBox;
    toggleSphere = _s->toggleSphere;
  }

  void setToggle(const State* _s) {
    toggleAA = _s->toggleAA;
    toggleBox = _s->toggleBox;
    toggleSphere = _s->toggleSphere;
  }

  void init() {
    pose.pos().set(0,0.1,3.0);
    pose.quat().setIdentity();

    epsilon = 1e-3;
    mandel_power = 1.0;

    maxIterations = 10;
    maxRaySteps = 120;
    multisample = 0;
    
    minRadius2 = 0.0;
    fixedRadius2 = 1.0;
    foldingLimit = 1.0;
    scale = 1.0;
    time_mult = 1.0;
    d_factor = 0.0;
    d_julia = 0.0;
    d_rotate = 1.0;
    
    toggleAA = false;
    toggleBox = false;
    toggleSphere = false;
  }

  void resetToggle() {
    toggleAA = false;
    toggleBox = false;
    toggleSphere = false;
  }

  void print() {
    printf("---state---\n");
    pose.print();
    printf("time: %f\n", time);
    printf("epsilon: %f\n", epsilon);
    printf("mandel_power: %f\n", mandel_power);
    printf("maxIter: %d\n", maxIterations);
    printf("maxRaySteps: %d\n", maxRaySteps);
    printf("multisample: %d\n", multisample);
    printf("toggleAA: %d\n", toggleAA);
    printf("minRadius2: %f\n", minRadius2);
    printf("fixedRadius2: %f\n", fixedRadius2);
    printf("foldingLimit: %f\n", foldingLimit);
    printf("scale: %f\n", scale);
    printf("time_mult: %f\n", time_mult);
    printf("d_factor: %f\n", d_factor);
    printf("d_julia: %f\n", d_julia);
    printf("d_rotate: %f\n", d_rotate);
    printf("toggleBox: %d\n", toggleBox);
    printf("toggleSphere: %d\n", toggleSphere);
    printf("==endstate==\n");
  }
};

#endif
