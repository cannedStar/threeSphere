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

using namespace al;
using namespace std;

struct HyperApp : OmniApp {

  Material material;
  Light light;

  float theta, phi, epsilon;

  Poly poly, poly2;

  bool showDual;

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

    lens().eyeSep(0.03).near(0.1).far(100); // set eyeSep to zero

    theta = 0.f;
    phi = 0.f;
    epsilon = 0.f;

    showDual = false;

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
    poly.setHypercube();

    poly.generateMesh(camera, eye);

    // poly2.init();
    // poly2.setHypercube();
  } // HyperApp()
  
  virtual ~HyperApp() {}    // what does this do?

  // add in rotation functions

  // ANIMATE
  virtual void onAnimate(double dt) {   
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
      
      if(!showDual) {
        poly.generateMesh(camera, eye);
        poly.draw(g, omni().currentEye());
      } else {
        poly.generateMesh(camera, eye, 1);
        poly2.generateMesh(camera, eye, 2);
        poly.draw(g, omni().currentEye());
        poly2.draw(g, omni().currentEye());
      }
    g.popMatrix();
  } // onDraw
  
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
        case 'g': theta -= 0.005f; break;
        case 't': theta += 0.005f; break;
        case 'h': phi -= 0.005f; break;
        case 'y': phi += 0.005f; break;
        case '[': epsilon -= 0.01f; break;
        case ']': epsilon += 0.01f; break;
        case '1': poly.setHypercube(); break;
        case '2': poly.set16(); break;
        case '3': poly.set24(); break;
        case '4': poly.set120(); break;
        case '5': poly.set600(); break;
        case '0': showDual = !showDual; break;
        default: break;
      }
      camera = Mat4f(cos(theta), -sin(theta), 0.f, 0.f,
                     sin(theta), cos(theta), 0.f, 0.f,
                     0.f, 0.f, cos(theta+phi), -sin(theta+phi),
                     0.f, 0.f, sin(theta+phi), cos(theta+phi));

      eye = Mat4f(cos(epsilon), 0.f, 0.f, -sin(epsilon),
                  0.f, 1.f, 0.f, 0.f,
                  0.f, 0.f, 1.f, 0.f,
                  sin(epsilon), 0.f, 0.f, cos(epsilon));
    } else { m.print(); } // as_key
  } // onMessage
  
  // KEYBOARD commands local
  virtual bool onKeyDown(const Keyboard& k){
    switch (k.key()) {
      case 'g': theta -= 0.1f; break;
      case 't': theta += 0.1f; break;
      case 'h': phi -= 0.1f; break;
      case 'y': phi += 0.1f; break;
      case '[': epsilon -= 0.01f; break;
      case ']': epsilon += 0.01f; break;

      // case '1': omni().mode(OmniStereo::DUAL).stereo(true); break;
      // case '2': omni().mode(OmniStereo::ANAGLYPH).stereo(true); break;
      case '1': poly.setHypercube(); break;
      case '2': poly.set16(); break;
      case '3': poly.set24(); break;
      case '4': poly.set120(); break;
      case '5': poly.set600(); break;
      case '0': showDual = !showDual; break;
      default: break;
    }

    camera = Mat4f(cos(theta), -sin(theta), 0.f, 0.f,
                   sin(theta), cos(theta), 0.f, 0.f,
                   0.f, 0.f, cos(theta+phi), -sin(theta+phi),
                   0.f, 0.f, sin(theta+phi), cos(theta+phi));

    eye = Mat4f(cos(epsilon), 0.f, 0.f, -sin(epsilon),
                0.f, 1.f, 0.f, 0.f,
                0.f, 0.f, 1.f, 0.f,
                sin(epsilon), 0.f, 0.f, cos(epsilon));

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
        vec4 final_color = colorMixed * gl_LightSource[0].ambient;
        vec3 N = normalize(normal);
        vec3 L = lightDir;
        float lambertTerm = max(dot(N, L), 0.0);
        final_color += gl_LightSource[0].diffuse * colorMixed * lambertTerm;
        vec3 E = eyeVec;
        vec3 R = reflect(-L, N);
        float spec = pow(max(dot(R, E), 0.0), 0.9 + 1e-20);
        final_color += gl_LightSource[0].specular * spec;
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
