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

// static const int vertNum = 485;
static const int edgeNum = 644;
static const int edgeRes = 128;

struct HyperApp : OmniApp {

  Material material;
  Light light;

  std::vector<Vec4f> h3Vert;
  std::vector<Vec2i> h3Edge;
  std::vector<Mesh> leftMesh, rightMesh;

  float theta, phi;
  float epsilon;
  Mat4f camera;
  Mat4f eye;

  // void generateEdge(std::vector<Vec4f>& tgtEdge, const Vec4f& srcVt1, const Vec4f& srcVt2) {

  //   for (int i = 0; i < edgeRes; ++i) {
  //     float t = (float)i / ((float)edgeRes - 1.f);
  //     Vec4f newPoint = srcVt1 + t * (srcVt2 - srcVt1);

  //     // projection onto S3
  //     tgtEdge[i] = newPoint.normalized();
  //   }
  // }

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

  // add in rotation functions
  /*
  cosh(s), 0, 0, sinh
  0, 1, 0 0
  0, 0, 1, 0
  
  */

  // CONSTRUCTOR
  HyperApp() {

    nav().pos(0.0, 0.0, 0.0);
    light.pos(0, 10.0, 0);
    light.specular(Color(0.1,0.1,0.1));
    light.diffuse(Color(0.2,0.2,0.2));
    light.ambient(Color(1,1,1));
    initWindow();
    initAudio();

    lens().eyeSep(0.0); // set eyeSep to zero

    theta = 0.f;
    camera = Mat4f(
      cos(theta), -sin(theta), 0.f, 0.f,
      sin(theta), cos(theta), 0.f, 0.f,
      0.f, 0.f, cos(theta), -sin(theta),
      0.f, 0.f, sin(theta), cos(theta));

    epsilon = 0.01f;
    eye = Mat4f(
      cos(epsilon), 0.f, 0.f, -sin(epsilon),
      0.f, 1.f, 0.f, 0.f,
      0.f, 0.f, 1.f, 0.f,
      sin(epsilon), 0.f, 0.f, cos(epsilon));

    h3Vert.resize(512);
    h3Edge.resize(700);

    h3Vert.push_back(Vec4f(1, 0, 0, 0));
    h3Vert.push_back(Vec4f(3, -2, -2, 0));
    h3Vert.push_back(Vec4f(3, -2, 2, 0));
    h3Vert.push_back(Vec4f(3, 0, -2, -2));
    h3Vert.push_back(Vec4f(3, 0, 2, -2));
    h3Vert.push_back(Vec4f(3, 2, -2, 0));
    h3Vert.push_back(Vec4f(3, 2, 2, 0));
    h3Vert.push_back(Vec4f(5, -2, -4, -2));
    h3Vert.push_back(Vec4f(5, -2, 4, -2));
    h3Vert.push_back(Vec4f(5, 2, -4, -2));
    h3Vert.push_back(Vec4f(5, 2, 4, -2));
    h3Vert.push_back(Vec4f(9, -8, -4, 0));
    h3Vert.push_back(Vec4f(9, -8, 4, 0));
    h3Vert.push_back(Vec4f(9, -4, -8, 0));
    h3Vert.push_back(Vec4f(9, -4, 8, 0));
    h3Vert.push_back(Vec4f(9, 0, -4, -8));
    h3Vert.push_back(Vec4f(9, 0, 4, -8));
    h3Vert.push_back(Vec4f(9, 4, -8, 0));
    h3Vert.push_back(Vec4f(9, 4, 8, 0));
    h3Vert.push_back(Vec4f(9, 8, -4, 0));
    h3Vert.push_back(Vec4f(9, 8, 4, 0));
    h3Vert.push_back(Vec4f(11, -4, -10, -2));
    h3Vert.push_back(Vec4f(11, -4, 10, -2));
    h3Vert.push_back(Vec4f(11, 4, -10, -2));
    h3Vert.push_back(Vec4f(11, 4, 10, -2));
    h3Vert.push_back(Vec4f(13, -10, -8, -2));
    h3Vert.push_back(Vec4f(13, -10, 8, -2));
    h3Vert.push_back(Vec4f(13, -2, -8, -10));
    h3Vert.push_back(Vec4f(13, -2, 8, -10));
    h3Vert.push_back(Vec4f(13, 2, -8, -10));
    h3Vert.push_back(Vec4f(13, 2, 8, -10));
    h3Vert.push_back(Vec4f(13, 10, -8, -2));
    h3Vert.push_back(Vec4f(13, 10, 8, -2));
    h3Vert.push_back(Vec4f(17, -12, -12, 0));
    h3Vert.push_back(Vec4f(17, -12, 12, 0));
    h3Vert.push_back(Vec4f(17, 12, -12, 0));
    h3Vert.push_back(Vec4f(17, 12, 12, 0));
    h3Vert.push_back(Vec4f(19, -18, 6, 0));
    h3Vert.push_back(Vec4f(19, -16, -10, -2));
    h3Vert.push_back(Vec4f(19, -16, 10, -2));
    h3Vert.push_back(Vec4f(19, -10, -14, -8));
    h3Vert.push_back(Vec4f(19, -10, 14, -8));
    h3Vert.push_back(Vec4f(19, -8, -14, -10));
    h3Vert.push_back(Vec4f(19, -8, 14, -10));
    h3Vert.push_back(Vec4f(19, -6, -18, 0));
    h3Vert.push_back(Vec4f(19, -6, 18, 0));
    h3Vert.push_back(Vec4f(19, -2, -10, -16));
    h3Vert.push_back(Vec4f(19, -2, 10, -16));
    h3Vert.push_back(Vec4f(19, 0, -6, -18));
    h3Vert.push_back(Vec4f(19, 0, 6, -18));
    h3Vert.push_back(Vec4f(19, 2, -10, -16));
    h3Vert.push_back(Vec4f(19, 2, 10, -16));
    h3Vert.push_back(Vec4f(19, 6, -18, 0));
    h3Vert.push_back(Vec4f(19, 6, 18, 0));
    h3Vert.push_back(Vec4f(19, 8, -14, -10));
    h3Vert.push_back(Vec4f(19, 8, 14, -10));
    h3Vert.push_back(Vec4f(19, 10, -14, -8));
    h3Vert.push_back(Vec4f(19, 10, 14, -8));
    h3Vert.push_back(Vec4f(19, 16, -10, -2));
    h3Vert.push_back(Vec4f(19, 16, 10, -2));
    h3Vert.push_back(Vec4f(19, 18, 6, 0));
    h3Vert.push_back(Vec4f(21, -6, -20, -2));
    h3Vert.push_back(Vec4f(21, -6, 20, -2));
    h3Vert.push_back(Vec4f(21, 6, -20, -2));
    h3Vert.push_back(Vec4f(21, 6, 20, -2));
    h3Vert.push_back(Vec4f(27, -20, -18, -2));
    h3Vert.push_back(Vec4f(27, -20, 18, -2));
    h3Vert.push_back(Vec4f(27, -12, -22, -10));
    h3Vert.push_back(Vec4f(27, -12, 22, -10));
    h3Vert.push_back(Vec4f(27, 12, -22, -10));
    h3Vert.push_back(Vec4f(27, 12, 22, -10));
    h3Vert.push_back(Vec4f(27, 20, -18, -2));
    h3Vert.push_back(Vec4f(27, 20, 18, -2));
    h3Vert.push_back(Vec4f(33, -16, -24, -16));
    h3Vert.push_back(Vec4f(33, -16, 24, -16));
    h3Vert.push_back(Vec4f(33, -8, 32, 0));
    h3Vert.push_back(Vec4f(33, 0, -8, -32));
    h3Vert.push_back(Vec4f(33, 0, 8, -32));
    h3Vert.push_back(Vec4f(33, 8, 32, 0));
    h3Vert.push_back(Vec4f(33, 16, -24, -16));
    h3Vert.push_back(Vec4f(33, 16, 24, -16));
    h3Vert.push_back(Vec4f(35, -32, -14, -2));
    h3Vert.push_back(Vec4f(35, -32, 14, -2));
    h3Vert.push_back(Vec4f(35, -30, -18, 0));
    h3Vert.push_back(Vec4f(35, -30, 18, 0));
    h3Vert.push_back(Vec4f(35, -26, -22, -8));
    h3Vert.push_back(Vec4f(35, -26, 22, -8));
    h3Vert.push_back(Vec4f(35, -18, -30, 0));
    h3Vert.push_back(Vec4f(35, -18, 30, 0));
    h3Vert.push_back(Vec4f(35, -8, -34, -2));
    h3Vert.push_back(Vec4f(35, -8, -22, -26));
    h3Vert.push_back(Vec4f(35, -8, 22, -26));
    h3Vert.push_back(Vec4f(35, -8, 34, -2));
    h3Vert.push_back(Vec4f(35, -2, -14, -32));
    h3Vert.push_back(Vec4f(35, -2, 14, -32));
    h3Vert.push_back(Vec4f(35, 2, -14, -32));
    h3Vert.push_back(Vec4f(35, 2, 14, -32));
    h3Vert.push_back(Vec4f(35, 8, -34, -2));
    h3Vert.push_back(Vec4f(35, 8, -22, -26));
    h3Vert.push_back(Vec4f(35, 8, 22, -26));
    h3Vert.push_back(Vec4f(35, 8, 34, -2));
    h3Vert.push_back(Vec4f(35, 18, -30, 0));
    h3Vert.push_back(Vec4f(35, 18, 30, 0));
    h3Vert.push_back(Vec4f(35, 26, -22, -8));
    h3Vert.push_back(Vec4f(35, 26, 22, -8));
    h3Vert.push_back(Vec4f(35, 30, -18, 0));
    h3Vert.push_back(Vec4f(35, 30, 18, 0));
    h3Vert.push_back(Vec4f(35, 32, -14, -2));
    h3Vert.push_back(Vec4f(35, 32, 14, -2));
    h3Vert.push_back(Vec4f(41, -20, -32, -16));
    h3Vert.push_back(Vec4f(41, -20, 32, -16));
    h3Vert.push_back(Vec4f(41, 20, -32, -16));
    h3Vert.push_back(Vec4f(41, 20, 32, -16));
    h3Vert.push_back(Vec4f(43, -20, -38, -2));
    h3Vert.push_back(Vec4f(43, -20, 38, -2));
    h3Vert.push_back(Vec4f(43, -4, -26, -34));
    h3Vert.push_back(Vec4f(43, -4, 26, -34));
    h3Vert.push_back(Vec4f(43, 4, -26, -34));
    h3Vert.push_back(Vec4f(43, 4, 26, -34));
    h3Vert.push_back(Vec4f(43, 20, -38, -2));
    h3Vert.push_back(Vec4f(43, 20, 38, -2));
    h3Vert.push_back(Vec4f(45, -38, 24, -2));
    h3Vert.push_back(Vec4f(45, -26, -32, -18));
    h3Vert.push_back(Vec4f(45, -26, 32, -18));
    h3Vert.push_back(Vec4f(45, -18, 40, -10));
    h3Vert.push_back(Vec4f(45, -2, -16, -42));
    h3Vert.push_back(Vec4f(45, -2, 16, -42));
    h3Vert.push_back(Vec4f(45, 2, -16, -42));
    h3Vert.push_back(Vec4f(45, 2, 16, -42));
    h3Vert.push_back(Vec4f(45, 18, 40, -10));
    h3Vert.push_back(Vec4f(45, 26, -32, -18));
    h3Vert.push_back(Vec4f(45, 26, 32, -18));
    h3Vert.push_back(Vec4f(45, 38, 24, -2));
    h3Vert.push_back(Vec4f(49, -20, -44, -8));
    h3Vert.push_back(Vec4f(49, -20, 44, -8));
    h3Vert.push_back(Vec4f(49, -4, -28, -40));
    h3Vert.push_back(Vec4f(49, -4, 28, -40));
    h3Vert.push_back(Vec4f(49, 4, -28, -40));
    h3Vert.push_back(Vec4f(49, 4, 28, -40));
    h3Vert.push_back(Vec4f(49, 20, -44, -8));
    h3Vert.push_back(Vec4f(49, 20, 44, -8));
    h3Vert.push_back(Vec4f(51, -46, 22, 0));
    h3Vert.push_back(Vec4f(51, -40, -30, -10));
    h3Vert.push_back(Vec4f(51, -40, 30, -10));
    h3Vert.push_back(Vec4f(51, -38, -34, 0));
    h3Vert.push_back(Vec4f(51, -38, 34, 0));
    h3Vert.push_back(Vec4f(51, -34, 38, 0));
    h3Vert.push_back(Vec4f(51, -22, -46, 0));
    h3Vert.push_back(Vec4f(51, -22, 46, 0));
    h3Vert.push_back(Vec4f(51, -10, -30, -40));
    h3Vert.push_back(Vec4f(51, -10, 30, -40));
    h3Vert.push_back(Vec4f(51, -10, 50, 0));
    h3Vert.push_back(Vec4f(51, 0, -10, -50));
    h3Vert.push_back(Vec4f(51, 0, 10, -50));
    h3Vert.push_back(Vec4f(51, 10, -30, -40));
    h3Vert.push_back(Vec4f(51, 10, 30, -40));
    h3Vert.push_back(Vec4f(51, 10, 50, 0));
    h3Vert.push_back(Vec4f(51, 22, -46, 0));
    h3Vert.push_back(Vec4f(51, 22, 46, 0));
    h3Vert.push_back(Vec4f(51, 34, 38, 0));
    h3Vert.push_back(Vec4f(51, 38, -34, 0));
    h3Vert.push_back(Vec4f(51, 38, 34, 0));
    h3Vert.push_back(Vec4f(51, 40, -30, -10));
    h3Vert.push_back(Vec4f(51, 40, 30, -10));
    h3Vert.push_back(Vec4f(51, 46, 22, 0));
    h3Vert.push_back(Vec4f(59, -44, -38, -10));
    h3Vert.push_back(Vec4f(59, -44, 38, -10));
    h3Vert.push_back(Vec4f(59, 44, -38, -10));
    h3Vert.push_back(Vec4f(59, 44, 38, -10));
    h3Vert.push_back(Vec4f(61, -46, 40, -2));
    h3Vert.push_back(Vec4f(61, -22, 56, -10));
    h3Vert.push_back(Vec4f(61, 22, 56, -10));
    h3Vert.push_back(Vec4f(61, 46, 40, -2));
    h3Vert.push_back(Vec4f(65, -56, 32, -8));
    h3Vert.push_back(Vec4f(65, -8, 32, -56));
    h3Vert.push_back(Vec4f(65, 8, 32, -56));
    h3Vert.push_back(Vec4f(65, 56, 32, -8));
    h3Vert.push_back(Vec4f(69, -50, 44, -18));
    h3Vert.push_back(Vec4f(69, -34, 60, -2));
    h3Vert.push_back(Vec4f(69, -30, 52, -34));
    h3Vert.push_back(Vec4f(69, -10, -36, -58));
    h3Vert.push_back(Vec4f(69, -10, 36, -58));
    h3Vert.push_back(Vec4f(69, -2, 20, -66));
    h3Vert.push_back(Vec4f(69, 2, 20, -66));
    h3Vert.push_back(Vec4f(69, 10, -36, -58));
    h3Vert.push_back(Vec4f(69, 10, 36, -58));
    h3Vert.push_back(Vec4f(69, 30, 52, -34));
    h3Vert.push_back(Vec4f(69, 34, 60, -2));
    h3Vert.push_back(Vec4f(69, 50, 44, -18));
    h3Vert.push_back(Vec4f(73, -56, 44, -16));
    h3Vert.push_back(Vec4f(73, -40, 52, -32));
    h3Vert.push_back(Vec4f(73, -32, 52, -40));
    h3Vert.push_back(Vec4f(73, -16, 44, -56));
    h3Vert.push_back(Vec4f(73, 16, 44, -56));
    h3Vert.push_back(Vec4f(73, 32, 52, -40));
    h3Vert.push_back(Vec4f(73, 40, 52, -32));
    h3Vert.push_back(Vec4f(73, 56, 44, -16));
    h3Vert.push_back(Vec4f(75, -52, -54, -2));
    h3Vert.push_back(Vec4f(75, -52, 54, -2));
    h3Vert.push_back(Vec4f(75, -12, -46, -58));
    h3Vert.push_back(Vec4f(75, -12, 46, -58));
    h3Vert.push_back(Vec4f(75, 12, -46, -58));
    h3Vert.push_back(Vec4f(75, 12, 46, -58));
    h3Vert.push_back(Vec4f(75, 52, -54, -2));
    h3Vert.push_back(Vec4f(75, 52, 54, -2));
    h3Vert.push_back(Vec4f(81, -60, 52, -16));
    h3Vert.push_back(Vec4f(81, -44, 60, -32));
    h3Vert.push_back(Vec4f(81, 44, 60, -32));
    h3Vert.push_back(Vec4f(81, 60, 52, -16));
    h3Vert.push_back(Vec4f(83, -50, -58, -32));
    h3Vert.push_back(Vec4f(83, -50, 58, -32));
    h3Vert.push_back(Vec4f(83, -38, -62, -40));
    h3Vert.push_back(Vec4f(83, -38, 62, -40));
    h3Vert.push_back(Vec4f(83, -34, -74, -16));
    h3Vert.push_back(Vec4f(83, -34, 74, -16));
    h3Vert.push_back(Vec4f(83, -2, -22, -80));
    h3Vert.push_back(Vec4f(83, -2, 22, -80));
    h3Vert.push_back(Vec4f(83, 2, -22, -80));
    h3Vert.push_back(Vec4f(83, 2, 22, -80));
    h3Vert.push_back(Vec4f(83, 34, -74, -16));
    h3Vert.push_back(Vec4f(83, 34, 74, -16));
    h3Vert.push_back(Vec4f(83, 38, -62, -40));
    h3Vert.push_back(Vec4f(83, 38, 62, -40));
    h3Vert.push_back(Vec4f(83, 50, -58, -32));
    h3Vert.push_back(Vec4f(83, 50, 58, -32));
    h3Vert.push_back(Vec4f(85, -38, 68, -34));
    h3Vert.push_back(Vec4f(85, -38, 76, -2));
    h3Vert.push_back(Vec4f(85, 38, 68, -34));
    h3Vert.push_back(Vec4f(85, 38, 76, -2));
    h3Vert.push_back(Vec4f(91, -76, 50, -2));
    h3Vert.push_back(Vec4f(91, -28, 86, -10));
    h3Vert.push_back(Vec4f(91, 28, 86, -10));
    h3Vert.push_back(Vec4f(91, 76, 50, -2));
    h3Vert.push_back(Vec4f(93, -30, 88, -2));
    h3Vert.push_back(Vec4f(93, -6, 56, -74));
    h3Vert.push_back(Vec4f(93, 6, 56, -74));
    h3Vert.push_back(Vec4f(93, 30, 88, -2));
    h3Vert.push_back(Vec4f(99, -70, 70, 0));
    h3Vert.push_back(Vec4f(99, -56, -70, -42));
    h3Vert.push_back(Vec4f(99, -56, 70, -42));
    h3Vert.push_back(Vec4f(99, -46, -78, -40));
    h3Vert.push_back(Vec4f(99, -46, 78, -40));
    h3Vert.push_back(Vec4f(99, -42, 70, -56));
    h3Vert.push_back(Vec4f(99, -38, -90, -16));
    h3Vert.push_back(Vec4f(99, -38, 90, -16));
    h3Vert.push_back(Vec4f(99, -30, -94, -8));
    h3Vert.push_back(Vec4f(99, -30, 94, -8));
    h3Vert.push_back(Vec4f(99, -6, -58, -80));
    h3Vert.push_back(Vec4f(99, -6, 58, -80));
    h3Vert.push_back(Vec4f(99, 6, -58, -80));
    h3Vert.push_back(Vec4f(99, 6, 58, -80));
    h3Vert.push_back(Vec4f(99, 30, -94, -8));
    h3Vert.push_back(Vec4f(99, 30, 94, -8));
    h3Vert.push_back(Vec4f(99, 38, -90, -16));
    h3Vert.push_back(Vec4f(99, 38, 90, -16));
    h3Vert.push_back(Vec4f(99, 42, 70, -56));
    h3Vert.push_back(Vec4f(99, 46, -78, -40));
    h3Vert.push_back(Vec4f(99, 46, 78, -40));
    h3Vert.push_back(Vec4f(99, 56, -70, -42));
    h3Vert.push_back(Vec4f(99, 56, 70, -42));
    h3Vert.push_back(Vec4f(99, 70, 70, 0));
    h3Vert.push_back(Vec4f(105, -76, 72, -8));
    h3Vert.push_back(Vec4f(105, -12, 56, -88));
    h3Vert.push_back(Vec4f(105, 12, 56, -88));
    h3Vert.push_back(Vec4f(105, 76, 72, -8));
    h3Vert.push_back(Vec4f(107, -60, -78, -42));
    h3Vert.push_back(Vec4f(107, -60, 78, -42));
    h3Vert.push_back(Vec4f(107, 60, -78, -42));
    h3Vert.push_back(Vec4f(107, 60, 78, -42));
    h3Vert.push_back(Vec4f(113, -52, 92, -40));
    h3Vert.push_back(Vec4f(113, -52, 100, -8));
    h3Vert.push_back(Vec4f(113, -20, 68, -88));
    h3Vert.push_back(Vec4f(113, -4, 44, -104));
    h3Vert.push_back(Vec4f(113, 4, 44, -104));
    h3Vert.push_back(Vec4f(113, 20, 68, -88));
    h3Vert.push_back(Vec4f(113, 52, 92, -40));
    h3Vert.push_back(Vec4f(113, 52, 100, -8));
    h3Vert.push_back(Vec4f(115, -98, 58, -16));
    h3Vert.push_back(Vec4f(115, -88, 74, -2));
    h3Vert.push_back(Vec4f(115, -82, 74, -32));
    h3Vert.push_back(Vec4f(115, -58, 82, -56));
    h3Vert.push_back(Vec4f(115, -56, -82, -58));
    h3Vert.push_back(Vec4f(115, -56, 82, -58));
    h3Vert.push_back(Vec4f(115, -32, 110, -10));
    h3Vert.push_back(Vec4f(115, -16, -58, -98));
    h3Vert.push_back(Vec4f(115, -16, 58, -98));
    h3Vert.push_back(Vec4f(115, -2, 26, -112));
    h3Vert.push_back(Vec4f(115, 2, 26, -112));
    h3Vert.push_back(Vec4f(115, 16, -58, -98));
    h3Vert.push_back(Vec4f(115, 16, 58, -98));
    h3Vert.push_back(Vec4f(115, 32, 110, -10));
    h3Vert.push_back(Vec4f(115, 56, -82, -58));
    h3Vert.push_back(Vec4f(115, 56, 82, -58));
    h3Vert.push_back(Vec4f(115, 58, 82, -56));
    h3Vert.push_back(Vec4f(115, 82, 74, -32));
    h3Vert.push_back(Vec4f(115, 88, 74, -2));
    h3Vert.push_back(Vec4f(115, 98, 58, -16));
    h3Vert.push_back(Vec4f(123, -52, -110, -18));
    h3Vert.push_back(Vec4f(123, -52, 110, -18));
    h3Vert.push_back(Vec4f(123, -4, -46, -114));
    h3Vert.push_back(Vec4f(123, -4, 46, -114));
    h3Vert.push_back(Vec4f(123, 4, -46, -114));
    h3Vert.push_back(Vec4f(123, 4, 46, -114));
    h3Vert.push_back(Vec4f(123, 52, -110, -18));
    h3Vert.push_back(Vec4f(123, 52, 110, -18));
    h3Vert.push_back(Vec4f(133, -26, 76, -106));
    h3Vert.push_back(Vec4f(133, -10, 52, -122));
    h3Vert.push_back(Vec4f(133, 10, 52, -122));
    h3Vert.push_back(Vec4f(133, 26, 76, -106));
    h3Vert.push_back(Vec4f(147, -128, 70, -18));
    h3Vert.push_back(Vec4f(147, -126, 74, -16));
    h3Vert.push_back(Vec4f(147, -74, 114, -56));
    h3Vert.push_back(Vec4f(147, -8, 50, -138));
    h3Vert.push_back(Vec4f(147, 8, 50, -138));
    h3Vert.push_back(Vec4f(147, 74, 114, -56));
    h3Vert.push_back(Vec4f(147, 126, 74, -16));
    h3Vert.push_back(Vec4f(147, 128, 70, -18));
    h3Vert.push_back(Vec4f(155, -76, -122, -58));
    h3Vert.push_back(Vec4f(155, -76, 122, -58));
    h3Vert.push_back(Vec4f(155, -68, 118, -74));
    h3Vert.push_back(Vec4f(155, -52, 146, -2));
    h3Vert.push_back(Vec4f(155, -20, -82, -130));
    h3Vert.push_back(Vec4f(155, -20, 82, -130));
    h3Vert.push_back(Vec4f(155, 20, -82, -130));
    h3Vert.push_back(Vec4f(155, 20, 82, -130));
    h3Vert.push_back(Vec4f(155, 52, 146, -2));
    h3Vert.push_back(Vec4f(155, 68, 118, -74));
    h3Vert.push_back(Vec4f(155, 76, -122, -58));
    h3Vert.push_back(Vec4f(155, 76, 122, -58));
    h3Vert.push_back(Vec4f(163, -120, 102, -42));
    h3Vert.push_back(Vec4f(163, -96, 114, -66));
    h3Vert.push_back(Vec4f(163, -80, 142, -2));
    h3Vert.push_back(Vec4f(163, -74, 134, -56));
    h3Vert.push_back(Vec4f(163, -70, 118, -88));
    h3Vert.push_back(Vec4f(163, -64, 146, -34));
    h3Vert.push_back(Vec4f(163, -40, 158, -2));
    h3Vert.push_back(Vec4f(163, -26, -86, -136));
    h3Vert.push_back(Vec4f(163, -26, 86, -136));
    h3Vert.push_back(Vec4f(163, -10, -58, -152));
    h3Vert.push_back(Vec4f(163, -10, 58, -152));
    h3Vert.push_back(Vec4f(163, -8, 98, -130));
    h3Vert.push_back(Vec4f(163, 8, 98, -130));
    h3Vert.push_back(Vec4f(163, 10, -58, -152));
    h3Vert.push_back(Vec4f(163, 10, 58, -152));
    h3Vert.push_back(Vec4f(163, 26, -86, -136));
    h3Vert.push_back(Vec4f(163, 26, 86, -136));
    h3Vert.push_back(Vec4f(163, 40, 158, -2));
    h3Vert.push_back(Vec4f(163, 64, 146, -34));
    h3Vert.push_back(Vec4f(163, 70, 118, -88));
    h3Vert.push_back(Vec4f(163, 74, 134, -56));
    h3Vert.push_back(Vec4f(163, 80, 142, -2));
    h3Vert.push_back(Vec4f(163, 96, 114, -66));
    h3Vert.push_back(Vec4f(163, 120, 102, -42));
    h3Vert.push_back(Vec4f(171, -124, 110, -42));
    h3Vert.push_back(Vec4f(171, -100, 122, -66));
    h3Vert.push_back(Vec4f(171, 100, 122, -66));
    h3Vert.push_back(Vec4f(171, 124, 110, -42));
    h3Vert.push_back(Vec4f(179, -130, 122, -16));
    h3Vert.push_back(Vec4f(179, -86, 130, -88));
    h3Vert.push_back(Vec4f(179, -80, 142, -74));
    h3Vert.push_back(Vec4f(179, -74, 158, -40));
    h3Vert.push_back(Vec4f(179, -56, 170, -2));
    h3Vert.push_back(Vec4f(179, -38, 110, -136));
    h3Vert.push_back(Vec4f(179, 38, 110, -136));
    h3Vert.push_back(Vec4f(179, 56, 170, -2));
    h3Vert.push_back(Vec4f(179, 74, 158, -40));
    h3Vert.push_back(Vec4f(179, 80, 142, -74));
    h3Vert.push_back(Vec4f(179, 86, 130, -88));
    h3Vert.push_back(Vec4f(179, 130, 122, -16));
    h3Vert.push_back(Vec4f(195, -166, 102, -8));
    h3Vert.push_back(Vec4f(195, -150, 118, -40));
    h3Vert.push_back(Vec4f(195, -88, 138, -106));
    h3Vert.push_back(Vec4f(195, -88, 174, -2));
    h3Vert.push_back(Vec4f(195, -82, 174, -32));
    h3Vert.push_back(Vec4f(195, -72, 178, -34));
    h3Vert.push_back(Vec4f(195, -18, 110, -160));
    h3Vert.push_back(Vec4f(195, -16, 78, -178));
    h3Vert.push_back(Vec4f(195, 16, 78, -178));
    h3Vert.push_back(Vec4f(195, 18, 110, -160));
    h3Vert.push_back(Vec4f(195, 72, 178, -34));
    h3Vert.push_back(Vec4f(195, 82, 174, -32));
    h3Vert.push_back(Vec4f(195, 88, 138, -106));
    h3Vert.push_back(Vec4f(195, 88, 174, -2));
    h3Vert.push_back(Vec4f(195, 150, 118, -40));
    h3Vert.push_back(Vec4f(195, 166, 102, -8));
    h3Vert.push_back(Vec4f(201, -20, 120, -160));
    h3Vert.push_back(Vec4f(201, 20, 120, -160));
    h3Vert.push_back(Vec4f(211, -158, 134, -40));
    h3Vert.push_back(Vec4f(211, -86, 190, -32));
    h3Vert.push_back(Vec4f(211, 86, 190, -32));
    h3Vert.push_back(Vec4f(211, 158, 134, -40));
    h3Vert.push_back(Vec4f(219, -100, 194, -18));
    h3Vert.push_back(Vec4f(219, -20, -126, -178));
    h3Vert.push_back(Vec4f(219, -20, 126, -178));
    h3Vert.push_back(Vec4f(219, -4, 62, -210));
    h3Vert.push_back(Vec4f(219, 4, 62, -210));
    h3Vert.push_back(Vec4f(219, 20, -126, -178));
    h3Vert.push_back(Vec4f(219, 20, 126, -178));
    h3Vert.push_back(Vec4f(219, 100, 194, -18));
    h3Vert.push_back(Vec4f(227, -166, 154, -16));
    h3Vert.push_back(Vec4f(227, -110,178, -88));
    h3Vert.push_back(Vec4f(227, 110, 178, -88));
    h3Vert.push_back(Vec4f(227, 166, 154, -16));
    h3Vert.push_back(Vec4f(243, -134, 174, -104));
    h3Vert.push_back(Vec4f(243, -122, 210, -8));
    h3Vert.push_back(Vec4f(243, -114, 214, -16));
    h3Vert.push_back(Vec4f(243, -110,198, -88));
    h3Vert.push_back(Vec4f(243, -78, 230, -8));
    h3Vert.push_back(Vec4f(243, -56, 146, -186));
    h3Vert.push_back(Vec4f(243, -32, 118, -210));
    h3Vert.push_back(Vec4f(243, -30, 122, -208));
    h3Vert.push_back(Vec4f(243, -6, 94, -224));
    h3Vert.push_back(Vec4f(243, 6, 94, -224));
    h3Vert.push_back(Vec4f(243, 30, 122, -208));
    h3Vert.push_back(Vec4f(243, 32, 118, -210));
    h3Vert.push_back(Vec4f(243, 56, 146, -186));
    h3Vert.push_back(Vec4f(243, 78, 230, -8));
    h3Vert.push_back(Vec4f(243, 110, 198, -88));
    h3Vert.push_back(Vec4f(243, 114, 214, -16));
    h3Vert.push_back(Vec4f(243, 122, 210, -8));
    h3Vert.push_back(Vec4f(243, 134, 174, -104));
    h3Vert.push_back(Vec4f(251, -180, 174, -18));
    h3Vert.push_back(Vec4f(251, -12, 90, -234));
    h3Vert.push_back(Vec4f(251, 12, 90, -234));
    h3Vert.push_back(Vec4f(251, 180, 174, -18));
    h3Vert.push_back(Vec4f(259, -200, 154, -58));
    h3Vert.push_back(Vec4f(259, -142, 190, -104));
    h3Vert.push_back(Vec4f(259, -118, 230, -16));
    h3Vert.push_back(Vec4f(259, -94, 238, -40));
    h3Vert.push_back(Vec4f(259, -50, 146, -208));
    h3Vert.push_back(Vec4f(259, -46, 158, -200));
    h3Vert.push_back(Vec4f(259, -40, 134, -218));
    h3Vert.push_back(Vec4f(259, -10, 74, -248));
    h3Vert.push_back(Vec4f(259, 10, 74, -248));
    h3Vert.push_back(Vec4f(259, 40, 134, -218));
    h3Vert.push_back(Vec4f(259, 46, 158, -200));
    h3Vert.push_back(Vec4f(259, 50, 146, -208));
    h3Vert.push_back(Vec4f(259, 94, 238, -40));
    h3Vert.push_back(Vec4f(259, 118, 230, -16));
    h3Vert.push_back(Vec4f(259, 142, 190, -104));
    h3Vert.push_back(Vec4f(259, 200, 154, -58));
    h3Vert.push_back(Vec4f(275, -206, 182, -8));
    h3Vert.push_back(Vec4f(275, -152, 194, -122));
    h3Vert.push_back(Vec4f(275, -56, 158, -218));
    h3Vert.push_back(Vec4f(275, -22, 158, -224));
    h3Vert.push_back(Vec4f(275, 22, 158, -224));
    h3Vert.push_back(Vec4f(275, 56, 158, -218));
    h3Vert.push_back(Vec4f(275, 152, 194, -122));
    h3Vert.push_back(Vec4f(275, 206, 182, -8));
    h3Vert.push_back(Vec4f(299, -220, 194, -58));
    h3Vert.push_back(Vec4f(299, -140, 242, -106));
    h3Vert.push_back(Vec4f(299, -44, 158, -250));
    h3Vert.push_back(Vec4f(299, -20, 118, -274));
    h3Vert.push_back(Vec4f(299, 20, 118, -274));
    h3Vert.push_back(Vec4f(299, 44, 158, -250));
    h3Vert.push_back(Vec4f(299, 140, 242, -106));
    h3Vert.push_back(Vec4f(299, 220, 194, -58));
    h3Vert.push_back(Vec4f(315, -172, 234, -122));
    h3Vert.push_back(Vec4f(315, -60, 182, -250));
    h3Vert.push_back(Vec4f(315, 60, 182, -250));
    h3Vert.push_back(Vec4f(315, 172, 234, -122));
    h3Vert.push_back(Vec4f(323, -142, 242, -160));
    h3Vert.push_back(Vec4f(323, -142, 290, -8));
    h3Vert.push_back(Vec4f(323, -38, 170, -272));
    h3Vert.push_back(Vec4f(323, -34, 194, -256));
    h3Vert.push_back(Vec4f(323, 34, 194, -256));
    h3Vert.push_back(Vec4f(323, 38, 170, -272));
    h3Vert.push_back(Vec4f(323, 142, 242, -160));
    h3Vert.push_back(Vec4f(323, 142, 290, -8));
    h3Vert.push_back(Vec4f(339, -26, 130, -312));
    h3Vert.push_back(Vec4f(339, 26, 130, -312));
    h3Vert.push_back(Vec4f(403, -182, 322, -160));
    h3Vert.push_back(Vec4f(403, -38, 242, -320));
    h3Vert.push_back(Vec4f(403, 38, 242, -320));
    h3Vert.push_back(Vec4f(403, 182, 322, -160));
    h3Vert.push_back(Vec4f(443, -76, 266, -346));
    h3Vert.push_back(Vec4f(443, -52, 238, -370));
    h3Vert.push_back(Vec4f(443, 52, 238, -370));
    h3Vert.push_back(Vec4f(443, 76, 266, -346));
    h3Vert.push_back(Vec4f(451, -30, 270, -360));
    h3Vert.push_back(Vec4f(451, 30, 270, -360));
    h3Vert.push_back(Vec4f(523, -52, 310, -418));
    h3Vert.push_back(Vec4f(523, -20, 202, -482));
    h3Vert.push_back(Vec4f(523, 20, 202, -482));
    h3Vert.push_back(Vec4f(523, 52, 310, -418));

    h3Edge.push_back(Vec4f(1, 3));
    h3Edge.push_back(Vec4f(1, 4));
    h3Edge.push_back(Vec4f(1, 5));
    h3Edge.push_back(Vec4f(1, 7));
    h3Edge.push_back(Vec4f(2, 8));
    h3Edge.push_back(Vec4f(2, 12));
    h3Edge.push_back(Vec4f(2, 26));
    h3Edge.push_back(Vec4f(2, 34));
    h3Edge.push_back(Vec4f(3, 1));
    h3Edge.push_back(Vec4f(3, 9));
    h3Edge.push_back(Vec4f(3, 15));
    h3Edge.push_back(Vec4f(3, 27));
    h3Edge.push_back(Vec4f(4, 1));
    h3Edge.push_back(Vec4f(4, 8));
    h3Edge.push_back(Vec4f(4, 10));
    h3Edge.push_back(Vec4f(4, 16));
    h3Edge.push_back(Vec4f(5, 1));
    h3Edge.push_back(Vec4f(5, 17));
    h3Edge.push_back(Vec4f(5, 29));
    h3Edge.push_back(Vec4f(5, 31));
    h3Edge.push_back(Vec4f(6, 10));
    h3Edge.push_back(Vec4f(6, 20));
    h3Edge.push_back(Vec4f(6, 32));
    h3Edge.push_back(Vec4f(6, 36));
    h3Edge.push_back(Vec4f(7, 1));
    h3Edge.push_back(Vec4f(7, 11));
    h3Edge.push_back(Vec4f(7, 19));
    h3Edge.push_back(Vec4f(7, 33));
    h3Edge.push_back(Vec4f(8, 2));
    h3Edge.push_back(Vec4f(8, 4));
    h3Edge.push_back(Vec4f(8, 22));
    h3Edge.push_back(Vec4f(8, 41));
    h3Edge.push_back(Vec4f(9, 3));
    h3Edge.push_back(Vec4f(9, 42));
    h3Edge.push_back(Vec4f(9, 44));
    h3Edge.push_back(Vec4f(9, 69));
    h3Edge.push_back(Vec4f(10, 4));
    h3Edge.push_back(Vec4f(10, 6));
    h3Edge.push_back(Vec4f(10, 24));
    h3Edge.push_back(Vec4f(10, 57));
    h3Edge.push_back(Vec4f(11, 7));
    h3Edge.push_back(Vec4f(11, 56));
    h3Edge.push_back(Vec4f(11, 58));
    h3Edge.push_back(Vec4f(11, 71));
    h3Edge.push_back(Vec4f(12, 2));
    h3Edge.push_back(Vec4f(13, 40));
    h3Edge.push_back(Vec4f(14, 22));
    h3Edge.push_back(Vec4f(15, 3));
    h3Edge.push_back(Vec4f(15, 23));
    h3Edge.push_back(Vec4f(15, 46));
    h3Edge.push_back(Vec4f(15, 115));
    h3Edge.push_back(Vec4f(16, 4));
    h3Edge.push_back(Vec4f(16, 47));
    h3Edge.push_back(Vec4f(16, 49));
    h3Edge.push_back(Vec4f(16, 51));
    h3Edge.push_back(Vec4f(17, 5));
    h3Edge.push_back(Vec4f(17, 50));
    h3Edge.push_back(Vec4f(17, 95));
    h3Edge.push_back(Vec4f(17, 97));
    h3Edge.push_back(Vec4f(18, 24));
    h3Edge.push_back(Vec4f(19, 7));
    h3Edge.push_back(Vec4f(19, 25));
    h3Edge.push_back(Vec4f(19, 54));
    h3Edge.push_back(Vec4f(19, 121));
    h3Edge.push_back(Vec4f(20, 6));
    h3Edge.push_back(Vec4f(21, 60));
    h3Edge.push_back(Vec4f(22, 8));
    h3Edge.push_back(Vec4f(22, 14));
    h3Edge.push_back(Vec4f(22, 62));
    h3Edge.push_back(Vec4f(22, 134));
    h3Edge.push_back(Vec4f(23, 15));
    h3Edge.push_back(Vec4f(23, 125));
    h3Edge.push_back(Vec4f(23, 135));
    h3Edge.push_back(Vec4f(23, 171));
    h3Edge.push_back(Vec4f(24, 10));
    h3Edge.push_back(Vec4f(24, 18));
    h3Edge.push_back(Vec4f(24, 64));
    h3Edge.push_back(Vec4f(24, 140));
    h3Edge.push_back(Vec4f(25, 19));
    h3Edge.push_back(Vec4f(25, 130));
    h3Edge.push_back(Vec4f(25, 141));
    h3Edge.push_back(Vec4f(25, 172));
    h3Edge.push_back(Vec4f(26, 2));
    h3Edge.push_back(Vec4f(27, 3));
    h3Edge.push_back(Vec4f(27, 40));
    h3Edge.push_back(Vec4f(27, 67));
    h3Edge.push_back(Vec4f(27, 87));
    h3Edge.push_back(Vec4f(28, 47));
    h3Edge.push_back(Vec4f(29, 5));
    h3Edge.push_back(Vec4f(29, 48));
    h3Edge.push_back(Vec4f(29, 117));
    h3Edge.push_back(Vec4f(29, 151));
    h3Edge.push_back(Vec4f(30, 51));
    h3Edge.push_back(Vec4f(31, 5));
    h3Edge.push_back(Vec4f(31, 52));
    h3Edge.push_back(Vec4f(31, 119));
    h3Edge.push_back(Vec4f(31, 156));
    h3Edge.push_back(Vec4f(32, 6));
    h3Edge.push_back(Vec4f(33, 7));
    h3Edge.push_back(Vec4f(33, 60));
    h3Edge.push_back(Vec4f(33, 73));
    h3Edge.push_back(Vec4f(33, 105));
    h3Edge.push_back(Vec4f(34, 2));
    h3Edge.push_back(Vec4f(35, 67));
    h3Edge.push_back(Vec4f(36, 6));
    h3Edge.push_back(Vec4f(37, 73));
    h3Edge.push_back(Vec4f(38, 13));
    h3Edge.push_back(Vec4f(39, 12));
    h3Edge.push_back(Vec4f(40, 13));
    h3Edge.push_back(Vec4f(40, 27));
    h3Edge.push_back(Vec4f(40, 122));
    h3Edge.push_back(Vec4f(40, 174));
    h3Edge.push_back(Vec4f(41, 8));
    h3Edge.push_back(Vec4f(41, 74));
    h3Edge.push_back(Vec4f(41, 110));
    h3Edge.push_back(Vec4f(41, 123));
    h3Edge.push_back(Vec4f(42, 9));
    h3Edge.push_back(Vec4f(42, 124));
    h3Edge.push_back(Vec4f(42, 191));
    h3Edge.push_back(Vec4f(42, 207));
    h3Edge.push_back(Vec4f(43, 74));
    h3Edge.push_back(Vec4f(44, 9));
    h3Edge.push_back(Vec4f(44, 75));
    h3Edge.push_back(Vec4f(44, 180));
    h3Edge.push_back(Vec4f(44, 192));
    h3Edge.push_back(Vec4f(45, 62));
    h3Edge.push_back(Vec4f(46, 15));
    h3Edge.push_back(Vec4f(46, 63));
    h3Edge.push_back(Vec4f(46, 76));
    h3Edge.push_back(Vec4f(46, 234));
    h3Edge.push_back(Vec4f(47, 16));
    h3Edge.push_back(Vec4f(47, 28));
    h3Edge.push_back(Vec4f(47, 136));
    h3Edge.push_back(Vec4f(47, 181));
    h3Edge.push_back(Vec4f(48, 29));
    h3Edge.push_back(Vec4f(48, 175));
    h3Edge.push_back(Vec4f(48, 182));
    h3Edge.push_back(Vec4f(48, 263));
    h3Edge.push_back(Vec4f(49, 16));
    h3Edge.push_back(Vec4f(49, 77));
    h3Edge.push_back(Vec4f(49, 126));
    h3Edge.push_back(Vec4f(49, 128));
    h3Edge.push_back(Vec4f(50, 17));
    h3Edge.push_back(Vec4f(50, 78));
    h3Edge.push_back(Vec4f(50, 183));
    h3Edge.push_back(Vec4f(50, 184));
    h3Edge.push_back(Vec4f(51, 16));
    h3Edge.push_back(Vec4f(51, 30));
    h3Edge.push_back(Vec4f(51, 138));
    h3Edge.push_back(Vec4f(51, 185));
    h3Edge.push_back(Vec4f(52, 31));
    h3Edge.push_back(Vec4f(52, 176));
    h3Edge.push_back(Vec4f(52, 186));
    h3Edge.push_back(Vec4f(52, 264));
    h3Edge.push_back(Vec4f(53, 64));
    h3Edge.push_back(Vec4f(54, 19));
    h3Edge.push_back(Vec4f(54, 65));
    h3Edge.push_back(Vec4f(54, 79));
    h3Edge.push_back(Vec4f(54, 237));
    h3Edge.push_back(Vec4f(55, 80));
    h3Edge.push_back(Vec4f(56, 11));
    h3Edge.push_back(Vec4f(56, 81));
    h3Edge.push_back(Vec4f(56, 187));
    h3Edge.push_back(Vec4f(56, 195));
    h3Edge.push_back(Vec4f(57, 10));
    h3Edge.push_back(Vec4f(57, 80));
    h3Edge.push_back(Vec4f(57, 112));
    h3Edge.push_back(Vec4f(57, 131));
    h3Edge.push_back(Vec4f(58, 11));
    h3Edge.push_back(Vec4f(58, 132));
    h3Edge.push_back(Vec4f(58, 196));
    h3Edge.push_back(Vec4f(58, 208));
    h3Edge.push_back(Vec4f(59, 20));
    h3Edge.push_back(Vec4f(60, 21));
    h3Edge.push_back(Vec4f(60, 33));
    h3Edge.push_back(Vec4f(60, 133));
    h3Edge.push_back(Vec4f(60, 177));
    h3Edge.push_back(Vec4f(61, 21));
    h3Edge.push_back(Vec4f(62, 22));
    h3Edge.push_back(Vec4f(63, 46));
    h3Edge.push_back(Vec4f(64, 24));
    h3Edge.push_back(Vec4f(65, 54));
    h3Edge.push_back(Vec4f(66, 34));
    h3Edge.push_back(Vec4f(67, 27));
    h3Edge.push_back(Vec4f(67, 35));
    h3Edge.push_back(Vec4f(67, 170));
    h3Edge.push_back(Vec4f(67, 262));
    h3Edge.push_back(Vec4f(68, 110));
    h3Edge.push_back(Vec4f(69, 9));
    h3Edge.push_back(Vec4f(69, 111));
    h3Edge.push_back(Vec4f(69, 226));
    h3Edge.push_back(Vec4f(69, 270));
    h3Edge.push_back(Vec4f(70, 112));
    h3Edge.push_back(Vec4f(71, 11));
    h3Edge.push_back(Vec4f(71, 113));
    h3Edge.push_back(Vec4f(71, 228));
    h3Edge.push_back(Vec4f(71, 276));
    h3Edge.push_back(Vec4f(72, 36));
    h3Edge.push_back(Vec4f(73, 33));
    h3Edge.push_back(Vec4f(73, 37));
    h3Edge.push_back(Vec4f(73, 173));
    h3Edge.push_back(Vec4f(73, 265));
    h3Edge.push_back(Vec4f(74, 41));
    h3Edge.push_back(Vec4f(75, 44));
    h3Edge.push_back(Vec4f(76, 46));
    h3Edge.push_back(Vec4f(77, 49));
    h3Edge.push_back(Vec4f(78, 50));
    h3Edge.push_back(Vec4f(79, 54));
    h3Edge.push_back(Vec4f(80, 57));
    h3Edge.push_back(Vec4f(81, 56));
    h3Edge.push_back(Vec4f(82, 12));
    h3Edge.push_back(Vec4f(83, 13));
    h3Edge.push_back(Vec4f(84, 12));
    h3Edge.push_back(Vec4f(85, 122));
    h3Edge.push_back(Vec4f(86, 26));
    h3Edge.push_back(Vec4f(87, 27));
    h3Edge.push_back(Vec4f(87, 178));
    h3Edge.push_back(Vec4f(87, 190));
    h3Edge.push_back(Vec4f(87, 206));
    h3Edge.push_back(Vec4f(88, 14));
    h3Edge.push_back(Vec4f(89, 179));
    h3Edge.push_back(Vec4f(90, 62));
    h3Edge.push_back(Vec4f(91, 28));
    h3Edge.push_back(Vec4f(92, 193));
    h3Edge.push_back(Vec4f(93, 76));
    h3Edge.push_back(Vec4f(94, 126));
    h3Edge.push_back(Vec4f(95, 17));
    h3Edge.push_back(Vec4f(95, 127));
    h3Edge.push_back(Vec4f(95, 273));
    h3Edge.push_back(Vec4f(95, 307));
    h3Edge.push_back(Vec4f(96, 128));
    h3Edge.push_back(Vec4f(97, 17));
    h3Edge.push_back(Vec4f(97, 129));
    h3Edge.push_back(Vec4f(97, 274));
    h3Edge.push_back(Vec4f(97, 308));
    h3Edge.push_back(Vec4f(98, 64));
    h3Edge.push_back(Vec4f(99, 30));
    h3Edge.push_back(Vec4f(100, 194));
    h3Edge.push_back(Vec4f(101, 79));
    h3Edge.push_back(Vec4f(102, 18));
    h3Edge.push_back(Vec4f(103, 188));
    h3Edge.push_back(Vec4f(104, 32));
    h3Edge.push_back(Vec4f(105, 33));
    h3Edge.push_back(Vec4f(105, 189));
    h3Edge.push_back(Vec4f(105, 197));
    h3Edge.push_back(Vec4f(105, 209));
    h3Edge.push_back(Vec4f(106, 20));
    h3Edge.push_back(Vec4f(107, 133));
    h3Edge.push_back(Vec4f(108, 20));
    h3Edge.push_back(Vec4f(109, 21));
    h3Edge.push_back(Vec4f(110, 41));
    h3Edge.push_back(Vec4f(111, 69));
    h3Edge.push_back(Vec4f(112, 57));
    h3Edge.push_back(Vec4f(113, 71));
    h3Edge.push_back(Vec4f(114, 14));
    h3Edge.push_back(Vec4f(115, 15));
    h3Edge.push_back(Vec4f(115, 179));
    h3Edge.push_back(Vec4f(115, 227));
    h3Edge.push_back(Vec4f(115, 271));
    h3Edge.push_back(Vec4f(116, 136));
    h3Edge.push_back(Vec4f(117, 29));
    h3Edge.push_back(Vec4f(117, 137));
    h3Edge.push_back(Vec4f(117, 235));
    h3Edge.push_back(Vec4f(117, 386));
    h3Edge.push_back(Vec4f(118, 138));
    h3Edge.push_back(Vec4f(119, 31));
    h3Edge.push_back(Vec4f(119, 139));
    h3Edge.push_back(Vec4f(119, 236));
    h3Edge.push_back(Vec4f(119, 387));
    h3Edge.push_back(Vec4f(120, 18));
    h3Edge.push_back(Vec4f(121, 19));
    h3Edge.push_back(Vec4f(121, 188));
    h3Edge.push_back(Vec4f(121, 229));
    h3Edge.push_back(Vec4f(121, 277));
    h3Edge.push_back(Vec4f(122, 40));
    h3Edge.push_back(Vec4f(123, 41));
    h3Edge.push_back(Vec4f(124, 42));
    h3Edge.push_back(Vec4f(125, 23));
    h3Edge.push_back(Vec4f(126, 49));
    h3Edge.push_back(Vec4f(127, 95));
    h3Edge.push_back(Vec4f(128, 49));
    h3Edge.push_back(Vec4f(129, 97));
    h3Edge.push_back(Vec4f(130, 25));
    h3Edge.push_back(Vec4f(131, 57));
    h3Edge.push_back(Vec4f(132, 58));
    h3Edge.push_back(Vec4f(133, 60));
    h3Edge.push_back(Vec4f(134, 22));
    h3Edge.push_back(Vec4f(135, 23));
    h3Edge.push_back(Vec4f(136, 47));
    h3Edge.push_back(Vec4f(137, 117));
    h3Edge.push_back(Vec4f(138, 51));
    h3Edge.push_back(Vec4f(139, 119));
    h3Edge.push_back(Vec4f(140, 24));
    h3Edge.push_back(Vec4f(141, 25));
    h3Edge.push_back(Vec4f(142, 13));
    h3Edge.push_back(Vec4f(143, 26));
    h3Edge.push_back(Vec4f(144, 190));
    h3Edge.push_back(Vec4f(145, 34));
    h3Edge.push_back(Vec4f(146, 170));
    h3Edge.push_back(Vec4f(147, 35));
    h3Edge.push_back(Vec4f(148, 14));
    h3Edge.push_back(Vec4f(149, 227));
    h3Edge.push_back(Vec4f(150, 28));
    h3Edge.push_back(Vec4f(151, 29));
    h3Edge.push_back(Vec4f(151, 193));
    h3Edge.push_back(Vec4f(151, 272));
    h3Edge.push_back(Vec4f(151, 306));
    h3Edge.push_back(Vec4f(152, 76));
    h3Edge.push_back(Vec4f(153, 77));
    h3Edge.push_back(Vec4f(154, 78));
    h3Edge.push_back(Vec4f(155, 30));
    h3Edge.push_back(Vec4f(156, 31));
    h3Edge.push_back(Vec4f(156, 194));
    h3Edge.push_back(Vec4f(156, 275));
    h3Edge.push_back(Vec4f(156, 309));
    h3Edge.push_back(Vec4f(157, 79));
    h3Edge.push_back(Vec4f(158, 18));
    h3Edge.push_back(Vec4f(159, 229));
    h3Edge.push_back(Vec4f(160, 37));
    h3Edge.push_back(Vec4f(161, 36));
    h3Edge.push_back(Vec4f(162, 173));
    h3Edge.push_back(Vec4f(163, 32));
    h3Edge.push_back(Vec4f(164, 197));
    h3Edge.push_back(Vec4f(165, 21));
    h3Edge.push_back(Vec4f(166, 26));
    h3Edge.push_back(Vec4f(167, 206));
    h3Edge.push_back(Vec4f(168, 32));
    h3Edge.push_back(Vec4f(169, 209));
    h3Edge.push_back(Vec4f(170, 67));
    h3Edge.push_back(Vec4f(171, 23));
    h3Edge.push_back(Vec4f(172, 25));
    h3Edge.push_back(Vec4f(173, 73));
    h3Edge.push_back(Vec4f(174, 40));
    h3Edge.push_back(Vec4f(175, 48));
    h3Edge.push_back(Vec4f(176, 52));
    h3Edge.push_back(Vec4f(177, 60));
    h3Edge.push_back(Vec4f(178, 87));
    h3Edge.push_back(Vec4f(179, 115));
    h3Edge.push_back(Vec4f(180, 44));
    h3Edge.push_back(Vec4f(181, 47));
    h3Edge.push_back(Vec4f(182, 48));
    h3Edge.push_back(Vec4f(183, 50));
    h3Edge.push_back(Vec4f(184, 50));
    h3Edge.push_back(Vec4f(185, 51));
    h3Edge.push_back(Vec4f(186, 52));
    h3Edge.push_back(Vec4f(187, 56));
    h3Edge.push_back(Vec4f(188, 121));
    h3Edge.push_back(Vec4f(189, 105));
    h3Edge.push_back(Vec4f(190, 87));
    h3Edge.push_back(Vec4f(191, 42));
    h3Edge.push_back(Vec4f(192, 44));
    h3Edge.push_back(Vec4f(193, 151));
    h3Edge.push_back(Vec4f(194, 156));
    h3Edge.push_back(Vec4f(195, 56));
    h3Edge.push_back(Vec4f(196, 58));
    h3Edge.push_back(Vec4f(197, 105));
    h3Edge.push_back(Vec4f(198, 34));
    h3Edge.push_back(Vec4f(199, 35));
    h3Edge.push_back(Vec4f(200, 28));
    h3Edge.push_back(Vec4f(201, 272));
    h3Edge.push_back(Vec4f(202, 30));
    h3Edge.push_back(Vec4f(203, 275));
    h3Edge.push_back(Vec4f(204, 36));
    h3Edge.push_back(Vec4f(205, 37));
    h3Edge.push_back(Vec4f(206, 87));
    h3Edge.push_back(Vec4f(207, 42));
    h3Edge.push_back(Vec4f(208, 58));
    h3Edge.push_back(Vec4f(209, 105));
    h3Edge.push_back(Vec4f(210, 123));
    h3Edge.push_back(Vec4f(211, 124));
    h3Edge.push_back(Vec4f(212, 74));
    h3Edge.push_back(Vec4f(213, 180));
    h3Edge.push_back(Vec4f(214, 134));
    h3Edge.push_back(Vec4f(215, 125));
    h3Edge.push_back(Vec4f(216, 77));
    h3Edge.push_back(Vec4f(217, 183));
    h3Edge.push_back(Vec4f(218, 77));
    h3Edge.push_back(Vec4f(219, 184));
    h3Edge.push_back(Vec4f(220, 140));
    h3Edge.push_back(Vec4f(221, 130));
    h3Edge.push_back(Vec4f(222, 80));
    h3Edge.push_back(Vec4f(223, 187));
    h3Edge.push_back(Vec4f(224, 131));
    h3Edge.push_back(Vec4f(225, 132));
    h3Edge.push_back(Vec4f(226, 69));
    h3Edge.push_back(Vec4f(227, 115));
    h3Edge.push_back(Vec4f(228, 71));
    h3Edge.push_back(Vec4f(229, 121));
    h3Edge.push_back(Vec4f(230, 122));
    h3Edge.push_back(Vec4f(231, 63));
    h3Edge.push_back(Vec4f(232, 65));
    h3Edge.push_back(Vec4f(233, 133));
    h3Edge.push_back(Vec4f(234, 46));
    h3Edge.push_back(Vec4f(235, 117));
    h3Edge.push_back(Vec4f(236, 119));
    h3Edge.push_back(Vec4f(237, 54));
    h3Edge.push_back(Vec4f(238, 35));
    h3Edge.push_back(Vec4f(239, 123));
    h3Edge.push_back(Vec4f(240, 191));
    h3Edge.push_back(Vec4f(241, 110));
    h3Edge.push_back(Vec4f(242, 226));
    h3Edge.push_back(Vec4f(243, 192));
    h3Edge.push_back(Vec4f(244, 134));
    h3Edge.push_back(Vec4f(245, 171));
    h3Edge.push_back(Vec4f(246, 62));
    h3Edge.push_back(Vec4f(247, 63));
    h3Edge.push_back(Vec4f(248, 136));
    h3Edge.push_back(Vec4f(249, 235));
    h3Edge.push_back(Vec4f(250, 138));
    h3Edge.push_back(Vec4f(251, 236));
    h3Edge.push_back(Vec4f(252, 64));
    h3Edge.push_back(Vec4f(253, 65));
    h3Edge.push_back(Vec4f(254, 140));
    h3Edge.push_back(Vec4f(255, 172));
    h3Edge.push_back(Vec4f(256, 195));
    h3Edge.push_back(Vec4f(257, 112));
    h3Edge.push_back(Vec4f(258, 228));
    h3Edge.push_back(Vec4f(259, 131));
    h3Edge.push_back(Vec4f(260, 196));
    h3Edge.push_back(Vec4f(261, 37));
    h3Edge.push_back(Vec4f(262, 67));
    h3Edge.push_back(Vec4f(263, 48));
    h3Edge.push_back(Vec4f(264, 52));
    h3Edge.push_back(Vec4f(265, 73));
    h3Edge.push_back(Vec4f(266, 123));
    h3Edge.push_back(Vec4f(267, 207));
    h3Edge.push_back(Vec4f(268, 131));
    h3Edge.push_back(Vec4f(269, 208));
    h3Edge.push_back(Vec4f(270, 69));
    h3Edge.push_back(Vec4f(271, 115));
    h3Edge.push_back(Vec4f(272, 151));
    h3Edge.push_back(Vec4f(273, 95));
    h3Edge.push_back(Vec4f(274, 97));
    h3Edge.push_back(Vec4f(275, 156));
    h3Edge.push_back(Vec4f(276, 71));
    h3Edge.push_back(Vec4f(277, 121));
    h3Edge.push_back(Vec4f(278, 174));
    h3Edge.push_back(Vec4f(279, 170));
    h3Edge.push_back(Vec4f(280, 178));
    h3Edge.push_back(Vec4f(281, 75));
    h3Edge.push_back(Vec4f(282, 74));
    h3Edge.push_back(Vec4f(283, 75));
    h3Edge.push_back(Vec4f(284, 63));
    h3Edge.push_back(Vec4f(285, 181));
    h3Edge.push_back(Vec4f(286, 175));
    h3Edge.push_back(Vec4f(287, 78));
    h3Edge.push_back(Vec4f(288, 78));
    h3Edge.push_back(Vec4f(289, 185));
    h3Edge.push_back(Vec4f(290, 176));
    h3Edge.push_back(Vec4f(291, 65));
    h3Edge.push_back(Vec4f(292, 80));
    h3Edge.push_back(Vec4f(293, 81));
    h3Edge.push_back(Vec4f(294, 81));
    h3Edge.push_back(Vec4f(295, 189));
    h3Edge.push_back(Vec4f(296, 173));
    h3Edge.push_back(Vec4f(297, 177));
    h3Edge.push_back(Vec4f(298, 134));
    h3Edge.push_back(Vec4f(299, 135));
    h3Edge.push_back(Vec4f(300, 126));
    h3Edge.push_back(Vec4f(301, 273));
    h3Edge.push_back(Vec4f(302, 128));
    h3Edge.push_back(Vec4f(303, 274));
    h3Edge.push_back(Vec4f(304, 140));
    h3Edge.push_back(Vec4f(305, 141));
    h3Edge.push_back(Vec4f(306, 151));
    h3Edge.push_back(Vec4f(307, 95));
    h3Edge.push_back(Vec4f(308, 97));
    h3Edge.push_back(Vec4f(309, 156));
    h3Edge.push_back(Vec4f(310, 174));
    h3Edge.push_back(Vec4f(311, 174));
    h3Edge.push_back(Vec4f(312, 111));
    h3Edge.push_back(Vec4f(313, 127));
    h3Edge.push_back(Vec4f(314, 129));
    h3Edge.push_back(Vec4f(315, 113));
    h3Edge.push_back(Vec4f(316, 177));
    h3Edge.push_back(Vec4f(317, 177));
    h3Edge.push_back(Vec4f(318, 110));
    h3Edge.push_back(Vec4f(319, 111));
    h3Edge.push_back(Vec4f(320, 180));
    h3Edge.push_back(Vec4f(321, 234));
    h3Edge.push_back(Vec4f(322, 181));
    h3Edge.push_back(Vec4f(323, 263));
    h3Edge.push_back(Vec4f(324, 185));
    h3Edge.push_back(Vec4f(325, 264));
    h3Edge.push_back(Vec4f(326, 237));
    h3Edge.push_back(Vec4f(327, 187));
    h3Edge.push_back(Vec4f(328, 112));
    h3Edge.push_back(Vec4f(329, 113));
    h3Edge.push_back(Vec4f(330, 178));
    h3Edge.push_back(Vec4f(331, 124));
    h3Edge.push_back(Vec4f(332, 179));
    h3Edge.push_back(Vec4f(333, 270));
    h3Edge.push_back(Vec4f(334, 192));
    h3Edge.push_back(Vec4f(335, 125));
    h3Edge.push_back(Vec4f(336, 76));
    h3Edge.push_back(Vec4f(337, 181));
    h3Edge.push_back(Vec4f(338, 182));
    h3Edge.push_back(Vec4f(339, 126));
    h3Edge.push_back(Vec4f(340, 127));
    h3Edge.push_back(Vec4f(341, 235));
    h3Edge.push_back(Vec4f(342, 236));
    h3Edge.push_back(Vec4f(343, 128));
    h3Edge.push_back(Vec4f(344, 129));
    h3Edge.push_back(Vec4f(345, 185));
    h3Edge.push_back(Vec4f(346, 186));
    h3Edge.push_back(Vec4f(347, 79));
    h3Edge.push_back(Vec4f(348, 130));
    h3Edge.push_back(Vec4f(349, 195));
    h3Edge.push_back(Vec4f(350, 276));
    h3Edge.push_back(Vec4f(351, 188));
    h3Edge.push_back(Vec4f(352, 132));
    h3Edge.push_back(Vec4f(353, 189));
    h3Edge.push_back(Vec4f(354, 178));
    h3Edge.push_back(Vec4f(355, 124));
    h3Edge.push_back(Vec4f(356, 132));
    h3Edge.push_back(Vec4f(357, 189));
    h3Edge.push_back(Vec4f(358, 262));
    h3Edge.push_back(Vec4f(359, 75));
    h3Edge.push_back(Vec4f(360, 226));
    h3Edge.push_back(Vec4f(361, 125));
    h3Edge.push_back(Vec4f(362, 234));
    h3Edge.push_back(Vec4f(363, 193));
    h3Edge.push_back(Vec4f(364, 194));
    h3Edge.push_back(Vec4f(365, 237));
    h3Edge.push_back(Vec4f(366, 130));
    h3Edge.push_back(Vec4f(367, 228));
    h3Edge.push_back(Vec4f(368, 81));
    h3Edge.push_back(Vec4f(369, 265));
    h3Edge.push_back(Vec4f(370, 122));
    h3Edge.push_back(Vec4f(371, 190));
    h3Edge.push_back(Vec4f(372, 192));
    h3Edge.push_back(Vec4f(373, 227));
    h3Edge.push_back(Vec4f(374, 135));
    h3Edge.push_back(Vec4f(375, 171));
    h3Edge.push_back(Vec4f(376, 137));
    h3Edge.push_back(Vec4f(377, 307));
    h3Edge.push_back(Vec4f(378, 308));
    h3Edge.push_back(Vec4f(379, 139));
    h3Edge.push_back(Vec4f(380, 172));
    h3Edge.push_back(Vec4f(381, 141));
    h3Edge.push_back(Vec4f(382, 195));
    h3Edge.push_back(Vec4f(383, 229));
    h3Edge.push_back(Vec4f(384, 197));
    h3Edge.push_back(Vec4f(385, 133));
    h3Edge.push_back(Vec4f(386, 117));
    h3Edge.push_back(Vec4f(387, 119));
    h3Edge.push_back(Vec4f(388, 206));
    h3Edge.push_back(Vec4f(389, 135));
    h3Edge.push_back(Vec4f(390, 141));
    h3Edge.push_back(Vec4f(391, 209));
    h3Edge.push_back(Vec4f(392, 271));
    h3Edge.push_back(Vec4f(393, 136));
    h3Edge.push_back(Vec4f(394, 137));
    h3Edge.push_back(Vec4f(395, 183));
    h3Edge.push_back(Vec4f(396, 184));
    h3Edge.push_back(Vec4f(397, 138));
    h3Edge.push_back(Vec4f(398, 139));
    h3Edge.push_back(Vec4f(399, 277));
    h3Edge.push_back(Vec4f(400, 262));
    h3Edge.push_back(Vec4f(401, 111));
    h3Edge.push_back(Vec4f(402, 113));
    h3Edge.push_back(Vec4f(403, 265));
    h3Edge.push_back(Vec4f(404, 191));
    h3Edge.push_back(Vec4f(405, 179));
    h3Edge.push_back(Vec4f(406, 271));
    h3Edge.push_back(Vec4f(407, 270));
    h3Edge.push_back(Vec4f(408, 234));
    h3Edge.push_back(Vec4f(409, 193));
    h3Edge.push_back(Vec4f(410, 175));
    h3Edge.push_back(Vec4f(411, 175));
    h3Edge.push_back(Vec4f(412, 273));
    h3Edge.push_back(Vec4f(413, 274));
    h3Edge.push_back(Vec4f(414, 176));
    h3Edge.push_back(Vec4f(415, 176));
    h3Edge.push_back(Vec4f(416, 194));
    h3Edge.push_back(Vec4f(417, 237));
    h3Edge.push_back(Vec4f(418, 276));
    h3Edge.push_back(Vec4f(419, 277));
    h3Edge.push_back(Vec4f(420, 188));
    h3Edge.push_back(Vec4f(421, 196));
    h3Edge.push_back(Vec4f(422, 262));
    h3Edge.push_back(Vec4f(423, 127));
    h3Edge.push_back(Vec4f(424, 129));
    h3Edge.push_back(Vec4f(425, 265));
    h3Edge.push_back(Vec4f(426, 190));
    h3Edge.push_back(Vec4f(427, 207));
    h3Edge.push_back(Vec4f(428, 271));
    h3Edge.push_back(Vec4f(429, 171));
    h3Edge.push_back(Vec4f(430, 306));
    h3Edge.push_back(Vec4f(431, 272));
    h3Edge.push_back(Vec4f(432, 182));
    h3Edge.push_back(Vec4f(433, 183));
    h3Edge.push_back(Vec4f(434, 184));
    h3Edge.push_back(Vec4f(435, 186));
    h3Edge.push_back(Vec4f(436, 275));
    h3Edge.push_back(Vec4f(437, 309));
    h3Edge.push_back(Vec4f(438, 172));
    h3Edge.push_back(Vec4f(439, 277));
    h3Edge.push_back(Vec4f(440, 208));
    h3Edge.push_back(Vec4f(441, 197));
    h3Edge.push_back(Vec4f(442, 170));
    h3Edge.push_back(Vec4f(443, 191));
    h3Edge.push_back(Vec4f(444, 306));
    h3Edge.push_back(Vec4f(445, 137));
    h3Edge.push_back(Vec4f(446, 139));
    h3Edge.push_back(Vec4f(447, 309));
    h3Edge.push_back(Vec4f(448, 196));
    h3Edge.push_back(Vec4f(449, 173));
    h3Edge.push_back(Vec4f(450, 206));
    h3Edge.push_back(Vec4f(451, 270));
    h3Edge.push_back(Vec4f(452, 182));
    h3Edge.push_back(Vec4f(453, 307));
    h3Edge.push_back(Vec4f(454, 308));
    h3Edge.push_back(Vec4f(455, 186));
    h3Edge.push_back(Vec4f(456, 276));
    h3Edge.push_back(Vec4f(457, 209));
    h3Edge.push_back(Vec4f(458, 207));
    h3Edge.push_back(Vec4f(459, 306));
    h3Edge.push_back(Vec4f(460, 309));
    h3Edge.push_back(Vec4f(461, 208));
    h3Edge.push_back(Vec4f(462, 180));
    h3Edge.push_back(Vec4f(463, 227));
    h3Edge.push_back(Vec4f(464, 263));
    h3Edge.push_back(Vec4f(465, 386));
    h3Edge.push_back(Vec4f(466, 387));
    h3Edge.push_back(Vec4f(467, 264));
    h3Edge.push_back(Vec4f(468, 187));
    h3Edge.push_back(Vec4f(469, 229));
    h3Edge.push_back(Vec4f(470, 307));
    h3Edge.push_back(Vec4f(471, 308));
    h3Edge.push_back(Vec4f(472, 226));
    h3Edge.push_back(Vec4f(473, 386));
    h3Edge.push_back(Vec4f(474, 387));
    h3Edge.push_back(Vec4f(475, 228));
    h3Edge.push_back(Vec4f(476, 272));
    h3Edge.push_back(Vec4f(477, 263));
    h3Edge.push_back(Vec4f(478, 264));
    h3Edge.push_back(Vec4f(479, 275));
    h3Edge.push_back(Vec4f(480, 235));
    h3Edge.push_back(Vec4f(481, 236));
    h3Edge.push_back(Vec4f(482, 386));
    h3Edge.push_back(Vec4f(483, 273));
    h3Edge.push_back(Vec4f(484, 274));
    h3Edge.push_back(Vec4f(485, 387));

    leftMesh.resize(edgeNum);
    rightMesh.resize(edgeNum);

    for(int i = 0; i < edgeNum; ++i) {
      generateMesh(leftMesh[i], h3Vert[h3Edge[i][0]], h3Vert[h3Edge[i][1]], HSV((float)i / (float)edgeNum, 1.f, 1.f), false);
      generateMesh(rightMesh[i], h3Vert[h3Edge[i][0]], h3Vert[h3Edge[i][1]], HSV((float)i / (float)edgeNum, 1.f, 1.f), true);
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
        generateMesh(leftMesh[i], h3Vert[h3Edge[i][0]], h3Vert[h3Edge[i][1]], HSV((float)i / (float)edgeNum, 1.f, 1.f), false);
        generateMesh(rightMesh[i], h3Vert[h3Edge[i][0]], h3Vert[h3Edge[i][1]], HSV((float)i / (float)edgeNum, 1.f, 1.f), true);
        
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
