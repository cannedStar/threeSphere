#include "alloutil/al_OmniStereoGraphicsRenderer.hpp"
#include "Cuttlebone/Cuttlebone.hpp"
#include "allocore/graphics/al_Asset.hpp"

#include <iostream>

#include "common.hpp"
#include "transformations.hpp"
#include "particles.hpp"
#include "mesh4D.hpp"
#include "generator.hpp"

using namespace al;
using namespace std;

struct HyperApp : OmniStereoGraphicsRenderer {

  Material material;
  Light light;

  cuttlebone::Taker<State> taker;
  State* state;

  Mat4d camera;
  double theta, epsilon, phi;
  int depth;

  std::vector<Mesh4D> meshes4D;
  Graphics::Primitive prim;

  Texture tex;
  Scene* ascene = 0;
  Vec3f scene_center;
  float scene_scaleInv;

  Generator fig8;

  HyperApp() {
    nav().pos(0.0, 0.0, 0.0);

    initWindow();

    lens().eyeSep(0.03).far(200);

    theta = 0;
    epsilon = 0;
    phi = 0;
    camera.setIdentity();

    SearchPaths sPath;

    sPath.addAppPaths();
    FilePath path = sPath.find("ducky.obj");
    printf("reading %s\n", path.filepath().c_str());

    ascene = Scene::import(path.filepath());

    Vec3f scene_min, scene_max;
    if(ascene==0) {
      printf("error reading %s\n", path.filepath().c_str());
    } else {
      ascene->getBounds(scene_min,scene_max);
      scene_center = (scene_min + scene_max) / 2.f;
      ascene->dump();
    }

    scene_scaleInv = scene_max[0]-scene_min[0];
    scene_scaleInv = al::max(scene_max[1] - scene_min[1],scene_scaleInv);
    scene_scaleInv = al::max(scene_max[2] - scene_min[2],scene_scaleInv);
    scene_scaleInv = 2.f / scene_scaleInv;

    for(unsigned i = 0; i < ascene->meshes(); ++i) {
      Mesh mesh;
      ascene->mesh(i, mesh);

      for(unsigned j = 0; j < mesh.vertices().size(); ++j) {
        Vec3f& v = mesh.vertices()[j];

        v -= scene_center;
        v *= scene_scaleInv;
        v *= 0.5;
      }

      prim = (Graphics::Primitive)mesh.primitive();
      
      Mesh4D mesh4D;
      mesh4D.init(mesh);

      meshes4D.push_back(mesh4D);
    }

    // Image img(sPath.find("hubble.jpg").filepath());
    Image img(sPath.find("hue.png").filepath());
    tex.allocate(img.array());

    // // Figure 8 knot complement
    // Mat4d genA = Mat4d(
    //   1.5, 1.0, 0.0, -0.5,
    //   1.0, 1.0, 0.0, -1.0,
    //   0.0, 0.0, 1.0, 0.0,
    //   0.5, 1.0, 0.0, 0.5);

    // Mat4d genB = Mat4d(
    //   1.5, 0.5, -sqrt(3)/2.0, 0.5,
    //   0.5, 1.0, 0.0, 0.5,
    //   -sqrt(3)/2.0, 0.0, 1.0, -sqrt(3)/2.0,
    //   -0.5, -0.5, sqrt(3)/2.0, 0.5);

    Mat4d genA = 0.5 * Mat4d(
      1, -1, -1, -1,
      1, 1, -1, 1,
      1, 1, 1, -1,
      1, -1, 1, 1);

    Mat4d genB = 0.5 * Mat4d(
      1, -1, -1, 1,
      1, 1, 1, 1,
      1, -1, 1, -1,
      -1, -1, 1, 1);

    fig8.init(genA, genB, 4);
  }

  ~HyperApp() {}

  void onAnimate(double dt) {
    pose.set(nav());
  }

  void onDraw(Graphics& g) {
    material();
    light();

    g.pushMatrix();
      shader().uniform("texture0", 1);
      shader().uniform("texture", 1.0);
      tex.bind(1);
      
      Mesh mesh;
      for(int k = 0; k < fig8.size(); ++k) {
        if (fig8.getDepth(k) > depth) break;
        for(int j = 0; j < meshes4D.size(); ++j) {
          Mesh4D& mesh4D = meshes4D[j];
          mesh.reset();
          mesh.primitive(prim);
          for(int i = 0; i < mesh4D.vertices.size(); ++i) {
            mesh.normal(mesh4D.normals[i]);
            mesh.texCoord(mesh4D.texCoords[i]);

            Vec4d newVert;
            // Vec4d newVert = mesh4D.vertices[i];
            // Mat4d::multiply(newVert, camera * transforms[i].mat, mesh4D.vertices[i]);
            Mat4d::multiply(newVert, camera * fig8.get(k), mesh4D.vertices[i]);
            Vec3d temp = s3tor3(newVert);
            // Vec3d temp = klein(newVert);
            // mesh.vertex(uhs(temp));
            mesh.vertex(temp);
          }
          g.draw(mesh);
        }
      }

      tex.unbind(1);
      shader().uniform("texture", 0.0);
    g.popMatrix();

  }

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

      // case '1': omni().mode(OmniStereo::DUAL).stereo(true); break;
      // case '2': omni().mode(OmniStereo::ANAGLYPH).stereo(true); break;
      default: break;
    }

    camera.setIdentity();
    // camera = rotateTheta(camera, theta);
    // camera = rotateEpsilon(camera, epsilon);
    // camera = rotatePhi(camera, phi);
    // camera = para(camera, epsilon, phi);
    camera = rotate3s(camera, theta, phi);

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
};

int main(int argc, char* argv[]) {
  HyperApp().start();
  return 0;
}