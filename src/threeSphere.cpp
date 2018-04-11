/*
4D Polytopes

[UCSB]
Steve Trettel
Dennis Adderton
Kenny Kim
*/

#include "al_OmniApp.hpp"
#include <iostream>

#include "poly.hpp"
#include "hopf.hpp"

using namespace al;
using namespace std;

struct HyperApp : OmniApp {

  Material material;
  Light light;

  float theta, phi, epsilon;
  int x_hopf, y_hopf, a_hopf, b_hopf;

  Poly poly, poly2;
  Hopf hopf;

  int showState; // 0 poly 1 dual 2 hopf

  Mat4f camera;
  Mat4f eye;

  // CONSTRUCTOR
  HyperApp() {
    nav().pos(0.0, 0.0, 0.0);
    light.pos(0, 10.0, 0);
    light.specular(Color(0.1,0.1,0.1));
    light.diffuse(Color(0.2,0.2,0.2));
    light.ambient(Color(1,1,1));
    initWindow();
    initAudio();

    lens().eyeSep(0.02).near(0.1).far(200); // set eyeSep to zero

    theta = 0.f;
    phi = 0.f;
    epsilon = 0.f;

    x_hopf = 2;
    y_hopf = 2;
    a_hopf = 1;
    b_hopf = 1;

    showState = 0;

    camera = Mat4f(
      cos(theta), -sin(theta), 0.f, 0.f,
      sin(theta), cos(theta), 0.f, 0.f,
      0.f, 0.f, cos(theta+phi), -sin(theta+phi),
      0.f, 0.f, sin(theta+phi), cos(theta+phi));

    eye = Mat4f(
      cos(epsilon), 0.f, 0.f, -sin(epsilon),
      0.f, 1.f, 0.f, 0.f,
      0.f, 0.f, 1.f, 0.f,
      sin(epsilon), 0.f, 0.f, cos(epsilon));

    poly.init();
    poly.set16();

    poly.generateMesh(camera, eye, 0);

    poly2.init();
    poly2.setHypercube();
    poly2.generateMesh(camera, eye, 2);

    hopf.init();
    hopf.setHopf(x_hopf, y_hopf, a_hopf, b_hopf);
    hopf.generateMesh(camera, eye);
  } // HyperApp()
  
  virtual ~HyperApp() {}    // what does this do?

  // add in rotation functions

  // ANIMATE
  virtual void onAnimate(double dt) {
    if(showState == 0) {
      if(poly.dirty && !poly.busy) poly.generateMeshT(camera, eye, 0);
    } else if (showState == 1) {
      if(poly.dirty && !poly.busy) poly.generateMeshT(camera, eye, 1);
      if(poly2.dirty && !poly2.busy) poly2.generateMeshT(camera, eye, 2);
    } else if (showState == 2) {
      if(hopf.dirty && !hopf.busy) hopf.generateMeshT(camera, eye);
    }
  }
    
  // DRAW 
  virtual void onDraw(Graphics& g) {
    material(); // material ...
    light();    // light ...
      
    g.pushMatrix();  
      g.depthTesting(true);
      g.blending(true);
      g.blendModeTrans();
      g.pointSize(6);
      g.lineWidth(5);
      
      if(showState == 0) poly.draw(g, omni().currentEye());
      else if(showState == 1) {
        poly.draw(g, omni().currentEye());
        poly2.draw(g, omni().currentEye());
      } else if(showState == 2) {
        hopf.draw(g, omni().currentEye());
      }
    g.popMatrix();
  } // onDraw

  void update() {
    if (showState == 2) {
      camera = Mat4f(cos(theta), 0.f, 0.f, -sin(theta),
                     0.f, cos(theta+phi), -sin(theta+phi), 0.f,
                     0.f, sin(theta+phi), cos(theta+phi), 0.f,
                     sin(theta), 0.f, 0.f, cos(theta));
    } else {
      camera = Mat4f(cos(theta), -sin(theta), 0.f, 0.f,
                     sin(theta), cos(theta), 0.f, 0.f,
                     0.f, 0.f, cos(theta+phi), -sin(theta+phi),
                     0.f, 0.f, sin(theta+phi), cos(theta+phi));
    }

    eye = Mat4f(cos(epsilon), 0.f, 0.f, -sin(epsilon),
                0.f, 1.f, 0.f, 0.f,
                0.f, 0.f, 1.f, 0.f,
                sin(epsilon), 0.f, 0.f, cos(epsilon));

    poly.dirty = true;
    poly2.dirty = true;
    hopf.dirty = true;
  }
  
