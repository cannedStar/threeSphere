// Steve Trettel's Hypercube Projection
//
// Dennis Adderton
// Kenny Kim
/*
*/


#include "al_OmniApp.hpp"
#include "allocore/io/al_App.hpp"
#include "allocore/graphics/al_Asset.hpp"
#include <iostream>
#include <set>

using namespace al;
using namespace std;

static const int edgeRes = 2;

struct Edge {
  Vec4f left;
  Vec4f right;
};

struct HyperApp : OmniApp {

  Material material;
  Light light;

  std::vector<Edge> h3Edge;
  std::vector<Mesh> leftMesh, rightMesh;

  std::vector<Vec4f> cubeVerts;

  Texture tex;
  Scene* ascene = 0;
  Vec3f scene_min, scene_max, scene_center;
  DisplayList scene_list;

  Mesh cube, ducky;

  float theta, epsilon, phi;
  float eyeAngle;
  Mat4f camera;
  Mat4f eye;

  std::vector<Mat4f> generator;
  Vec4f seed = Vec4f(1, 0, 0, 0);
  int depth = 2;

  float cubeLen = 0.5;

  bool initDraw = true;

  //RTR^-1
  // z axis

  void generateEdge(std::vector<Mat4f>& gen, Vec4f& seed, int depth) {
    h3Edge.clear();

    int currentSize = 0;

    if (depth > 0) {
      Edge newEdge;
      newEdge.left = seed;
      for (int i = 0; i < gen.size(); ++i) {
        Mat4f::multiply(newEdge.right, gen[i], seed);
        cout << newEdge.left << endl;
        cout << newEdge.right << endl;
        h3Edge.push_back(newEdge);
      }
    }

    for (int d = 1; d < depth; ++d) {
      int formerSize = currentSize;
      currentSize = h3Edge.size();
      cout << "current depth = " << d << endl;
      cout << " current size = " << currentSize << endl;

      for (int j = formerSize; j < currentSize; ++j) {
        for (int i = 0; i < gen.size(); ++i) {
          Edge newEdge;
          // gen[i].print();
          // h3Vert[j].print();
          cout << j << endl;
          Mat4f::multiply(newEdge.left, gen[i], h3Edge[j].left);
          Mat4f::multiply(newEdge.right, gen[i], h3Edge[j].right);

          // bool unique = true;

          // for (int k = 0; k < h3Vert.size(); ++k) {
          //   if (newPoint == h3Vert[k]) {
          //     unique = false;
          //   }
          // }

          // if(unique) {
            cout << newEdge.left << endl;
            cout << newEdge.right << endl;
            h3Edge.push_back(newEdge);
          // }
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

  Mat4f para(Mat4f& srcMat, float& p, float& q) {
    Mat4f par = Mat4f(
      0.5f*(2.f + p*p + q*q), p, -q, -0.5f*(p*p + q*q),
      p, 1, 0, -p,
      -q, 0, 1, q,
      0.5f*(p*p + q*q), p, -q, 0.5f*(2.f - p*p - q*q));

    return par * srcMat;
  }

  void addCube4D(Mesh& m, float l) {
    m.reset();
    m.primitive(Graphics::TRIANGLES);
    int Nv = 8;

    cubeVerts.push_back(Vec4f(0, -l, l, -l));
    cubeVerts.push_back(Vec4f(0, l, l, -l));
    cubeVerts.push_back(Vec4f(0, -l, -l, -l));
    cubeVerts.push_back(Vec4f(0, l, -l, -l));
    cubeVerts.push_back(Vec4f(0, -l, l, l));
    cubeVerts.push_back(Vec4f(0, l, l, l));
    cubeVerts.push_back(Vec4f(0, -l, -l, l));
    cubeVerts.push_back(Vec4f(0, l, -l, l));

    // All six faces will have the same tex coords
    for(int i=0;i<6;++i){
      m.texCoord( 0, 0);
      m.texCoord( 1, 0);
      m.texCoord( 1, 1);
      m.texCoord( 0, 1);
    }

    for (int i = 0; i < Nv; ++i) {
      Vec4f& v = cubeVerts[i];

      // cout << v[3] * v[3] - v[0] * v[0] - v[1] * v[1] - v[2] * v[2] << endl;

      v[0] = sqrt(v[1]*v[1] + v[2]*v[2] + v[3]*v[3] + 1);

      cout << v << endl;

      // apply camera rotation
      Vec4f postRotVt;
      Mat4f::multiply(postRotVt, camera, v);
      // if (isRight) {
      //   Mat4f::multiply(postRotVt, eye, postRotVt);
      // }

      // projection onto R3
      Vec3f newVt = Vec3f(
        postRotVt[1] / postRotVt[0],
        postRotVt[2] / postRotVt[0],
        postRotVt[3] / postRotVt[0]
        );

      cout << i << ": " << newVt << endl;

      m.vertex(newVt);
      // m.vertex(v);
    }
    // m.vertex(-l, l,-l); m.vertex( l, l,-l);
    // m.vertex(-l,-l,-l); m.vertex( l,-l,-l);
    // m.vertex(-l, l, l); m.vertex( l, l, l);
    // m.vertex(-l,-l, l); m.vertex( l,-l, l);

    static const int indices[] = {
      6,5,4, 6,7,5, 7,1,5, 7,3,1,
      3,0,1, 3,2,0, 2,4,0, 2,6,4,
      4,1,0, 4,5,1, 2,3,6, 3,7,6
    };

    m.index(indices, sizeof(indices)/sizeof(*indices), m.vertices().size()-Nv);

    m.generateNormals();
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

    h3Edge.reserve(204800);
    leftMesh.reserve(409600);
    rightMesh.reserve(409600);

    Mat4f genA, genB, genAinv, genBinv;

    SearchPaths sPath;

    sPath.addAppPaths();
    FilePath path = sPath.find("ducky.obj");
    printf("reading %s\n", path.filepath().c_str());

    ascene = Scene::import(path.filepath());

    if(ascene==0) {
      printf("error reading %s\n", path.filepath().c_str());
    } else {
      ascene->getBounds(scene_min,scene_max);
      scene_center = (scene_min + scene_max) / 2.f;
      ascene->dump();
    }

    Image img(sPath.find("hubble.jpg").filepath());
    tex.allocate(img.array());

    // // Aplonian Gasket
    // genA = Mat4f(
    //   3.f, 0.f, -2.f, 2.f,
    //   0.f, 1.f, 0.f, 0.f,
    //   -2.f, 0.f, 1.f, -2.f,
    //   -2.f, 0.f, 2.f, -1.f);

    // genB = Mat4f(
    //   3.f, 2.f, -2.f, 0.f,
    //   2.f, 1.f, -2.f, 0.f,
    //   2.f, 2.f, -1.f, 0.f,
    //   0.f, 0.f, 0.f, 1.f);

    // // Figure 8 knot complement
    // genA = Mat4f(
    //   1.5f, 1.f, 0.f, -0.5f,
    //   1.f, 1.f, 0.f, -1.f,
    //   0.f, 0.f, 1.f, 0.f,
    //   0.5f, 1.f, 0.f, 0.5f);

    // genB = Mat4f(
    //   1.5f, 0.5f, -sqrt(3) / 2.f, 0.5f,
    //   0.5f, 1.f, 0.f, 0.5f,
    //   -sqrt(3)/2.f, 0.f, 1.f, -sqrt(3)/2.f,
    //   -0.5f, -0.5f, sqrt(3)/2.f, 0.5f);

    // 

    genA = Mat4f(
      5.93389, -5.64013, -1.21902, 0.956005,
      4.32405, -4.06786, -1.20665, 1.30152,
      -2.92548, 3.06786, -0.20665, -0.322444,
      -2.63726, 2.61759, 0.99363, 0.340625);

    genB = Mat4f(
      1.86603, 0.371514, -0.545342, 1.4306,
      1.51127, 0.622915, -0.78229, 1.51127,
      0.112695, -0.78229, -0.622915, 0.112695,
      -0.430605, 0.371514, -0.545342, -0.866025);

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

    generateEdge(generator, seed, depth);

    addCube4D(cube, cubeLen);

    for(int i = 0; i < h3Edge.size(); ++i) {
      generateMesh(leftMesh[i], h3Edge[i].left, h3Edge[i].right, HSV((float)i / (float)h3Edge.size(), 1.f, 1.f), false);
      generateMesh(rightMesh[i], h3Edge[i].left, h3Edge[i].right, HSV((float)i / (float)h3Edge.size(), 1.f, 1.f), true);
    }
  } // HyperApp()
  
  virtual ~HyperApp() {}    // what does this do?

  // ANIMATE
  virtual void onAnimate(double dt) {   
  }
    
  // DRAW 
  virtual void onDraw(Graphics& g) {
    if(initDraw) {
      scene_list.begin();
      for(unsigned i=0; i < ascene->meshes(); ++i) {
        ascene->mesh(i, ducky);
        g.draw(ducky);
      }
      scene_list.end();
      initDraw = false;
    }

    material(); // material ...
    light();    // light ...

    // addCube4D(cube, cubeLen);

    // g.draw(cube);

    g.pushMatrix();
      float tmp = scene_max[0]-scene_min[0];
      tmp = al::max(scene_max[1] - scene_min[1],tmp);
      tmp = al::max(scene_max[2] - scene_min[2],tmp);
      tmp = 2.f / tmp;
      g.scale(tmp);
      g.translate(-scene_center);
      shader().uniform("texture0", 1);
      shader().uniform("texture", 1.0);
      tex.bind(1);
      scene_list.draw();
      tex.unbind(1);
      shader().uniform("texture", 0.0);
    g.popMatrix();
      
    g.pushMatrix();  
      g.depthTesting(true);
      g.blending(true);
      g.blendModeTrans();
      g.pointSize(6);
      g.lineWidth(5);
      
      for(int i = 0; i < h3Edge.size(); ++i) {
        generateMesh(leftMesh[i], h3Edge[i].left, h3Edge[i].right, HSV((float)i / (float)h3Edge.size(), 1.f, 1.f), false);
        generateMesh(rightMesh[i], h3Edge[i].left, h3Edge[i].right, HSV((float)i / (float)h3Edge.size(), 1.f, 1.f), true);
        
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
