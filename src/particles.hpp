#ifndef INCLUDE_PARTICLES_HPP
#define INCLUDE_PARTICLES_HPP

#include <iostream>

#include "allocore/al_Allocore.hpp"
#include "transformations.hpp"

using namespace al;
using namespace std;

struct Particle {
  Particle(): age(0){}

  void init() {
    pos[1] = rnd::uniformS(1.0);
    pos[2] = rnd::uniformS(1.0);
    pos[3] = -0.9f;
    pos[1] = pos[1]*pos[1];
    pos[2] = pos[2]*pos[2];
    pos[0] = sqrt(pos[1]*pos[1] + pos[2]*pos[2] + pos[3]*pos[3] + 1.0);

    age = 0;
  }

  void update(double& ageInc, double& s, double& t) {
    pos = loxodromic(pos, s*ageInc, t*ageInc, 10.0);
    age += ageInc;
  }

  Vec4d pos;
  double age;
};

template <int N>
struct Emitter {
  Emitter(): tap(0) {
    for(int i = 0; i < N; ++i) {
      Particle& p = particles[i];

      p.init();
    }
  }

  template <int M>
  void update(double ageInc, double& s, double& t) {
    for(int i = 0; i < N; ++i) {
      particles[i].update(ageInc, s, t);
    }

    for(int i = 0; i < M; ++i) {
      // if(i==0) cout << particles[tap].pos[0] << endl;
      particles[tap].init();

      ++tap;
      if(tap >= N) tap = 0;
    }
  }

  int size() { return N; }

  Particle particles[N];
  int tap;
};

#endif