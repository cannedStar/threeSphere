// Steve Trettel's Hypercube Projection
//
// Dennis Adderton
// Kenny Kim
/*
*/

// t2 - x2 - y2 -z2 = 1
// x/t, y/t, z/t


#include "al_OmniApp.hpp"
// #include "alloutil/al_OmniApp.hpp"
#include "allocore/graphics/al_Asset.hpp"
#include <iostream>
#include <set>

#include "transformations.hpp"
#include "mesh4D.hpp"

using namespace al;
using namespace std;

const int edgeRes = 2;
const int maxDepth = 8;

struct Transform {
  Mat4d mat;
  Mat4d origin;
  int depth;
  Color color;
};

struct HyperApp : OmniApp {

  Material material;
  Light light;

  std::vector<Transform> transforms;

  std::vector<Mesh> leftMesh, rightMesh;

  Mesh sphere;
  Mesh4D sphere4D;

  double theta, epsilon, phi;
  double eyeAngle;
  double t_temp;
  Mat4d camera;
  Mat4d eye;

  std::vector<Mat4d> generator;
  Vec4d seed = Vec4d(1, 0, 0, 0);
  int depth = 2;

  Color transColor(int idx) {
    switch(idx) {
      case 0: return Color(1.0, 0.0, 0.0, 1.0);
      case 1: return Color(0.0, 1.0, 0.0, 1.0);
      case 2: return Color(0.0, 0.0, 1.0, 1.0);
      case 3: return Color(1.0, 1.0, 0.0, 1.0);
      default: return Color(1.0, 1.0, 1.0, 1.0);
    }
  }

  void generateTrans(std::vector<Mat4d>& gen) {
    transforms.clear();

    Transform newTrans;
    newTrans.mat = Mat4d::identity();
    newTrans.origin = Mat4d::identity();
    newTrans.depth = 0;
    newTrans.color = Color(1.0,1.0,1.0,1.0);
    transforms.push_back(newTrans);

    if (maxDepth > 0) {
      newTrans.depth = 1;
      for (int i = 0; i < gen.size(); ++i) {
        newTrans.mat = gen[i] * newTrans.origin;
        newTrans.color = transColor(i);
        transforms.push_back(newTrans);
      }
    }

    int currentIdx = 1;

    for (int i = 1; i < maxDepth; ++i) {
      int formerIdx = currentIdx;
      currentIdx = transforms.size();

      for(int j = formerIdx; j < currentIdx; ++j) {
        Transform& old = transforms[j];
        for (int k = 0; k < gen.size(); ++k) {
          newTrans.origin = gen[k] * old.origin;
          newTrans.mat = gen[k] * old.mat;
          newTrans.depth = old.depth + 1;
          newTrans.color = old.color;
          // newTrans.origin.print();
          // newTrans.mat.print();
          bool unique = true;

          for(int l = 0; l < transforms.size(); ++l) {
            // cout << "- dest -" << endl;
            // newTrans.mat.print();
            // cout << "- origin -" << endl;
            // transforms[l].mat.print();
            if (compare(newTrans.mat, transforms[l].mat)) {
              // cout << "same" << endl;
              unique = false;
              break;
            }
            // cout << endl;
            // cout << "- dest -" << endl;
            // newTrans.mat.print();
            // cout << "- origin -" << endl;
            // transforms[l].origin.print();
            if (compare(newTrans.mat, transforms[l].origin)) {
              unique = false;
              // cout << "same dest" << endl;
              break;
            }
          }

          if(unique) transforms.push_back(newTrans);
        }
      }
      // cout << transforms.size() << endl;
    }
    cout << transforms.size() << endl;
  }

