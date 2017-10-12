// Steve Trettel's Hypercube Projection
//
// Dennis Adderton
// Kenny Kim
/*
*/


#include "al_OmniApp.hpp"
#include "allocore/io/al_App.hpp"
#include <iostream>
#include <set>

using namespace al;
using namespace std;

static const int edgeRes = 2;

struct HyperApp : OmniApp {

  Material material;
  Light light;

  std::vector<Vec4f> h3Vert;
  std::vector<Vec2i> h3Edge;
  std::vector<Mesh> leftMesh, rightMesh;

  float theta, epsilon, phi;
  float eyeAngle;
  Mat4f camera;
  Mat4f eye;

  std::vector<Mat4f> generator;
  Vec4f seed = Vec4f(1, 0, 0, 0);
  int depth = 5;

  //RTR^-1
  // z axis

  void generateEdge(std::vector<Mat4f>& gen, Vec4f& seed, int depth) {
    h3Vert.clear();
    h3Edge.clear();

    h3Vert.push_back(seed);
    
    for (int d = 0; d < depth; ++d) {
      int currentSize = h3Vert.size();
      // cout << "current depth = " << d << endl;
      // cout << " current size = " << currentSize << endl;

      for (int j = 0; j < currentSize; ++j) {
        for (int i = 0; i < gen.size(); ++i) {
          Vec4f newPoint;
          // gen[i].print();
          // h3Vert[j].print();
          Mat4f::multiply(newPoint, gen[i], h3Vert[j]);

          bool unique = true;

          for (int k = 0; k < h3Vert.size(); ++k) {
            if (newPoint == h3Vert[k]) {
              unique = false;
            }
          }

          if(unique) {
            h3Vert.push_back(newPoint);
            h3Edge.push_back(Vec2i(j, h3Vert.size()-1));
          }
        }
      }
    }
  }

  void generateMesh(Mesh& tgtMesh, const Vec4f srcVt1, const Vec4f srcVt2, const HSV meshColor, const bool isRight = false) {
    tgtMesh.reset();
    tgtMesh.primitive(Graphics::LINE_STRIP);

    // apply camera rotation
    Vec4f postRotVt1, postRotVt2;
    Mat4f::multiply(postRotVt1, camera, srcVt1);
    Mat4f::multiply(postRotVt2, camera, srcVt2);
    if (isRight) {
      Mat4f::multiply(postRotVt1, eye, postRotVt1);
      Mat4f::multiply(postRotVt2, eye, postRotVt2);
    }

    // projection onto R3
    Vec3f newVt1 = Vec3f(
      postRotVt1[1] / postRotVt1[0],
      postRotVt1[2] / postRotVt1[0],
      postRotVt1[3] / postRotVt1[0]
      );
    Vec3f newVt2 = Vec3f(
      postRotVt2[1] / postRotVt2[0],
      postRotVt2[2] / postRotVt2[0],
      postRotVt2[3] / postRotVt2[0]
      );

    for (int i = 0; i < edgeRes; ++i) {
      float t = (float)i / ((float)edgeRes - 1.f);
      Vec3f newPoint = newVt1 + t * (newVt2 - newVt1);
      tgtMesh.vertex(newPoint);
      tgtMesh.color(meshColor);
    }
  }

  // t2 - x2 - y2 -z2 = 1
  // x/t, y/t, z/t

  // add in rotation functions

  Mat4f rotateTheta(Mat4f& srcMat, float& angle) {
    Mat4f rotate = Mat4f(
      cosh(angle), sinh(angle), 0.f, 0.f,
      sinh(angle), cosh(angle), 0.f, 0.f,
      0.f, 0.f, 1.f, 0.f,
      0.f, 0.f, 0.f, 1.f);

    return rotate * srcMat;
  }

  Mat4f rotateEpsilon(Mat4f& srcMat, float& angle) {
    Mat4f rotate = Mat4f(
      cosh(angle), 0.f, 0.f, sinh(angle),
      0.f, 1.f, 0.f, 0.f,
      0.f, 0.f, 1.f, 0.f,
      sinh(angle), 0.f, 0.f, cosh(angle));

    return rotate * srcMat;
  }

  Mat4f rotatePhi(Mat4f& srcMat, float& angle) {
    Mat4f rotate = Mat4f(
      cosh(angle), 0.f, sinh(angle), 0.f,
      0.f, 1.f, 0.f, 0.f,
      sinh(angle), 0.f, cosh(angle), 0.f,
      0.f, 0.f, 0.f, 1.f);

    return rotate * srcMat;
  }

