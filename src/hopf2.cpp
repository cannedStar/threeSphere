// Steve Trettel's Hypercube Projection
//
// Dennis Adderton
// Kenny Kim
/*
*/


#include "al_OmniApp.hpp"
#include "allocore/io/al_App.hpp"
#include <iostream>
using namespace al;
using namespace std;

static const int vertNum = 16;
static const int edgeNum = 120;
static const int edgeRes = 512;

struct HyperApp : OmniApp {

  Material material;
  Light light;

  std::vector<Vec4f> s3Edge[edgeNum];
  std::vector<Mesh> leftMesh, rightMesh;

  float theta, phi;
  float epsilon;
  Mat4f camera;
  Mat4f eye;

  void generateEdge(std::vector<Vec4f>& tgtEdge, int v1, int v2) {
    
    for (int i = 0; i < edgeRes; ++i) {
      float t = 2.f * M_PI * (float)i / ((float)edgeRes - 1.f);

      float R_ = powf((float)v1 / 5.f, 2);
      float T_ = 2.f * M_PI * (float)v2 / (float)(vertNum - 1);

      Vec4f newPoint = Vec4f(
        R_ / sqrt(R_*R_ + 1.f) * cos(T_ + 2*t),
        R_ / sqrt(R_*R_ + 1.f) * sin(T_ + 2*t),
        1.f / sqrt(R_*R_ + 1.f) * cos(3*t),
        1.f / sqrt(R_*R_ + 1.f) * sin(3*t)
        );

      // projection onto S3
      tgtEdge[i] = newPoint.normalized();
    }
  }

  void generateMesh(Mesh& tgtMesh, const std::vector<Vec4f>& srcVt, HSV meshColor, const bool isRight = false) {
    tgtMesh.reset();
    tgtMesh.primitive(Graphics::LINE_STRIP);

    for (int i = 0; i < srcVt.size(); ++i) {
      Vec4f postRotVt = srcVt[i];
      Mat4f::multiply(postRotVt, camera, srcVt[i]);
      if (isRight)
        Mat4f::multiply(postRotVt, eye, postRotVt);
      
      // projection onto R3
      Vec3f newVt = Vec3f(
        postRotVt[1] / (1.f - postRotVt[0]),
        postRotVt[2] / (1.f - postRotVt[0]),
        postRotVt[3] / (1.f - postRotVt[0])
        );

      tgtMesh.vertex(newVt);
      tgtMesh.color(meshColor); 
    }
  }

  // add in rotation functions

  // CONSTRUCTOR
  HyperApp() {

    nav().pos(0.0, 0.0, 0.0);
    light.pos(0, 10.0, 0);
    light.specular(Color(0.3,0.3,0.3));
    light.diffuse(Color(0.2,0.2,0.2));
    light.ambient(Color(1,1,1));
    initWindow();
    initAudio();

    lens().eyeSep(0.02).far(200); // set eyeSep to zero

    theta = 0.f;
    phi = 0.f;
    camera = Mat4f(
      cos(theta), 0.f, 0.f, -sin(theta),
      0.f, cos(theta+phi), -sin(theta+phi), 0.f,
      0.f, sin(theta+phi), cos(theta+phi), 0.f,
      sin(theta), 0.f, 0.f, cos(theta));

    epsilon = 0.00f;
    eye = Mat4f(
      cos(epsilon), 0.f, 0.f, -sin(epsilon),
      0.f, 1.f, 0.f, 0.f,
      0.f, 0.f, 1.f, 0.f,
      sin(epsilon), 0.f, 0.f, cos(epsilon));

    for (int i = 0; i < edgeNum; ++i)
      s3Edge[i].resize(edgeRes);

    int k = 0;
    for (int i = 0; i < vertNum; ++i) {
      for (int j = i + 1; j < vertNum; ++j) {
        generateEdge(s3Edge[k++], i, j);
      }
    }

    cout << vertNum << " vertices -> " << k << " edges" << endl;
    if (k != edgeNum) {
      cout << "Error: Predefined Edge Number(" << edgeNum << ") didn't match generated Edge Number(" << k << ")!" << endl;
    }

    leftMesh.resize(edgeNum);
    rightMesh.resize(edgeNum);

    for(int i = 0; i < edgeNum; ++i) {
      generateMesh(leftMesh[i], s3Edge[i], HSV((float)i / (float)edgeNum, 1.f, 1.f), false);
      generateMesh(rightMesh[i], s3Edge[i], HSV((float)i / (float)edgeNum, 1.f, 1.f), true);
    }
  } // HyperApp()
  
  virtual ~HyperApp() {}    // what does this do?

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
      g.lineWidth(10);
      
      for(int i = 0; i < edgeNum; ++i) {
        generateMesh(leftMesh[i], s3Edge[i], HSV((float)i / (float)edgeNum, 1.f, 1.f), false);
        generateMesh(rightMesh[i], s3Edge[i], HSV((float)i / (float)edgeNum, 1.f, 1.f), true);
        
        if (omni().currentEye() == 0) g.draw(leftMesh[i]);
        else g.draw(rightMesh[i]);
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
        case 'h': phi -= 0.0025f; break;
        case 'y': phi += 0.0025f; break;
        case '[': epsilon -= 0.01f; break;
        case ']': epsilon += 0.01f; break;
        default: break;
      }

      camera = Mat4f(
        cos(theta), 0.f, 0.f, -sin(theta),
        0.f, cos(theta+phi), -sin(theta+phi), 0.f,
        0.f, sin(theta+phi), cos(theta+phi), 0.f,
        sin(theta), 0.f, 0.f, cos(theta));

      eye = Mat4f(
        cos(epsilon), 0.f, 0.f, -sin(epsilon),
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        sin(epsilon), 0.f, 0.f, cos(epsilon));
    } else { m.print(); } // as_key
  } // onMessage
  
  // KEYBOARD commands local
  virtual bool onKeyDown(const Keyboard& k){
    switch (k.key()) {
      case 'g': theta -= 0.05f; break;
      case 't': theta += 0.05f; break;
      case 'h': phi -= 0.05f; break;
      case 'y': phi += 0.05f; break;
      case '[': epsilon -= 0.01f; break;
      case ']': epsilon += 0.01f; break;
      default: break;
    }

    camera = Mat4f(
      cos(theta), 0.f, 0.f, -sin(theta),
      0.f, cos(theta+phi), -sin(theta+phi), 0.f,
      0.f, sin(theta+phi), cos(theta+phi), 0.f,
      sin(theta), 0.f, 0.f, cos(theta));

    eye = Mat4f(
      cos(epsilon), 0.f, 0.f, -sin(epsilon),
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
