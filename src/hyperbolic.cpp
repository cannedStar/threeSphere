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

  float theta, epsilon, phi;
  float eyeAngle;
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
      cosh(angle), 0.f, 0.f, -sinh(angle),
      0.f, 1.f, 0.f, 0.f,
      0.f, 0.f, 1.f, 0.f,
      sinh(angle), 0.f, 0.f, cosh(angle));

    return rotate * srcMat;
  }

  Mat4f rotatePhi(Mat4f& srcMat, float& angle) {
    Mat4f rotate = Mat4f(
      cosh(angle), 0.f, -sinh(angle), 0.f,
      0.f, 1.f, 0.f, 0.f,
      sinh(angle), 0.f, cosh(angle), 0.f,
      0.f, 0.f, 0.f, 1.f);

    return rotate * srcMat;
  }

  // CONSTRUCTOR
  HyperApp() {

    nav().pos(0.0, 0.0, 0.0);
    light.pos(0, 10.0, 0);
    light.specular(Color(0.1,0.1,0.1));
    light.diffuse(Color(0.2,0.2,0.2));
    light.ambient(Color(1,1,1));
    initWindow();
    initAudio();

    lens().eyeSep(0.001); // set eyeSep to zero

    theta = 0.f;
    epsilon = 0.f;
    phi = 0.f;
    camera.setIdentity();
    // camera = rotateTheta(camera, theta);
    // camera = Mat4f(
    //   cos(theta), -sin(theta), 0.f, 0.f,
    //   sin(theta), cos(theta), 0.f, 0.f,
    //   0.f, 0.f, cos(theta), -sin(theta),
    //   0.f, 0.f, sin(theta), cos(theta));

    eyeAngle = 0.01f;
    eye.setIdentity();
    eye = rotateEpsilon(eye, eyeAngle);

    // eye = Mat4f(
    //   cos(epsilon), 0.f, 0.f, -sin(epsilon),
    //   0.f, 1.f, 0.f, 0.f,
    //   0.f, 0.f, 1.f, 0.f,
    //   sin(epsilon), 0.f, 0.f, cos(epsilon));

    h3Vert.reserve(512);
    h3Edge.reserve(700);

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

    h3Edge.push_back(Vec2i(1, 3));
    h3Edge.push_back(Vec2i(1, 4));
    h3Edge.push_back(Vec2i(1, 5));
    h3Edge.push_back(Vec2i(1, 7));
    h3Edge.push_back(Vec2i(2, 8));
    h3Edge.push_back(Vec2i(2, 12));
    h3Edge.push_back(Vec2i(2, 26));
    h3Edge.push_back(Vec2i(2, 34));
    h3Edge.push_back(Vec2i(3, 1));
    h3Edge.push_back(Vec2i(3, 9));
    h3Edge.push_back(Vec2i(3, 15));
    h3Edge.push_back(Vec2i(3, 27));
    h3Edge.push_back(Vec2i(4, 1));
    h3Edge.push_back(Vec2i(4, 8));
    h3Edge.push_back(Vec2i(4, 10));
    h3Edge.push_back(Vec2i(4, 16));
    h3Edge.push_back(Vec2i(5, 1));
    h3Edge.push_back(Vec2i(5, 17));
    h3Edge.push_back(Vec2i(5, 29));
    h3Edge.push_back(Vec2i(5, 31));
    h3Edge.push_back(Vec2i(6, 10));
    h3Edge.push_back(Vec2i(6, 20));
    h3Edge.push_back(Vec2i(6, 32));
    h3Edge.push_back(Vec2i(6, 36));
    h3Edge.push_back(Vec2i(7, 1));
    h3Edge.push_back(Vec2i(7, 11));
    h3Edge.push_back(Vec2i(7, 19));
    h3Edge.push_back(Vec2i(7, 33));
    h3Edge.push_back(Vec2i(8, 2));
    h3Edge.push_back(Vec2i(8, 4));
    h3Edge.push_back(Vec2i(8, 22));
    h3Edge.push_back(Vec2i(8, 41));
    h3Edge.push_back(Vec2i(9, 3));
    h3Edge.push_back(Vec2i(9, 42));
    h3Edge.push_back(Vec2i(9, 44));
    h3Edge.push_back(Vec2i(9, 69));
    h3Edge.push_back(Vec2i(10, 4));
    h3Edge.push_back(Vec2i(10, 6));
    h3Edge.push_back(Vec2i(10, 24));
    h3Edge.push_back(Vec2i(10, 57));
    h3Edge.push_back(Vec2i(11, 7));
    h3Edge.push_back(Vec2i(11, 56));
    h3Edge.push_back(Vec2i(11, 58));
    h3Edge.push_back(Vec2i(11, 71));
    h3Edge.push_back(Vec2i(12, 2));
    h3Edge.push_back(Vec2i(13, 40));
    h3Edge.push_back(Vec2i(14, 22));
    h3Edge.push_back(Vec2i(15, 3));
    h3Edge.push_back(Vec2i(15, 23));
    h3Edge.push_back(Vec2i(15, 46));
    h3Edge.push_back(Vec2i(15, 115));
    h3Edge.push_back(Vec2i(16, 4));
    h3Edge.push_back(Vec2i(16, 47));
    h3Edge.push_back(Vec2i(16, 49));
    h3Edge.push_back(Vec2i(16, 51));
    h3Edge.push_back(Vec2i(17, 5));
    h3Edge.push_back(Vec2i(17, 50));
    h3Edge.push_back(Vec2i(17, 95));
    h3Edge.push_back(Vec2i(17, 97));
    h3Edge.push_back(Vec2i(18, 24));
    h3Edge.push_back(Vec2i(19, 7));
    h3Edge.push_back(Vec2i(19, 25));
    h3Edge.push_back(Vec2i(19, 54));
    h3Edge.push_back(Vec2i(19, 121));
    h3Edge.push_back(Vec2i(20, 6));
    h3Edge.push_back(Vec2i(21, 60));
    h3Edge.push_back(Vec2i(22, 8));
    h3Edge.push_back(Vec2i(22, 14));
    h3Edge.push_back(Vec2i(22, 62));
    h3Edge.push_back(Vec2i(22, 134));
    h3Edge.push_back(Vec2i(23, 15));
    h3Edge.push_back(Vec2i(23, 125));
    h3Edge.push_back(Vec2i(23, 135));
    h3Edge.push_back(Vec2i(23, 171));
    h3Edge.push_back(Vec2i(24, 10));
    h3Edge.push_back(Vec2i(24, 18));
    h3Edge.push_back(Vec2i(24, 64));
    h3Edge.push_back(Vec2i(24, 140));
    h3Edge.push_back(Vec2i(25, 19));
    h3Edge.push_back(Vec2i(25, 130));
    h3Edge.push_back(Vec2i(25, 141));
    h3Edge.push_back(Vec2i(25, 172));
    h3Edge.push_back(Vec2i(26, 2));
    h3Edge.push_back(Vec2i(27, 3));
    h3Edge.push_back(Vec2i(27, 40));
    h3Edge.push_back(Vec2i(27, 67));
    h3Edge.push_back(Vec2i(27, 87));
    h3Edge.push_back(Vec2i(28, 47));
    h3Edge.push_back(Vec2i(29, 5));
    h3Edge.push_back(Vec2i(29, 48));
    h3Edge.push_back(Vec2i(29, 117));
    h3Edge.push_back(Vec2i(29, 151));
    h3Edge.push_back(Vec2i(30, 51));
    h3Edge.push_back(Vec2i(31, 5));
    h3Edge.push_back(Vec2i(31, 52));
    h3Edge.push_back(Vec2i(31, 119));
    h3Edge.push_back(Vec2i(31, 156));
    h3Edge.push_back(Vec2i(32, 6));
    h3Edge.push_back(Vec2i(33, 7));
    h3Edge.push_back(Vec2i(33, 60));
    h3Edge.push_back(Vec2i(33, 73));
    h3Edge.push_back(Vec2i(33, 105));
    h3Edge.push_back(Vec2i(34, 2));
    h3Edge.push_back(Vec2i(35, 67));
    h3Edge.push_back(Vec2i(36, 6));
    h3Edge.push_back(Vec2i(37, 73));
    h3Edge.push_back(Vec2i(38, 13));
    h3Edge.push_back(Vec2i(39, 12));
    h3Edge.push_back(Vec2i(40, 13));
    h3Edge.push_back(Vec2i(40, 27));
    h3Edge.push_back(Vec2i(40, 122));
    h3Edge.push_back(Vec2i(40, 174));
    h3Edge.push_back(Vec2i(41, 8));
    h3Edge.push_back(Vec2i(41, 74));
    h3Edge.push_back(Vec2i(41, 110));
    h3Edge.push_back(Vec2i(41, 123));
    h3Edge.push_back(Vec2i(42, 9));
    h3Edge.push_back(Vec2i(42, 124));
    h3Edge.push_back(Vec2i(42, 191));
    h3Edge.push_back(Vec2i(42, 207));
    h3Edge.push_back(Vec2i(43, 74));
    h3Edge.push_back(Vec2i(44, 9));
    h3Edge.push_back(Vec2i(44, 75));
    h3Edge.push_back(Vec2i(44, 180));
    h3Edge.push_back(Vec2i(44, 192));
    h3Edge.push_back(Vec2i(45, 62));
    h3Edge.push_back(Vec2i(46, 15));
    h3Edge.push_back(Vec2i(46, 63));
    h3Edge.push_back(Vec2i(46, 76));
    h3Edge.push_back(Vec2i(46, 234));
    h3Edge.push_back(Vec2i(47, 16));
    h3Edge.push_back(Vec2i(47, 28));
    h3Edge.push_back(Vec2i(47, 136));
    h3Edge.push_back(Vec2i(47, 181));
    h3Edge.push_back(Vec2i(48, 29));
    h3Edge.push_back(Vec2i(48, 175));
    h3Edge.push_back(Vec2i(48, 182));
    h3Edge.push_back(Vec2i(48, 263));
    h3Edge.push_back(Vec2i(49, 16));
    h3Edge.push_back(Vec2i(49, 77));
    h3Edge.push_back(Vec2i(49, 126));
    h3Edge.push_back(Vec2i(49, 128));
    h3Edge.push_back(Vec2i(50, 17));
    h3Edge.push_back(Vec2i(50, 78));
    h3Edge.push_back(Vec2i(50, 183));
    h3Edge.push_back(Vec2i(50, 184));
    h3Edge.push_back(Vec2i(51, 16));
    h3Edge.push_back(Vec2i(51, 30));
    h3Edge.push_back(Vec2i(51, 138));
    h3Edge.push_back(Vec2i(51, 185));
    h3Edge.push_back(Vec2i(52, 31));
    h3Edge.push_back(Vec2i(52, 176));
    h3Edge.push_back(Vec2i(52, 186));
    h3Edge.push_back(Vec2i(52, 264));
    h3Edge.push_back(Vec2i(53, 64));
    h3Edge.push_back(Vec2i(54, 19));
    h3Edge.push_back(Vec2i(54, 65));
    h3Edge.push_back(Vec2i(54, 79));
    h3Edge.push_back(Vec2i(54, 237));
    h3Edge.push_back(Vec2i(55, 80));
    h3Edge.push_back(Vec2i(56, 11));
    h3Edge.push_back(Vec2i(56, 81));
    h3Edge.push_back(Vec2i(56, 187));
    h3Edge.push_back(Vec2i(56, 195));
    h3Edge.push_back(Vec2i(57, 10));
    h3Edge.push_back(Vec2i(57, 80));
    h3Edge.push_back(Vec2i(57, 112));
    h3Edge.push_back(Vec2i(57, 131));
    h3Edge.push_back(Vec2i(58, 11));
    h3Edge.push_back(Vec2i(58, 132));
    h3Edge.push_back(Vec2i(58, 196));
    h3Edge.push_back(Vec2i(58, 208));
    h3Edge.push_back(Vec2i(59, 20));
    h3Edge.push_back(Vec2i(60, 21));
    h3Edge.push_back(Vec2i(60, 33));
    h3Edge.push_back(Vec2i(60, 133));
    h3Edge.push_back(Vec2i(60, 177));
    h3Edge.push_back(Vec2i(61, 21));
    h3Edge.push_back(Vec2i(62, 22));
    h3Edge.push_back(Vec2i(63, 46));
    h3Edge.push_back(Vec2i(64, 24));
    h3Edge.push_back(Vec2i(65, 54));
    h3Edge.push_back(Vec2i(66, 34));
    h3Edge.push_back(Vec2i(67, 27));
    h3Edge.push_back(Vec2i(67, 35));
    h3Edge.push_back(Vec2i(67, 170));
    h3Edge.push_back(Vec2i(67, 262));
    h3Edge.push_back(Vec2i(68, 110));
    h3Edge.push_back(Vec2i(69, 9));
    h3Edge.push_back(Vec2i(69, 111));
    h3Edge.push_back(Vec2i(69, 226));
    h3Edge.push_back(Vec2i(69, 270));
    h3Edge.push_back(Vec2i(70, 112));
    h3Edge.push_back(Vec2i(71, 11));
    h3Edge.push_back(Vec2i(71, 113));
    h3Edge.push_back(Vec2i(71, 228));
    h3Edge.push_back(Vec2i(71, 276));
    h3Edge.push_back(Vec2i(72, 36));
    h3Edge.push_back(Vec2i(73, 33));
    h3Edge.push_back(Vec2i(73, 37));
    h3Edge.push_back(Vec2i(73, 173));
    h3Edge.push_back(Vec2i(73, 265));
    h3Edge.push_back(Vec2i(74, 41));
    h3Edge.push_back(Vec2i(75, 44));
    h3Edge.push_back(Vec2i(76, 46));
    h3Edge.push_back(Vec2i(77, 49));
    h3Edge.push_back(Vec2i(78, 50));
    h3Edge.push_back(Vec2i(79, 54));
    h3Edge.push_back(Vec2i(80, 57));
    h3Edge.push_back(Vec2i(81, 56));
    h3Edge.push_back(Vec2i(82, 12));
    h3Edge.push_back(Vec2i(83, 13));
    h3Edge.push_back(Vec2i(84, 12));
    h3Edge.push_back(Vec2i(85, 122));
    h3Edge.push_back(Vec2i(86, 26));
    h3Edge.push_back(Vec2i(87, 27));
    h3Edge.push_back(Vec2i(87, 178));
    h3Edge.push_back(Vec2i(87, 190));
    h3Edge.push_back(Vec2i(87, 206));
    h3Edge.push_back(Vec2i(88, 14));
    h3Edge.push_back(Vec2i(89, 179));
    h3Edge.push_back(Vec2i(90, 62));
    h3Edge.push_back(Vec2i(91, 28));
    h3Edge.push_back(Vec2i(92, 193));
    h3Edge.push_back(Vec2i(93, 76));
    h3Edge.push_back(Vec2i(94, 126));
    h3Edge.push_back(Vec2i(95, 17));
    h3Edge.push_back(Vec2i(95, 127));
    h3Edge.push_back(Vec2i(95, 273));
    h3Edge.push_back(Vec2i(95, 307));
    h3Edge.push_back(Vec2i(96, 128));
    h3Edge.push_back(Vec2i(97, 17));
    h3Edge.push_back(Vec2i(97, 129));
    h3Edge.push_back(Vec2i(97, 274));
    h3Edge.push_back(Vec2i(97, 308));
    h3Edge.push_back(Vec2i(98, 64));
    h3Edge.push_back(Vec2i(99, 30));
    h3Edge.push_back(Vec2i(100, 194));
    h3Edge.push_back(Vec2i(101, 79));
    h3Edge.push_back(Vec2i(102, 18));
    h3Edge.push_back(Vec2i(103, 188));
    h3Edge.push_back(Vec2i(104, 32));
    h3Edge.push_back(Vec2i(105, 33));
    h3Edge.push_back(Vec2i(105, 189));
    h3Edge.push_back(Vec2i(105, 197));
    h3Edge.push_back(Vec2i(105, 209));
    h3Edge.push_back(Vec2i(106, 20));
    h3Edge.push_back(Vec2i(107, 133));
    h3Edge.push_back(Vec2i(108, 20));
    h3Edge.push_back(Vec2i(109, 21));
    h3Edge.push_back(Vec2i(110, 41));
    h3Edge.push_back(Vec2i(111, 69));
    h3Edge.push_back(Vec2i(112, 57));
    h3Edge.push_back(Vec2i(113, 71));
    h3Edge.push_back(Vec2i(114, 14));
    h3Edge.push_back(Vec2i(115, 15));
    h3Edge.push_back(Vec2i(115, 179));
    h3Edge.push_back(Vec2i(115, 227));
    h3Edge.push_back(Vec2i(115, 271));
    h3Edge.push_back(Vec2i(116, 136));
    h3Edge.push_back(Vec2i(117, 29));
    h3Edge.push_back(Vec2i(117, 137));
    h3Edge.push_back(Vec2i(117, 235));
    h3Edge.push_back(Vec2i(117, 386));
    h3Edge.push_back(Vec2i(118, 138));
    h3Edge.push_back(Vec2i(119, 31));
    h3Edge.push_back(Vec2i(119, 139));
    h3Edge.push_back(Vec2i(119, 236));
    h3Edge.push_back(Vec2i(119, 387));
    h3Edge.push_back(Vec2i(120, 18));
    h3Edge.push_back(Vec2i(121, 19));
    h3Edge.push_back(Vec2i(121, 188));
    h3Edge.push_back(Vec2i(121, 229));
    h3Edge.push_back(Vec2i(121, 277));
    h3Edge.push_back(Vec2i(122, 40));
    h3Edge.push_back(Vec2i(123, 41));
    h3Edge.push_back(Vec2i(124, 42));
    h3Edge.push_back(Vec2i(125, 23));
    h3Edge.push_back(Vec2i(126, 49));
    h3Edge.push_back(Vec2i(127, 95));
    h3Edge.push_back(Vec2i(128, 49));
    h3Edge.push_back(Vec2i(129, 97));
    h3Edge.push_back(Vec2i(130, 25));
    h3Edge.push_back(Vec2i(131, 57));
    h3Edge.push_back(Vec2i(132, 58));
    h3Edge.push_back(Vec2i(133, 60));
    h3Edge.push_back(Vec2i(134, 22));
    h3Edge.push_back(Vec2i(135, 23));
    h3Edge.push_back(Vec2i(136, 47));
    h3Edge.push_back(Vec2i(137, 117));
    h3Edge.push_back(Vec2i(138, 51));
    h3Edge.push_back(Vec2i(139, 119));
    h3Edge.push_back(Vec2i(140, 24));
    h3Edge.push_back(Vec2i(141, 25));
    h3Edge.push_back(Vec2i(142, 13));
    h3Edge.push_back(Vec2i(143, 26));
    h3Edge.push_back(Vec2i(144, 190));
    h3Edge.push_back(Vec2i(145, 34));
    h3Edge.push_back(Vec2i(146, 170));
    h3Edge.push_back(Vec2i(147, 35));
    h3Edge.push_back(Vec2i(148, 14));
    h3Edge.push_back(Vec2i(149, 227));
    h3Edge.push_back(Vec2i(150, 28));
    h3Edge.push_back(Vec2i(151, 29));
    h3Edge.push_back(Vec2i(151, 193));
    h3Edge.push_back(Vec2i(151, 272));
    h3Edge.push_back(Vec2i(151, 306));
    h3Edge.push_back(Vec2i(152, 76));
    h3Edge.push_back(Vec2i(153, 77));
    h3Edge.push_back(Vec2i(154, 78));
    h3Edge.push_back(Vec2i(155, 30));
    h3Edge.push_back(Vec2i(156, 31));
    h3Edge.push_back(Vec2i(156, 194));
    h3Edge.push_back(Vec2i(156, 275));
    h3Edge.push_back(Vec2i(156, 309));
    h3Edge.push_back(Vec2i(157, 79));
    h3Edge.push_back(Vec2i(158, 18));
    h3Edge.push_back(Vec2i(159, 229));
    h3Edge.push_back(Vec2i(160, 37));
    h3Edge.push_back(Vec2i(161, 36));
    h3Edge.push_back(Vec2i(162, 173));
    h3Edge.push_back(Vec2i(163, 32));
    h3Edge.push_back(Vec2i(164, 197));
    h3Edge.push_back(Vec2i(165, 21));
    h3Edge.push_back(Vec2i(166, 26));
    h3Edge.push_back(Vec2i(167, 206));
    h3Edge.push_back(Vec2i(168, 32));
    h3Edge.push_back(Vec2i(169, 209));
    h3Edge.push_back(Vec2i(170, 67));
    h3Edge.push_back(Vec2i(171, 23));
    h3Edge.push_back(Vec2i(172, 25));
    h3Edge.push_back(Vec2i(173, 73));
    h3Edge.push_back(Vec2i(174, 40));
    h3Edge.push_back(Vec2i(175, 48));
    h3Edge.push_back(Vec2i(176, 52));
    h3Edge.push_back(Vec2i(177, 60));
    h3Edge.push_back(Vec2i(178, 87));
    h3Edge.push_back(Vec2i(179, 115));
    h3Edge.push_back(Vec2i(180, 44));
    h3Edge.push_back(Vec2i(181, 47));
    h3Edge.push_back(Vec2i(182, 48));
    h3Edge.push_back(Vec2i(183, 50));
    h3Edge.push_back(Vec2i(184, 50));
    h3Edge.push_back(Vec2i(185, 51));
    h3Edge.push_back(Vec2i(186, 52));
    h3Edge.push_back(Vec2i(187, 56));
    h3Edge.push_back(Vec2i(188, 121));
    h3Edge.push_back(Vec2i(189, 105));
    h3Edge.push_back(Vec2i(190, 87));
    h3Edge.push_back(Vec2i(191, 42));
    h3Edge.push_back(Vec2i(192, 44));
    h3Edge.push_back(Vec2i(193, 151));
    h3Edge.push_back(Vec2i(194, 156));
    h3Edge.push_back(Vec2i(195, 56));
    h3Edge.push_back(Vec2i(196, 58));
    h3Edge.push_back(Vec2i(197, 105));
    h3Edge.push_back(Vec2i(198, 34));
    h3Edge.push_back(Vec2i(199, 35));
    h3Edge.push_back(Vec2i(200, 28));
    h3Edge.push_back(Vec2i(201, 272));
    h3Edge.push_back(Vec2i(202, 30));
    h3Edge.push_back(Vec2i(203, 275));
    h3Edge.push_back(Vec2i(204, 36));
    h3Edge.push_back(Vec2i(205, 37));
    h3Edge.push_back(Vec2i(206, 87));
    h3Edge.push_back(Vec2i(207, 42));
    h3Edge.push_back(Vec2i(208, 58));
    h3Edge.push_back(Vec2i(209, 105));
    h3Edge.push_back(Vec2i(210, 123));
    h3Edge.push_back(Vec2i(211, 124));
    h3Edge.push_back(Vec2i(212, 74));
    h3Edge.push_back(Vec2i(213, 180));
    h3Edge.push_back(Vec2i(214, 134));
    h3Edge.push_back(Vec2i(215, 125));
    h3Edge.push_back(Vec2i(216, 77));
    h3Edge.push_back(Vec2i(217, 183));
    h3Edge.push_back(Vec2i(218, 77));
    h3Edge.push_back(Vec2i(219, 184));
    h3Edge.push_back(Vec2i(220, 140));
    h3Edge.push_back(Vec2i(221, 130));
    h3Edge.push_back(Vec2i(222, 80));
    h3Edge.push_back(Vec2i(223, 187));
    h3Edge.push_back(Vec2i(224, 131));
    h3Edge.push_back(Vec2i(225, 132));
    h3Edge.push_back(Vec2i(226, 69));
    h3Edge.push_back(Vec2i(227, 115));
    h3Edge.push_back(Vec2i(228, 71));
    h3Edge.push_back(Vec2i(229, 121));
    h3Edge.push_back(Vec2i(230, 122));
    h3Edge.push_back(Vec2i(231, 63));
    h3Edge.push_back(Vec2i(232, 65));
    h3Edge.push_back(Vec2i(233, 133));
    h3Edge.push_back(Vec2i(234, 46));
    h3Edge.push_back(Vec2i(235, 117));
    h3Edge.push_back(Vec2i(236, 119));
    h3Edge.push_back(Vec2i(237, 54));
    h3Edge.push_back(Vec2i(238, 35));
    h3Edge.push_back(Vec2i(239, 123));
    h3Edge.push_back(Vec2i(240, 191));
    h3Edge.push_back(Vec2i(241, 110));
    h3Edge.push_back(Vec2i(242, 226));
    h3Edge.push_back(Vec2i(243, 192));
    h3Edge.push_back(Vec2i(244, 134));
    h3Edge.push_back(Vec2i(245, 171));
    h3Edge.push_back(Vec2i(246, 62));
    h3Edge.push_back(Vec2i(247, 63));
    h3Edge.push_back(Vec2i(248, 136));
    h3Edge.push_back(Vec2i(249, 235));
    h3Edge.push_back(Vec2i(250, 138));
    h3Edge.push_back(Vec2i(251, 236));
    h3Edge.push_back(Vec2i(252, 64));
    h3Edge.push_back(Vec2i(253, 65));
    h3Edge.push_back(Vec2i(254, 140));
    h3Edge.push_back(Vec2i(255, 172));
    h3Edge.push_back(Vec2i(256, 195));
    h3Edge.push_back(Vec2i(257, 112));
    h3Edge.push_back(Vec2i(258, 228));
    h3Edge.push_back(Vec2i(259, 131));
    h3Edge.push_back(Vec2i(260, 196));
    h3Edge.push_back(Vec2i(261, 37));
    h3Edge.push_back(Vec2i(262, 67));
    h3Edge.push_back(Vec2i(263, 48));
    h3Edge.push_back(Vec2i(264, 52));
    h3Edge.push_back(Vec2i(265, 73));
    h3Edge.push_back(Vec2i(266, 123));
    h3Edge.push_back(Vec2i(267, 207));
    h3Edge.push_back(Vec2i(268, 131));
    h3Edge.push_back(Vec2i(269, 208));
    h3Edge.push_back(Vec2i(270, 69));
    h3Edge.push_back(Vec2i(271, 115));
    h3Edge.push_back(Vec2i(272, 151));
    h3Edge.push_back(Vec2i(273, 95));
    h3Edge.push_back(Vec2i(274, 97));
    h3Edge.push_back(Vec2i(275, 156));
    h3Edge.push_back(Vec2i(276, 71));
    h3Edge.push_back(Vec2i(277, 121));
    h3Edge.push_back(Vec2i(278, 174));
    h3Edge.push_back(Vec2i(279, 170));
    h3Edge.push_back(Vec2i(280, 178));
    h3Edge.push_back(Vec2i(281, 75));
    h3Edge.push_back(Vec2i(282, 74));
    h3Edge.push_back(Vec2i(283, 75));
    h3Edge.push_back(Vec2i(284, 63));
    h3Edge.push_back(Vec2i(285, 181));
    h3Edge.push_back(Vec2i(286, 175));
    h3Edge.push_back(Vec2i(287, 78));
    h3Edge.push_back(Vec2i(288, 78));
    h3Edge.push_back(Vec2i(289, 185));
    h3Edge.push_back(Vec2i(290, 176));
    h3Edge.push_back(Vec2i(291, 65));
    h3Edge.push_back(Vec2i(292, 80));
    h3Edge.push_back(Vec2i(293, 81));
    h3Edge.push_back(Vec2i(294, 81));
    h3Edge.push_back(Vec2i(295, 189));
    h3Edge.push_back(Vec2i(296, 173));
    h3Edge.push_back(Vec2i(297, 177));
    h3Edge.push_back(Vec2i(298, 134));
    h3Edge.push_back(Vec2i(299, 135));
    h3Edge.push_back(Vec2i(300, 126));
    h3Edge.push_back(Vec2i(301, 273));
    h3Edge.push_back(Vec2i(302, 128));
    h3Edge.push_back(Vec2i(303, 274));
    h3Edge.push_back(Vec2i(304, 140));
    h3Edge.push_back(Vec2i(305, 141));
    h3Edge.push_back(Vec2i(306, 151));
    h3Edge.push_back(Vec2i(307, 95));
    h3Edge.push_back(Vec2i(308, 97));
    h3Edge.push_back(Vec2i(309, 156));
    h3Edge.push_back(Vec2i(310, 174));
    h3Edge.push_back(Vec2i(311, 174));
    h3Edge.push_back(Vec2i(312, 111));
    h3Edge.push_back(Vec2i(313, 127));
    h3Edge.push_back(Vec2i(314, 129));
    h3Edge.push_back(Vec2i(315, 113));
    h3Edge.push_back(Vec2i(316, 177));
    h3Edge.push_back(Vec2i(317, 177));
    h3Edge.push_back(Vec2i(318, 110));
    h3Edge.push_back(Vec2i(319, 111));
    h3Edge.push_back(Vec2i(320, 180));
    h3Edge.push_back(Vec2i(321, 234));
    h3Edge.push_back(Vec2i(322, 181));
    h3Edge.push_back(Vec2i(323, 263));
    h3Edge.push_back(Vec2i(324, 185));
    h3Edge.push_back(Vec2i(325, 264));
    h3Edge.push_back(Vec2i(326, 237));
    h3Edge.push_back(Vec2i(327, 187));
    h3Edge.push_back(Vec2i(328, 112));
    h3Edge.push_back(Vec2i(329, 113));
    h3Edge.push_back(Vec2i(330, 178));
    h3Edge.push_back(Vec2i(331, 124));
    h3Edge.push_back(Vec2i(332, 179));
    h3Edge.push_back(Vec2i(333, 270));
    h3Edge.push_back(Vec2i(334, 192));
    h3Edge.push_back(Vec2i(335, 125));
    h3Edge.push_back(Vec2i(336, 76));
    h3Edge.push_back(Vec2i(337, 181));
    h3Edge.push_back(Vec2i(338, 182));
    h3Edge.push_back(Vec2i(339, 126));
    h3Edge.push_back(Vec2i(340, 127));
    h3Edge.push_back(Vec2i(341, 235));
    h3Edge.push_back(Vec2i(342, 236));
    h3Edge.push_back(Vec2i(343, 128));
    h3Edge.push_back(Vec2i(344, 129));
    h3Edge.push_back(Vec2i(345, 185));
    h3Edge.push_back(Vec2i(346, 186));
    h3Edge.push_back(Vec2i(347, 79));
    h3Edge.push_back(Vec2i(348, 130));
    h3Edge.push_back(Vec2i(349, 195));
    h3Edge.push_back(Vec2i(350, 276));
    h3Edge.push_back(Vec2i(351, 188));
    h3Edge.push_back(Vec2i(352, 132));
    h3Edge.push_back(Vec2i(353, 189));
    h3Edge.push_back(Vec2i(354, 178));
    h3Edge.push_back(Vec2i(355, 124));
    h3Edge.push_back(Vec2i(356, 132));
    h3Edge.push_back(Vec2i(357, 189));
    h3Edge.push_back(Vec2i(358, 262));
    h3Edge.push_back(Vec2i(359, 75));
    h3Edge.push_back(Vec2i(360, 226));
    h3Edge.push_back(Vec2i(361, 125));
    h3Edge.push_back(Vec2i(362, 234));
    h3Edge.push_back(Vec2i(363, 193));
    h3Edge.push_back(Vec2i(364, 194));
    h3Edge.push_back(Vec2i(365, 237));
    h3Edge.push_back(Vec2i(366, 130));
    h3Edge.push_back(Vec2i(367, 228));
    h3Edge.push_back(Vec2i(368, 81));
    h3Edge.push_back(Vec2i(369, 265));
    h3Edge.push_back(Vec2i(370, 122));
    h3Edge.push_back(Vec2i(371, 190));
    h3Edge.push_back(Vec2i(372, 192));
    h3Edge.push_back(Vec2i(373, 227));
    h3Edge.push_back(Vec2i(374, 135));
    h3Edge.push_back(Vec2i(375, 171));
    h3Edge.push_back(Vec2i(376, 137));
    h3Edge.push_back(Vec2i(377, 307));
    h3Edge.push_back(Vec2i(378, 308));
    h3Edge.push_back(Vec2i(379, 139));
    h3Edge.push_back(Vec2i(380, 172));
    h3Edge.push_back(Vec2i(381, 141));
    h3Edge.push_back(Vec2i(382, 195));
    h3Edge.push_back(Vec2i(383, 229));
    h3Edge.push_back(Vec2i(384, 197));
    h3Edge.push_back(Vec2i(385, 133));
    h3Edge.push_back(Vec2i(386, 117));
    h3Edge.push_back(Vec2i(387, 119));
    h3Edge.push_back(Vec2i(388, 206));
    h3Edge.push_back(Vec2i(389, 135));
    h3Edge.push_back(Vec2i(390, 141));
    h3Edge.push_back(Vec2i(391, 209));
    h3Edge.push_back(Vec2i(392, 271));
    h3Edge.push_back(Vec2i(393, 136));
    h3Edge.push_back(Vec2i(394, 137));
    h3Edge.push_back(Vec2i(395, 183));
    h3Edge.push_back(Vec2i(396, 184));
    h3Edge.push_back(Vec2i(397, 138));
    h3Edge.push_back(Vec2i(398, 139));
    h3Edge.push_back(Vec2i(399, 277));
    h3Edge.push_back(Vec2i(400, 262));
    h3Edge.push_back(Vec2i(401, 111));
    h3Edge.push_back(Vec2i(402, 113));
    h3Edge.push_back(Vec2i(403, 265));
    h3Edge.push_back(Vec2i(404, 191));
    h3Edge.push_back(Vec2i(405, 179));
    h3Edge.push_back(Vec2i(406, 271));
    h3Edge.push_back(Vec2i(407, 270));
    h3Edge.push_back(Vec2i(408, 234));
    h3Edge.push_back(Vec2i(409, 193));
    h3Edge.push_back(Vec2i(410, 175));
    h3Edge.push_back(Vec2i(411, 175));
    h3Edge.push_back(Vec2i(412, 273));
    h3Edge.push_back(Vec2i(413, 274));
    h3Edge.push_back(Vec2i(414, 176));
    h3Edge.push_back(Vec2i(415, 176));
    h3Edge.push_back(Vec2i(416, 194));
    h3Edge.push_back(Vec2i(417, 237));
    h3Edge.push_back(Vec2i(418, 276));
    h3Edge.push_back(Vec2i(419, 277));
    h3Edge.push_back(Vec2i(420, 188));
    h3Edge.push_back(Vec2i(421, 196));
    h3Edge.push_back(Vec2i(422, 262));
    h3Edge.push_back(Vec2i(423, 127));
    h3Edge.push_back(Vec2i(424, 129));
    h3Edge.push_back(Vec2i(425, 265));
    h3Edge.push_back(Vec2i(426, 190));
    h3Edge.push_back(Vec2i(427, 207));
    h3Edge.push_back(Vec2i(428, 271));
    h3Edge.push_back(Vec2i(429, 171));
    h3Edge.push_back(Vec2i(430, 306));
    h3Edge.push_back(Vec2i(431, 272));
    h3Edge.push_back(Vec2i(432, 182));
    h3Edge.push_back(Vec2i(433, 183));
    h3Edge.push_back(Vec2i(434, 184));
    h3Edge.push_back(Vec2i(435, 186));
    h3Edge.push_back(Vec2i(436, 275));
    h3Edge.push_back(Vec2i(437, 309));
    h3Edge.push_back(Vec2i(438, 172));
    h3Edge.push_back(Vec2i(439, 277));
    h3Edge.push_back(Vec2i(440, 208));
    h3Edge.push_back(Vec2i(441, 197));
    h3Edge.push_back(Vec2i(442, 170));
    h3Edge.push_back(Vec2i(443, 191));
    h3Edge.push_back(Vec2i(444, 306));
    h3Edge.push_back(Vec2i(445, 137));
    h3Edge.push_back(Vec2i(446, 139));
    h3Edge.push_back(Vec2i(447, 309));
    h3Edge.push_back(Vec2i(448, 196));
    h3Edge.push_back(Vec2i(449, 173));
    h3Edge.push_back(Vec2i(450, 206));
    h3Edge.push_back(Vec2i(451, 270));
    h3Edge.push_back(Vec2i(452, 182));
    h3Edge.push_back(Vec2i(453, 307));
    h3Edge.push_back(Vec2i(454, 308));
    h3Edge.push_back(Vec2i(455, 186));
    h3Edge.push_back(Vec2i(456, 276));
    h3Edge.push_back(Vec2i(457, 209));
    h3Edge.push_back(Vec2i(458, 207));
    h3Edge.push_back(Vec2i(459, 306));
    h3Edge.push_back(Vec2i(460, 309));
    h3Edge.push_back(Vec2i(461, 208));
    h3Edge.push_back(Vec2i(462, 180));
    h3Edge.push_back(Vec2i(463, 227));
    h3Edge.push_back(Vec2i(464, 263));
    h3Edge.push_back(Vec2i(465, 386));
    h3Edge.push_back(Vec2i(466, 387));
    h3Edge.push_back(Vec2i(467, 264));
    h3Edge.push_back(Vec2i(468, 187));
    h3Edge.push_back(Vec2i(469, 229));
    h3Edge.push_back(Vec2i(470, 307));
    h3Edge.push_back(Vec2i(471, 308));
    h3Edge.push_back(Vec2i(472, 226));
    h3Edge.push_back(Vec2i(473, 386));
    h3Edge.push_back(Vec2i(474, 387));
    h3Edge.push_back(Vec2i(475, 228));
    h3Edge.push_back(Vec2i(476, 272));
    h3Edge.push_back(Vec2i(477, 263));
    h3Edge.push_back(Vec2i(478, 264));
    h3Edge.push_back(Vec2i(479, 275));
    h3Edge.push_back(Vec2i(480, 235));
    h3Edge.push_back(Vec2i(481, 236));
    h3Edge.push_back(Vec2i(482, 386));
    h3Edge.push_back(Vec2i(483, 273));
    h3Edge.push_back(Vec2i(484, 274));
    h3Edge.push_back(Vec2i(485, 387));

    leftMesh.resize(edgeNum);
    rightMesh.resize(edgeNum);

    for(int i = 0; i < edgeNum; ++i) {
      generateMesh(leftMesh[i], h3Vert[h3Edge[i][0]-1], h3Vert[h3Edge[i][1]-1], HSV((float)i / (float)edgeNum, 1.f, 1.f), false);
      generateMesh(rightMesh[i], h3Vert[h3Edge[i][0]-1], h3Vert[h3Edge[i][1]-1], HSV((float)i / (float)edgeNum, 1.f, 1.f), true);
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
        generateMesh(leftMesh[i], h3Vert[h3Edge[i][0]-1], h3Vert[h3Edge[i][1]-1], HSV((float)i / (float)edgeNum, 1.f, 1.f), false);
        generateMesh(rightMesh[i], h3Vert[h3Edge[i][0]-1], h3Vert[h3Edge[i][1]-1], HSV((float)i / (float)edgeNum, 1.f, 1.f), true);
        
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