  // MESSAGE FROM MAX
  virtual void onMessage(osc::Message& m) {
    std::string param;
    float w, x, y, z;
    int i;

    if (m.addressPattern() == "/as_view_quat") {
      m >> x >> y >> z >> w;
      nav().quat().set(x, y, z, w);
      nav().quat() *= Quatd().fromAxisY(M_PI);
    } else if (m.addressPattern() == "/as_view_pos") {
      m >> x >> y >> z;
      nav().pos().set(x, y, z);
    } else if (m.addressPattern() == "/as_key") {
      m >> i; 
      printf("OSC /as_key %d\n", i);
      switch(i) {
        case 'g': theta -= 0.005f; update(); break;
        case 't': theta += 0.005f; update(); break;
        case 'h': phi -= 0.005f; update(); break;
        case 'y': phi += 0.005f; update(); break;
        case '[': epsilon -= 0.01f; update(); break;
        case ']': epsilon += 0.01f; update(); break;
        case '\\': epsilon = 0.f; update(); break;
        case 'r': theta = 0.f; phi = 0.f; update(); break;
        case '1': showState = 0; poly.setHypercube(); update(); break;
        case '2': showState = 0; poly.set16(); update(); break;
        case '3': showState = 0; poly.set24(); update(); break;
        case '4': showState = 0; poly.set120(); update(); break;
        case '5': showState = 0; poly.set600(); update(); break;
        case '6': showState = 0; poly.set5(); update(); break;
        case '9': showState = 1; poly2.setHypercube(); poly.set16(); poly.showSphere = false; poly2.showSphere = false; update(); break;
        case '0': showState = 1; poly2.set600(); poly.set120(); poly.showSphere = false; poly2.showSphere = false; update(); break;
        case '-': showState = 2; a_hopf = 1; b_hopf = 1; hopf.setHopf(x_hopf, y_hopf, a_hopf, b_hopf); update(); break;
        case '=': showState = 2; a_hopf = 2; b_hopf = 3; hopf.setHopf(x_hopf, y_hopf, a_hopf, b_hopf); update(); break;
        case 'j': x_hopf++; hopf.setHopf(x_hopf, y_hopf, a_hopf, b_hopf); update(); break;
        case 'm': x_hopf--; hopf.setHopf(x_hopf, y_hopf, a_hopf, b_hopf); update(); break;
        case 'k': y_hopf++; hopf.setHopf(x_hopf, y_hopf, a_hopf, b_hopf); update(); break;
        case ',': y_hopf--; hopf.setHopf(x_hopf, y_hopf, a_hopf, b_hopf); update(); break;
        case 'l': a_hopf++; hopf.setHopf(x_hopf, y_hopf, a_hopf, b_hopf); update(); break;
        case '.': a_hopf--; hopf.setHopf(x_hopf, y_hopf, a_hopf, b_hopf); update(); break;
        case ';': b_hopf++; hopf.setHopf(x_hopf, y_hopf, a_hopf, b_hopf); update(); break;
        case '/': b_hopf--; hopf.setHopf(x_hopf, y_hopf, a_hopf, b_hopf); update(); break;
        case 'p': poly.showSphere = !poly.showSphere; poly2.showSphere = !poly2.showSphere; update(); break;
        default: break;
      }
    } else { m.print(); } // as_key
  } // onMessage
  
  // KEYBOARD commands local
  virtual bool onKeyDown(const Keyboard& k){
    switch (k.key()) {
      // case '1': omni().mode(OmniStereo::DUAL).stereo(true); break;
      // case '2': omni().mode(OmniStereo::ANAGLYPH).stereo(true); break;

      case 'g': theta -= 0.05f; update(); break;
      case 't': theta += 0.05f; update(); break;
      case 'h': phi -= 0.05f; update(); break;
      case 'y': phi += 0.05f; update(); break;
      case '[': epsilon -= 0.01f; update(); break;
      case ']': epsilon += 0.01f; update(); break;
      case '\\': epsilon = 0.f; update(); break;
      case 'r': theta = 0.f; phi = 0.f; update(); break;
      case '1': showState = 0; poly.setHypercube(); update(); break;
      case '2': showState = 0; poly.set16(); update(); break;
      case '3': showState = 0; poly.set24(); update(); break;
      case '4': showState = 0; poly.set120(); update(); break;
      case '5': showState = 0; poly.set600(); update(); break;
      case '6': showState = 0; poly.set5(); update(); break;
      case '9': showState = 1; poly2.setHypercube(); poly.set16(); poly.showSphere = false; poly2.showSphere = false; update(); break;
      case '0': showState = 1; poly2.set600(); poly.set120(); poly.showSphere = false; poly2.showSphere = false; update(); break;
      case '-': showState = 2; a_hopf = 1; b_hopf = 1; hopf.setHopf(x_hopf, y_hopf, a_hopf, b_hopf); update(); break;
      case '=': showState = 2; a_hopf = 2; b_hopf = 3; hopf.setHopf(x_hopf, y_hopf, a_hopf, b_hopf); update(); break;
      case 'j': x_hopf++; hopf.setHopf(x_hopf, y_hopf, a_hopf, b_hopf); update(); break;
      case 'm': x_hopf--; hopf.setHopf(x_hopf, y_hopf, a_hopf, b_hopf); update(); break;
      case 'k': y_hopf++; hopf.setHopf(x_hopf, y_hopf, a_hopf, b_hopf); update(); break;
      case ',': y_hopf--; hopf.setHopf(x_hopf, y_hopf, a_hopf, b_hopf); update(); break;
      case 'l': a_hopf++; hopf.setHopf(x_hopf, y_hopf, a_hopf, b_hopf); update(); break;
      case '.': a_hopf--; hopf.setHopf(x_hopf, y_hopf, a_hopf, b_hopf); update(); break;
      case ';': b_hopf++; hopf.setHopf(x_hopf, y_hopf, a_hopf, b_hopf); update(); break;
      case '/': b_hopf--; hopf.setHopf(x_hopf, y_hopf, a_hopf, b_hopf); update(); break;
      case 'p': poly.showSphere = !poly.showSphere; poly2.showSphere = !poly2.showSphere; update(); break;
      default: break;
    }
    return true;
  } // onKeyDown
  
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

}; // struct HyperApp

int main(int argc, char* argv[]) {
  HyperApp().start();
//  START
  return 0;
}