  void generateMesh(Mesh& tgtMesh, const Transform& trans, const bool isRight = false) {
    tgtMesh.reset();
    tgtMesh.primitive(Graphics::LINE_STRIP);

    // apply camera rotation
    Vec4d srcVt1 = trans.origin * seed;
    Vec4d srcVt2 = trans.mat * seed;
    Vec4d postRotVt1, postRotVt2;
    Mat4d::multiply(postRotVt1, camera, srcVt1);
    Mat4d::multiply(postRotVt2, camera, srcVt2);
    if (isRight) {
      Mat4d::multiply(postRotVt1, eye, postRotVt1);
      Mat4d::multiply(postRotVt2, eye, postRotVt2);
    }

    // projection onto R3
    Vec3d newVt1 = Vec3d(
      postRotVt1[1] / postRotVt1[0],
      postRotVt1[2] / postRotVt1[0],
      postRotVt1[3] / postRotVt1[0]
      );
    Vec3d newVt2 = Vec3d(
      postRotVt2[1] / postRotVt2[0],
      postRotVt2[2] / postRotVt2[0],
      postRotVt2[3] / postRotVt2[0]
      );



    for (int i = 0; i < edgeRes; ++i) {
      double t = (double)i / ((double)edgeRes - 1.0);
      Vec3d newPoint = newVt1 + t * (newVt2 - newVt1);
      // tgtMesh.vertex(uhs(newPoint));
      tgtMesh.vertex(newPoint);
      tgtMesh.color(trans.color);
    }
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

    theta = 0.0;
    epsilon = 0.0;
    phi = 0.0;
    t_temp = 0.0;
    camera.setIdentity();

    eyeAngle = 0.0;
    eye.setIdentity();
    eye = rotateEpsilon(eye, eyeAngle);


    leftMesh.reserve(409600);
    rightMesh.reserve(409600);

    addSphere(sphere, 0.05);
    sphere4D.init(sphere);

    Mat4d genA, genB, genAinv, genBinv;

    // // Aplonian Gasket
    // genA = Mat4d(
    //   3.0, 0.0, -2.0, 2.0,
    //   0.0, 1.0, 0.0, 0.0,
    //   -2.0, 0.0, 1.0, -2.0,
    //   -2.0, 0.0, 2.0, -1.0);

    // genB = Mat4d(
    //   3.0, 2.0, -2.0, 0.0,
    //   2.0, 1.0, -2.0, 0.0,
    //   2.0, 2.0, -1.0, 0.0,
    //   0.0, 0.0, 0.0, 1.0);

    // Figure 8 knot complement
    genA = Mat4d(
      1.5, 1.0, 0.0, -0.5,
      1.0, 1.0, 0.0, -1.0,
      0.0, 0.0, 1.0, 0.0,
      0.5, 1.0, 0.0, 0.5);

    genB = Mat4d(
      1.5, 0.5, -sqrt(3)/2.0, 0.5,
      0.5, 1.0, 0.0, 0.5,
      -sqrt(3)/2.0, 0.0, 1.0, -sqrt(3)/2.0,
      -0.5, -0.5, sqrt(3)/2.0, 0.5);

    genAinv = genA;
    invert(genAinv);

    genBinv = genB;
    invert(genBinv);

    // genA.print();
    // genAinv.print();
    // genB.print();
    // genBinv.print();

    generator.resize(4);

    generator[1] = genA;
    generator[2] = genB;
    generator[3] = genAinv;
    generator[0] = genBinv;

    generateTrans(generator);

    for(int i = 0; i < transforms.size(); ++i) {
      generateMesh(leftMesh[i], transforms[i], false);
      generateMesh(rightMesh[i], transforms[i], true);
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

    // for(int i = 0; i < transforms.size(); ++i) {
    //   // sphere4D.transform(transforms[i]);
    //   for(int j = 0; j < sphere4D.vertices.size(); ++j) {
    //     Vec4d newVert;
    //     // newVert = sphere4D.vertices[j];
    //     Mat4d::multiply(newVert, camera * transforms[i].mat, sphere4D.vertices[j]);
    //     Vec3d temp = klein(newVert);
    //     sphere.vertices()[j] = uhs(temp);
    //   }
    //   g.draw(sphere);
    // }

    g.pushMatrix();  
      g.depthTesting(true);
      g.blending(true);
      g.blendModeTrans();
      g.pointSize(6);
      g.lineWidth(5);
      
      for(int i = 0; i < transforms.size(); ++i) {
        if(transforms[i].depth >= depth) break;
        // if(transforms[i].depth == depth) {
          generateMesh(leftMesh[i], transforms[i], false);
          generateMesh(rightMesh[i], transforms[i], true);
          
          if (omni().currentEye() == 0) g.draw(leftMesh[i]);
          else g.draw(rightMesh[i]);
        // }
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
        case 'r': theta = 0.0; epsilon = 0.0; phi = 0.0; break;
        case 'g': theta -= 0.1; break;
        case 't': theta += 0.1; break;
        case 'h': epsilon -= 0.1; break;
        case 'y': epsilon += 0.1; break;
        case 'j': phi -= 0.1; break;
        case 'u': phi += 0.1; break;
        case 'i': ++depth; break;
        case 'k': --depth; break;

        case '[': eyeAngle -= 0.01; break;
        case ']': eyeAngle += 0.01; break;

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
      case 'r': theta = 0.0; epsilon = 0.0; phi = 0.0; break;
      case 'g': theta -= 0.1; break;
      case 't': theta += 0.1; break;
      case 'h': epsilon -= 0.1; break;
      case 'y': epsilon += 0.1; break;
      case 'j': phi -= 0.1; break;
      case 'u': phi += 0.1; break;
      case 'i': ++depth; break;
      case 'k': --depth; break;

      case '[': eyeAngle -= 0.01; break;
      case ']': eyeAngle += 0.01; break;

      // case '1': omni().mode(OmniStereo::DUAL).stereo(true); break;
      // case '2': omni().mode(OmniStereo::ANAGLYPH).stereo(true); break;
      default: break;
    }

    camera.setIdentity();
    camera = rotateTheta(camera, theta);
    // camera = rotateEpsilon(camera, epsilon);
    // camera = rotatePhi(camera, phi);
    camera = para(camera, epsilon, phi);

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