  // CONSTRUCTOR
  HyperApp() {

    nav().pos(0.0, 0.0, 0.0);
    light.pos(0, 10.0, 0);
    light.specular(Color(0.3,0.3,0.3));
    light.diffuse(Color(0.2,0.2,0.2));
    light.ambient(Color(1,1,1));
    initWindow();
    initAudio();

    lens().eyeSep(0.03); // set eyeSep to zero

    theta = 0.f;
    epsilon = 0.f;
    phi = 0.f;
    camera.setIdentity();

    eyeAngle = 0.f;
    eye.setIdentity();
    eye = rotateEpsilon(eye, eyeAngle);

    h3Vert.resize(2048);
    h3Edge.resize(2048);
    leftMesh.resize(4096);
    rightMesh.resize(4096);


    Mat4f genA, genB, genAinv, genBinv;

    genA = Mat4f(
      3.f, 0.f, -2.f, 2.f,
      0.f, 1.f, 0.f, 0.f,
      -2.f, 0.f, 1.f, -2.f,
      -2.f, 0.f, 2.f, -1.f);

    genB = Mat4f(
      3.f, 2.f, -2.f, 0.f,
      2.f, 1.f, -2.f, 0.f,
      2.f, 2.f, -1.f, 0.f,
      0.f, 0.f, 0.f, 1.f);

    genAinv = genA;
    invert(genAinv);

    genA.print();
    genAinv.print();

    genBinv = genB;
    invert(genBinv);

    genB.print();
    genBinv.print();

    generator.resize(4);

    generator[0] = genA;
    generator[1] = genB;
    generator[2] = genAinv;
    generator[3] = genBinv;

    generateEdge(generator, seed, depth);

    for(int i = 0; i < h3Edge.size(); ++i) {
      generateMesh(leftMesh[i], h3Vert[h3Edge[i][0]], h3Vert[h3Edge[i][1]], HSV((float)i / (float)h3Edge.size(), 1.f, 1.f), false);
      generateMesh(rightMesh[i], h3Vert[h3Edge[i][0]], h3Vert[h3Edge[i][1]], HSV((float)i / (float)h3Edge.size(), 1.f, 1.f), true);
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
      
      for(int i = 0; i < h3Edge.size(); ++i) {
        generateMesh(leftMesh[i], h3Vert[h3Edge[i][0]], h3Vert[h3Edge[i][1]], HSV((float)i / (float)h3Edge.size(), 1.f, 1.f), false);
        generateMesh(rightMesh[i], h3Vert[h3Edge[i][0]], h3Vert[h3Edge[i][1]], HSV((float)i / (float)h3Edge.size(), 1.f, 1.f), true);
        
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
      switch (i) {
        case 'r': theta = 0.f; epsilon = 0.f; phi = 0.f; break;
        case 'g': theta -= 0.1f; break;
        case 't': theta += 0.1f; break;
        case 'h': epsilon -= 0.1f; break;
        case 'y': epsilon += 0.1f; break;
        case 'j': phi -= 0.1f; break;
        case 'u': phi += 0.1f; break;

        case '[': eyeAngle -= 0.01f; break;
        case ']': eyeAngle += 0.01f; break;

        default: break;
      }

      camera.setIdentity();
      camera = rotateTheta(camera, theta);
      camera = rotateEpsilon(camera, epsilon);
      camera = rotatePhi(camera, phi);

      eye.setIdentity();
      eye = rotateEpsilon(eye, eyeAngle);

    } else { m.print(); } // as_key
  } // onMessage
  
  // KEYBOARD commands local
  virtual bool onKeyDown(const Keyboard& k){
    switch (k.key()) {
      case 'r': theta = 0.f; epsilon = 0.f; phi = 0.f; break;
      case 'g': theta -= 0.1f; break;
      case 't': theta += 0.1f; break;
      case 'h': epsilon -= 0.1f; break;
      case 'y': epsilon += 0.1f; break;
      case 'j': phi -= 0.1f; break;
      case 'u': phi += 0.1f; break;
      case 'i': ++depth; generateEdge(generator, seed, depth); break;
      case 'k': --depth; generateEdge(generator, seed, depth); break;

      case '[': eyeAngle -= 0.01f; break;
      case ']': eyeAngle += 0.01f; break;

      // case '1': omni().mode(OmniStereo::DUAL).stereo(true); break;
      // case '2': omni().mode(OmniStereo::ANAGLYPH).stereo(true); break;
      default: break;
    }

    camera.setIdentity();
    camera = rotateTheta(camera, theta);
    camera = rotateEpsilon(camera, epsilon);
    camera = rotatePhi(camera, phi);

    eye.setIdentity();
    eye = rotateEpsilon(eye, eyeAngle);

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
