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

static const int vertexNumber = 16;
static const int edgeNumber = 256;
static const int edgeResolution = 512;

struct HyperApp : OmniApp {

  Material material;
  Light light;

  std::vector<Vec4f> hypercube_vertices;
  std::vector<Vec4f> r4Edge[edgeNumber];
  std::vector<Vec3f> s3Edge[edgeNumber];
  std::vector<Mesh> edgeMesh;

  float theta, phi;
  float epsilon;
  Mat4f camera;
  Mat4f eye;

  void generateEdge(std::vector<Vec4f>& tgtEdge, int v1, int v2) {
    
    // tgtEdge.resize(edgeResolution);

    for (int i = 0; i < edgeResolution; ++i) {
      float t = 2.f * M_PI * (float)i / ((float)edgeResolution - 1.f);

      float R_ = (float)v1;
      float T_ = 2.f * M_PI * (float)v2 / (float)(vertexNumber - 1);

      Vec4f newPoint = Vec4f(
        R_ / sqrt(R_*R_ + 1.f) * cos(T_ + t),
        R_ / sqrt(R_*R_ + 1.f) * sin(T_ + t),
        1.f / sqrt(R_*R_ + 1.f) * cos(t),
        1.f / sqrt(R_*R_ + 1.f) * sin(t)
        );

      tgtEdge[i] = newPoint.normalized();
    }

    // cout << tgtEdge.size() << endl;

    // for (int i = 0; i < edgeResolution; ++i)
    //   cout << tgtEdge[i] << endl;
  }

  void projectR4toR3(std::vector<Vec3f>& tgtVt, std::vector<Vec4f>& srcVt) {
    tgtVt.clear();

    for (int i = 0; i < srcVt.size(); ++i) {

      Vec4f postRotVt = srcVt[i];
      Mat4f::multiply(postRotVt, camera, srcVt[i]);
      
      // projection onto R3
      Vec3f newVt = Vec3f(
        postRotVt[1] / (1.f - postRotVt[0]),
        postRotVt[2] / (1.f - postRotVt[0]),
        postRotVt[3] / (1.f - postRotVt[0])
        );

      tgtVt.push_back(newVt);
    }
  }

  void generateMesh(Mesh& targetMesh, std::vector<Vec3f>& srcVt, HSV meshColor) {
    targetMesh.reset();

    targetMesh.primitive(Graphics::LINE_STRIP);
    for (int i = 0; i < srcVt.size(); ++i) {
      targetMesh.vertex(srcVt[i]);
      targetMesh.color(meshColor);  
    }
  }

  // CONSTRUCTOR
  HyperApp() {

    nav().pos(0.0, 0.0, 0.0);
    light.pos(0, 10.0, 0);
    light.specular(Color(0,0,0));
    light.diffuse(Color(0,0,0));
    light.ambient(Color(1,1,1));
    initWindow();
    initAudio();

    theta = 0.f;
    phi = 0.f;
    camera = Mat4f(
      cos(theta), 0.f, 0.f, -sin(theta),
      0.f, cos(theta+phi), -sin(theta+phi), 0.f,
      0.f, sin(theta+phi), cos(theta+phi), 0.f,
      sin(theta), 0.f, 0.f, cos(theta));

    epsilon = 0.01f;
    eye = Mat4f(
      cos(epsilon), 0.f, 0.f, -sin(epsilon),
      0.f, 1.f, 0.f, 0.f,
      0.f, 0.f, 1.f, 0.f,
      sin(epsilon), 0.f, 0.f, cos(epsilon));

    hypercube_vertices.resize(vertexNumber);

    // list of vertices for hypercube
    hypercube_vertices[0]  = Vec4f(-1.0,-1.0,-1.0,-1.0);
    hypercube_vertices[1]  = Vec4f(-1.0,-1.0,-1.0, 1.0);
    hypercube_vertices[2]  = Vec4f(-1.0,-1.0, 1.0,-1.0);
    hypercube_vertices[3]  = Vec4f(-1.0,-1.0, 1.0, 1.0);
    hypercube_vertices[4]  = Vec4f(-1.0, 1.0,-1.0,-1.0);
    hypercube_vertices[5]  = Vec4f(-1.0, 1.0,-1.0, 1.0);
    hypercube_vertices[6]  = Vec4f(-1.0, 1.0, 1.0,-1.0);
    hypercube_vertices[7]  = Vec4f(-1.0, 1.0, 1.0, 1.0);
    hypercube_vertices[8]  = Vec4f( 1.0,-1.0,-1.0,-1.0);
    hypercube_vertices[9]  = Vec4f( 1.0,-1.0,-1.0, 1.0);
    hypercube_vertices[10] = Vec4f( 1.0,-1.0, 1.0,-1.0);
    hypercube_vertices[11] = Vec4f( 1.0,-1.0, 1.0, 1.0);
    hypercube_vertices[12] = Vec4f( 1.0, 1.0,-1.0,-1.0);
    hypercube_vertices[13] = Vec4f( 1.0, 1.0,-1.0, 1.0);
    hypercube_vertices[14] = Vec4f( 1.0, 1.0, 1.0,-1.0);
    hypercube_vertices[15] = Vec4f( 1.0, 1.0, 1.0, 1.0);

    for (int i = 0; i < edgeNumber; ++i)
      r4Edge[i].resize(edgeResolution);

    int k = 0;
    for (int i = 0; i < vertexNumber; ++i) {
      for (int j = i + 1; j < vertexNumber; ++j) {
        generateEdge(r4Edge[k++], i, j);
      }
    }

    cout << vertexNumber << " vertices -> " << k << " edges" << endl;

    edgeMesh.resize(edgeNumber);    

    for(int i = 0; i < edgeNumber; ++i) {
      projectR4toR3(s3Edge[i], r4Edge[i]);
      generateMesh(edgeMesh[i], s3Edge[i], HSV((float)i / (float)edgeNumber, 1.f, 1.f));
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
      g.lineWidth(5);
      
      for(int i = 0; i < edgeNumber; ++i) {
        projectR4toR3(s3Edge[i], r4Edge[i]);
        generateMesh(edgeMesh[i], s3Edge[i], HSV((float)i / (float)edgeNumber, 1.f, 1.f));
        g.draw(edgeMesh[i]);
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
      if (i == ' ') { /**/ }
    } else { m.print(); } // as_key
  } // onMessage
  
  // KEYBOARD commands local
  virtual bool onKeyDown(const Keyboard& k){
    switch (k.key()) {
      case 'g': theta -= 0.05f; break;
      case 't': theta += 0.05f; break;
      case 'h': phi -= 0.05f; break;
      case 'y': phi += 0.05f; break;
      default: break;
    }

    camera = Mat4f(
      cos(theta), 0.f, 0.f, -sin(theta),
      0.f, cos(theta+phi), -sin(theta+phi), 0.f,
      0.f, sin(theta+phi), cos(theta+phi), 0.f,
      sin(theta), 0.f, 0.f, cos(theta));

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
