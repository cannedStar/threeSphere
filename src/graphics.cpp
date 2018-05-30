#include "alloutil/al_OmniStereoGraphicsRenderer.hpp"
#include "Cuttlebone/Cuttlebone.hpp"
#include "allocore/graphics/al_Asset.hpp"

#include <iostream>

#include "common.hpp"
#include "transformations.hpp"
#include "particles.hpp"
#include "obj4D.hpp"
#include "mesh4D.hpp"
#include "graph4D.hpp"
#include "generator.hpp"

using namespace al;
using namespace std;

struct HyperApp : OmniStereoGraphicsRenderer {

  Material material;
  Light light;

  cuttlebone::Taker<State> taker;
  State* state;

  Scene* ascene = 0;
  Vec3f scene_center;
  float scene_scaleInv;

  Obj4D obj4D;
  Graph4D graph4D;

  Texture tex;

  Group group;

  HyperApp() {
    state = new State;
    state->init();

    nav().pos(0.0, 0.0, 5.0);
    pose.set(nav());

    initWindow();

    lens().eyeSep(0.03).far(200);

    SearchPaths sPath;
    sPath.addAppPaths();

    // Image img(sPath.find("hubble.jpg").filepath());
    Image img(sPath.find("hue.png").filepath());
    tex.allocate(img.array());

    group.init();

    FilePath path = sPath.find("ducky.obj");
    printf("reading %s\n", path.filepath().c_str());

    ascene = Scene::import(path.filepath());

    Vec3f scene_min, scene_max;
    if(ascene==0) {
      printf("error reading %s\n", path.filepath().c_str());
    } else {
      ascene->getBounds(scene_min,scene_max);
      scene_center = (scene_min + scene_max) / 2.f;
      // ascene->dump();
    }

    scene_scaleInv = scene_max[0]-scene_min[0];
    scene_scaleInv = al::max(scene_max[1] - scene_min[1],scene_scaleInv);
    scene_scaleInv = al::max(scene_max[2] - scene_min[2],scene_scaleInv);
    scene_scaleInv = 2.f / scene_scaleInv;

    obj4D.load(ascene, scene_center, scene_scaleInv);
    
  }

  ~HyperApp() {}

  void onAnimate(double dt) {
    // pose.set(nav());

    int popCount = taker.get(*state);
    
    pose.set(state->pose);
  }

  void onDraw(Graphics& g) {
    material();
    light();

    g.pushMatrix();
      Generator& gen = group.generators[state->activeGroup];

      if(state->showGraph != 2) {
        shader().uniform("texture0", 1);
        shader().uniform("texture", 1.0);
        tex.bind(1);

        for (int i = state->showOrigin? 0 : 1; i < gen.size(); ++i) {
          if (gen.getDepth(i) > state->depth) break;

          Mat4d trans = state->camera * gen.get(i);
          obj4D.update(trans, gen.type, state->meshSize, state->uhsProj);

          // if(!busy)
          //   obj4D.renderT(g, trans, gen.type, state->meshSize, state->uhsProj);
          obj4D.draw(g);
        }

        tex.unbind(1);
        shader().uniform("texture", 0.0);
      }

      if(state->showGraph != 0) {
        graph4D.update(state->camera, gen, state->depth, 10, state->uhsProj);
        // g.lineWidth(2.f);
        for(int i = 0; i < graph4D.size(); ++i) {
          g.draw(graph4D.meshes[i]);
        }
        // g.lineWidth(1.f);
      }

    g.popMatrix();

  }

  std::string fragmentCode() {
    return AL_STRINGIFY(
      uniform float lighting;
      uniform float texture;
      uniform sampler2D texture0;
      varying vec4 color;
      varying vec3 normal, lightDir, eyeVec;

      void main() {
        vec4 colorMixed;
        vec4 textureColor = texture2D(texture0, gl_TexCoord[0].st);
        if( texture > 0.0){ colorMixed = mix(color, textureColor, texture);} 
        else {colorMixed = color;}
        vec4 final_color = colorMixed * gl_LightSource[7].ambient;
        vec3 N = normalize(normal);
        vec3 L = lightDir;
        float lambertTerm = max(dot(N, L), 0.0);
        final_color += gl_LightSource[7].diffuse * colorMixed * lambertTerm;
        vec3 E = eyeVec;
        vec3 R = reflect(-L, N);
        float spec = pow(max(dot(R, E), 0.0), 0.9 + 1e-20);
        final_color += gl_LightSource[7].specular * spec;
        gl_FragColor = mix(colorMixed, final_color, lighting);
        if (texture > 0.0) gl_FragColor.a = textureColor.r; // sets alpha to 0 for background
      }
    );
  } // fragmentCode

  void start() {
    taker.start();
    OmniStereoGraphicsRenderer::start();
  }
};

int main() {
  HyperApp().start();
  return 0;
}