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

static const int vertNum = 1633;
static const int edgeNum = 200;
static const int edgeRes = 128;

struct HyperApp : OmniApp {

  Material material;
  Light light;

  std::vector<Vec4f> r4Vert;
  std::vector<Vec4f> s3Edge[edgeNum];
  std::vector<Mesh> leftMesh, rightMesh;

  float theta, phi;
  float epsilon;
  Mat4f camera;
  Mat4f eye;

  void generateEdge(std::vector<Vec4f>& tgtEdge, const Vec4f& srcVt1, const Vec4f& srcVt2) {

    for (int i = 0; i < edgeRes; ++i) {
      float t = (float)i / ((float)edgeRes - 1.f);
      Vec4f newPoint = srcVt1 + t * (srcVt2 - srcVt1);

      // projection onto S3
      tgtEdge[i] = newPoint.normalized();
    }
  }

  void generateMesh(Mesh& tgtMesh, const std::vector<Vec4f>& srcVt, HSV meshColor, const bool isRight = false) {
    tgtMesh.reset();
    tgtMesh.primitive(Graphics::LINE_STRIP);

    // apply camera rotation
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

  void addPermutation(Vec4f seed, int& index, bool even) {
    int i0 = (seed[0] != 0)? 1 : 0;
    int i1 = (seed[1] != 0)? 1 : 0;
    int i2 = (seed[2] != 0)? 1 : 0;
    int i3 = (seed[3] != 0)? 1 : 0;

    for (int i = 0; i <= i0; ++i) {
      for (int j = 0; j <= i1; ++j) {
        for (int k = 0; k <= i2; ++k) {
          for (int l = 0; l <= i3; ++l) {
            float s0 = seed[0] * pow(-1, i);
            float s1 = seed[1] * pow(-1, j);
            float s2 = seed[2] * pow(-1, k);
            float s3 = seed[3] * pow(-1, l);

            r4Vert[index++] = Vec4f(s0, s1, s2, s3);
            r4Vert[index++] = Vec4f(s0, s3, s1, s2);
            r4Vert[index++] = Vec4f(s0, s2, s3, s1);
            r4Vert[index++] = Vec4f(s1, s0, s3, s2);
            r4Vert[index++] = Vec4f(s1, s3, s2, s0);
            r4Vert[index++] = Vec4f(s1, s2, s0, s3);
            r4Vert[index++] = Vec4f(s2, s0, s1, s3);
            r4Vert[index++] = Vec4f(s2, s3, s0, s1);
            r4Vert[index++] = Vec4f(s2, s1, s3, s0);
            r4Vert[index++] = Vec4f(s3, s0, s2, s1);
            r4Vert[index++] = Vec4f(s3, s2, s1, s0);
            r4Vert[index++] = Vec4f(s3, s1, s0, s2);

            if (!even) {
              r4Vert[index++] = Vec4f(s0, s1, s3, s2);
              r4Vert[index++] = Vec4f(s0, s3, s2, s1);
              r4Vert[index++] = Vec4f(s0, s2, s1, s3);
              r4Vert[index++] = Vec4f(s1, s0, s2, s3);
              r4Vert[index++] = Vec4f(s1, s3, s0, s2);
              r4Vert[index++] = Vec4f(s1, s2, s3, s0);
              r4Vert[index++] = Vec4f(s2, s0, s3, s1);
              r4Vert[index++] = Vec4f(s2, s3, s1, s0);
              r4Vert[index++] = Vec4f(s2, s1, s0, s3);
              r4Vert[index++] = Vec4f(s3, s0, s1, s2);
              r4Vert[index++] = Vec4f(s3, s2, s0, s1);
              r4Vert[index++] = Vec4f(s3, s1, s2, s0);
            }
          }
        }
      }
    }
  }

  // add in rotation functions

  // CONSTRUCTOR
  HyperApp() {

    nav().pos(0.0, 0.0, 0.0);
    light.pos(0, 10.0, 0);
    light.specular(Color(0.1,0.1,0.1));
    light.diffuse(Color(0.2,0.2,0.2));
    light.ambient(Color(1,1,1));
    initWindow();
    initAudio();

    lens().eyeSep(0.03); // set eyeSep to zero

    theta = 0.f;
    camera = Mat4f(
      cos(theta), -sin(theta), 0.f, 0.f,
      sin(theta), cos(theta), 0.f, 0.f,
      0.f, 0.f, cos(theta), -sin(theta),
      0.f, 0.f, sin(theta), cos(theta));

    epsilon = 0.f;
    eye = Mat4f(
      cos(epsilon), 0.f, 0.f, -sin(epsilon),
      0.f, 1.f, 0.f, 0.f,
      0.f, 0.f, 1.f, 0.f,
      sin(epsilon), 0.f, 0.f, cos(epsilon));

    
    r4Vert.resize(vertNum);

    for (int i = 0; i < edgeNum; ++i)
      s3Edge[i].resize(edgeRes);

    // list of vertices for hypercube
    int vNum = 0;

    float gold = (1.f + sqrt(5)) / 2.f;

    addPermutation(Vec4f(0, 0, 2, 2), vNum, false);
    addPermutation(Vec4f(1, 1, 1, sqrt(5)), vNum, false);
    addPermutation(Vec4f(pow(gold, -2), gold, gold, gold), vNum, false);
    addPermutation(Vec4f(pow(gold, -1), pow(gold, -1), pow(gold, -1), pow(gold, 2)), vNum, false);
    addPermutation(Vec4f(0, pow(gold, -2), 1, pow(gold, 2)), vNum, true);
    addPermutation(Vec4f(0, pow(gold, -1), gold, sqrt(5)), vNum, true);
    addPermutation(Vec4f(pow(gold, -1), 1, gold, 2), vNum, true);

    cout << "vNum = " << vNum << endl;

    cout << r4Vert.size() << endl;

    cout << r4Vert[2] << endl;

    int k = 0;
    for (int i = 0; i < vNum; ++i) {
      for (int j = i + 1; j < vNum; ++j) {
        Vec4f dist = (r4Vert[i] - r4Vert[j]) * 0.5f;
        if (dist.mag() == 1.f) {
          generateEdge(s3Edge[k], r4Vert[i], r4Vert[j]);
          k++;
        }
      }
    }

    cout << vertNum << " vertices -> " << k << " edges" << endl;
    if (k != edgeNum) {
      cout << "Error: Predefined Edge Number didn't match generated Edge Number!" << endl;
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
      g.lineWidth(5);
      
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
      if (i == 'g') {
        theta -= 0.1f; camera = Mat4f(
        cos(theta), -sin(theta), 0.f, 0.f,
        sin(theta), cos(theta), 0.f, 0.f,
        0.f, 0.f, cos(theta), -sin(theta),
        0.f, 0.f, sin(theta), cos(theta));
      } else if (i == 't') {
        theta += 0.1f; camera = Mat4f(
        cos(theta), -sin(theta), 0.f, 0.f,
        sin(theta), cos(theta), 0.f, 0.f,
        0.f, 0.f, cos(theta), -sin(theta),
        0.f, 0.f, sin(theta), cos(theta));
      } else if (i == '[') {
        epsilon -= 0.01f; eye = Mat4f(
        cos(epsilon), 0.f, 0.f, -sin(epsilon),
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        sin(epsilon), 0.f, 0.f, cos(epsilon));
      } else if (i == ']') {
        epsilon += 0.01f; eye = Mat4f(
        cos(epsilon), 0.f, 0.f, -sin(epsilon),
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        sin(epsilon), 0.f, 0.f, cos(epsilon));
      }
    } else { m.print(); } // as_key
  } // onMessage
  
  // KEYBOARD commands local
  virtual bool onKeyDown(const Keyboard& k){
    switch (k.key()) {
      case 'g': theta -= 0.1f; camera = Mat4f(
      cos(theta), -sin(theta), 0.f, 0.f,
      sin(theta), cos(theta), 0.f, 0.f,
      0.f, 0.f, cos(theta), -sin(theta),
      0.f, 0.f, sin(theta), cos(theta)); break;
      case 't': theta += 0.1f; camera = Mat4f(
      cos(theta), -sin(theta), 0.f, 0.f,
      sin(theta), cos(theta), 0.f, 0.f,
      0.f, 0.f, cos(theta), -sin(theta),
      0.f, 0.f, sin(theta), cos(theta)); break;
      case '[': epsilon -= 0.01f; eye = Mat4f(
      cos(epsilon), 0.f, 0.f, -sin(epsilon),
      0.f, 1.f, 0.f, 0.f,
      0.f, 0.f, 1.f, 0.f,
      sin(epsilon), 0.f, 0.f, cos(epsilon)); break;
      case ']': epsilon += 0.01f; eye = Mat4f(
      cos(epsilon), 0.f, 0.f, -sin(epsilon),
      0.f, 1.f, 0.f, 0.f,
      0.f, 0.f, 1.f, 0.f,
      sin(epsilon), 0.f, 0.f, cos(epsilon)); break;
      // case '1': omni().mode(OmniStereo::DUAL).stereo(true); break;
      // case '2': omni().mode(OmniStereo::ANAGLYPH).stereo(true); break;
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
