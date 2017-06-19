//Steve Trettel's Hypercube Projection
//
// Dennis Adderton
// Summer 2017 revision 01
// stereoprojection01
/*
*/


#include "al_OmniApp.hpp"
#include "allocore/io/al_App.hpp"
#include <iostream>
using namespace al;
using namespace std;

struct HyperApp : OmniApp {

  Material material;
  Light light;

  Vec4f hypercube_vertices[16];

// Make pointmesh and then make 3-vector.
// Create 32 meshes for lines drawn to indicate edges of hypercube.

  std::vector<Mesh> edgeMesh;
  std::vector<Vec3f> edgePoints[32];

  void generateEdge(Mesh& targetMesh, Vec4f& srcVert1, Vec4f& srcVert2, Color meshColor) {
    targetMesh.primitive(Graphics::LINE_STRIP);
    for (int n = 0; n <= 100; ++n) {
      float t = (float)n / 100.f;

      Vec3f newPoint = Vec3f(
        (srcVert1[1]-srcVert1[1]*t+srcVert2[1]*t) / (srcVert1[0]*(-1+t)-srcVert2[0]*t + sqrt(pow((srcVert1[0]-srcVert1[0]*t+srcVert2[0]*t),2)+pow((srcVert1[1]-srcVert1[1]*t+srcVert2[1]*t),2)+pow((srcVert1[2]-srcVert1[2]*t+srcVert2[2]*t),2)+pow((srcVert1[3]-srcVert1[3]*t+srcVert2[3]*t),2))),
        (srcVert1[2]-srcVert1[2]*t+srcVert2[2]*t) / (srcVert1[0]*(-1+t)-srcVert2[0]*t + sqrt(pow((srcVert1[0]-srcVert1[0]*t+srcVert2[0]*t),2)+pow((srcVert1[1]-srcVert1[1]*t+srcVert2[1]*t),2)+pow((srcVert1[2]-srcVert1[2]*t+srcVert2[2]*t),2)+pow((srcVert1[3]-srcVert1[3]*t+srcVert2[3]*t),2))),
        (srcVert1[3]-srcVert1[3]*t+srcVert2[3]*t) / (srcVert1[0]*(-1+t)-srcVert2[0]*t + sqrt(pow((srcVert1[0]-srcVert1[0]*t+srcVert2[0]*t),2)+pow((srcVert1[1]-srcVert1[1]*t+srcVert2[1]*t),2)+pow((srcVert1[2]-srcVert1[2]*t+srcVert2[2]*t),2)+pow((srcVert1[3]-srcVert1[3]*t+srcVert2[3]*t),2)))
        );                
      targetMesh.vertex(newPoint);
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

    edgeMesh.resize(32);

    // Meshes

    generateEdge(edgeMesh[0], hypercube_vertices[0], hypercube_vertices[1], Color(0.3, 1.0, 0.3, 1.0));
    generateEdge(edgeMesh[1], hypercube_vertices[0], hypercube_vertices[2], Color(1.0, 0.3, 0.3, 1.0));
    generateEdge(edgeMesh[2], hypercube_vertices[0], hypercube_vertices[4], Color(1.0, 1.0, 0.0, 1.0));
    generateEdge(edgeMesh[3], hypercube_vertices[0], hypercube_vertices[8], Color(0.0, 1.0, 1.0, 1.0));
    generateEdge(edgeMesh[4], hypercube_vertices[1], hypercube_vertices[3], Color(0.3, 0.3, 1.0, 1.0));
    generateEdge(edgeMesh[5], hypercube_vertices[1], hypercube_vertices[5], Color(1.0, 0.0, 1.0, 1.0));
    generateEdge(edgeMesh[6], hypercube_vertices[1], hypercube_vertices[9], Color(1.0, 1.0, 0.0, 1.0));
    generateEdge(edgeMesh[7], hypercube_vertices[2], hypercube_vertices[3], Color(0.3, 1.0, 0.3, 1.0));
    generateEdge(edgeMesh[8], hypercube_vertices[2], hypercube_vertices[6], Color(0.3, 0.3, 1.0, 1.0));
    generateEdge(edgeMesh[9], hypercube_vertices[2], hypercube_vertices[10], Color(1.0, 0.3, 0.3, 1.0));
    generateEdge(edgeMesh[10], hypercube_vertices[3], hypercube_vertices[7], Color(0.0, 1.0, 1.0, 1.0));
    generateEdge(edgeMesh[11], hypercube_vertices[3], hypercube_vertices[11], Color(1.0, 0.0, 1.0, 1.0));
    generateEdge(edgeMesh[12], hypercube_vertices[4], hypercube_vertices[5], Color(0.3, 1.0, 0.3, 1.0));
    generateEdge(edgeMesh[13], hypercube_vertices[4], hypercube_vertices[6], Color(1.0, 0.0, 1.0, 1.0));
    generateEdge(edgeMesh[14], hypercube_vertices[4], hypercube_vertices[12], Color(0.0, 1.0, 1.0, 1.0));
    generateEdge(edgeMesh[15], hypercube_vertices[5], hypercube_vertices[7], Color(1.0, 0.3, 0.3, 1.0));
    generateEdge(edgeMesh[16], hypercube_vertices[5], hypercube_vertices[13], Color(0.3, 0.3, 1.0, 1.0));
    generateEdge(edgeMesh[17], hypercube_vertices[6], hypercube_vertices[7], Color(1.0, 0.3, 0.3, 1.0));
    generateEdge(edgeMesh[18], hypercube_vertices[6], hypercube_vertices[14], Color(0.0, 1.0, 1.0, 1.0));
    generateEdge(edgeMesh[19], hypercube_vertices[7], hypercube_vertices[15], Color(1.0, 1.0, 0.0, 1.0));
    generateEdge(edgeMesh[20], hypercube_vertices[8], hypercube_vertices[9], Color(0.3, 1.0, 0.3, 1.0));
    generateEdge(edgeMesh[21], hypercube_vertices[8], hypercube_vertices[10], Color(1.0, 0.0, 1.0, 1.0));
    generateEdge(edgeMesh[22], hypercube_vertices[8], hypercube_vertices[12], Color(0.0, 1.0, 1.0, 1.0));
    generateEdge(edgeMesh[23], hypercube_vertices[9], hypercube_vertices[11], Color(0.3, 0.3, 1.0, 1.0));
    generateEdge(edgeMesh[24], hypercube_vertices[9], hypercube_vertices[13], Color(0.3, 1.0, 0.3, 1.0));
    generateEdge(edgeMesh[25], hypercube_vertices[10], hypercube_vertices[11], Color(1.0, 0.3, 0.3, 1.0));
    generateEdge(edgeMesh[26], hypercube_vertices[10], hypercube_vertices[14], Color(1.0, 1.0, 0.0, 1.0));
    generateEdge(edgeMesh[27], hypercube_vertices[11], hypercube_vertices[15], Color(0.0, 1.0, 1.0, 1.0));
    generateEdge(edgeMesh[28], hypercube_vertices[12], hypercube_vertices[13], Color(0.3, 0.3, 1.0, 1.0));
    generateEdge(edgeMesh[29], hypercube_vertices[12], hypercube_vertices[14], Color(1.0, 0.0, 1.0, 1.0));
    generateEdge(edgeMesh[30], hypercube_vertices[13], hypercube_vertices[15], Color(1.0, 1.0, 0.0, 1.0));
    generateEdge(edgeMesh[31], hypercube_vertices[14], hypercube_vertices[15], Color(0.3, 1.0, 0.3, 1.0));

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
      g.lineWidth(8);
      
      for(int i = 0; i < 32; ++i) {
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
    //keydown(k.key());
    if (k.key() == ' ') { /**/ }
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
