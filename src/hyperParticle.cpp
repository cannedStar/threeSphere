// #include "alloutil/al_OmniStereoGraphicsRenderer.hpp"
#include "alloutil/al_OmniApp.hpp"

#include <iostream>

#include "transformations.hpp"
#include "particles.hpp"

using namespace al;
using namespace std;

// struct HyperApp : OmniStereoGraphicsRenderer {
struct HyperApp : OmniApp {
  Material material;
  Light light;

  Mesh origin;

  Emitter<8000> em;

  double s_vel, t_vel;

  HyperApp() {
    nav().pos(0.0, 0.0, 5.0);

    // cout << "lightID = " << light.id() << endl;
    light.pos(0, 10.0, 0);
    light.specular(Color(0.0,0.0,0.5));
    light.diffuse(Color(0.0,0.6,0.0));
    light.ambient(Color(0.5,0.5,0.5));

    initWindow();

    lens().eyeSep(0.03).far(200);

    addSphere(origin, 0.05, 4, 4);

    s_vel = 0.3;
    t_vel = 0.2;
    // origin.generateNormals();
    // origin.invertNormals();
  }

  ~HyperApp() {}

  void onAnimate(double dt) {
    pose.set(nav());
    em.update<20>(dt, s_vel, t_vel);
  }

  void onDraw(Graphics& g) {
    material();
    light();

    g.pushMatrix();  
      g.depthTesting(true);
      g.blending(true);
      g.blendModeTrans();
      
      
      for(int i = 0; i < em.size(); ++i) {
        g.pushMatrix();
          
          Particle& p = em.particles[i];
          // cout << klein(p.pos) << endl;
          g.color(HSV(0.2,1,1));
          g.translate(uhs(klein(p.pos)));
          // g.translate(klein(p.pos));
          g.draw(origin);

        g.popMatrix();
      }
    g.popMatrix();

    // Mesh& m = g.mesh();
    // m.reset();
    // m.primitive(g.POINTS);
    // g.pointSize(5.f);

    // for (int i = 0; i < em.size(); ++i) {
    //   Particle& p = em.particles[i];
    //   m.vertex(klein(p.pos));
    // }

    // g.draw(m);
  }

  virtual bool onKeyDown(const Keyboard& k){
    switch (k.key()) {
      case 'r': s_vel = 0.3; t_vel = 0.2; break;
      case 'g': s_vel -= 0.1; break;
      case 't': s_vel += 0.1; break;
      case 'h': t_vel -= 0.1; break;
      case 'y': t_vel += 0.1; break;

      default: break;
    }

    return true;
  }

  
}; // struct HyperApp

int main(int argc, char* argv[]) {
  HyperApp().start();
  return 0;
}