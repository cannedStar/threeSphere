#ifndef INCLUDE_HOPF_HPP
#define INCLUDE_HOPF_HPP

#include "allocore/al_Allocore.hpp"

#include <thread>
#include <atomic>

using namespace al;
using namespace std;

struct Hopf {
  int maxEdge = 1600;
  
  int edgeRes;

  std::vector<Vec4f> edges;
  std::vector<Mesh> edgeMesh[2];

  atomic<bool> dirty { false };
  atomic <bool> busy { false };

  std::thread hopfThread;

  void init(int er = 512) {
    edgeRes = er;
    dirty = false;
    busy = false;

    edges.reserve(maxEdge * edgeRes);
    edgeMesh[0].reserve(maxEdge);
    edgeMesh[1].reserve(maxEdge);
  }

  void clear() {
    edges.clear();
    edgeMesh[0].clear();
    edgeMesh[1].clear();
  }

  void generateEdge(const float& r, const float& t, const float a, const float b) {
    for (int i = 0; i < edgeRes; ++i) {
      float alpha = 2.f * M_PI * (float)i / ((float)edgeRes - 1.f);

      Vec4f newPoint = Vec4f(
        r / sqrt(r*r + 1.f) * cos(t + a * alpha),
        r / sqrt(r*r + 1.f) * sin(t + a * alpha),
        1.f / sqrt(r*r + 1.f) * cos(b * alpha),
        1.f / sqrt(r*r + 1.f) * sin(b * alpha)
        );

      // projection onto S3
      edges.push_back(newPoint.normalized());
    }
  }

  void setHopf(int xVert, int yVert, const float a, const float b) {
    clear();

    for (int i = 0; i < xVert; ++i) {
      for (int j = i + 1; j < yVert; ++j) {
        float r = powf((float)i / 5.f, 2);
        float t = 2.f * M_PI * (float)j / (float)(yVert - 1);
        generateEdge(r, t, a, b);
      }
    }
    
    cout << "Hopf: (" << xVert << ", " << yVert << ") vertices -> " << edges.size() / edgeRes << " edges" << endl;
  }

  void generateMesh(const Mat4f& camera, const Mat4f& eye) {
    // vertMesh[0].clear();
    // vertMesh[1].clear();
    if (edgeMesh[0].size() != edges.size() / edgeRes) {
      edgeMesh[0].resize(edges.size() / edgeRes);
      edgeMesh[1].resize(edges.size() / edgeRes);
    }

    for (int i = 0; i < edgeMesh[0].size(); ++i) {
      HSV meshColor;
      
      meshColor = HSV((float) i / (float)edgeMesh[0].size(), 1.f, 1.f);
      
      edgeMesh[0][i].reset();
      edgeMesh[0][i].primitive(Graphics::LINE_STRIP);
      edgeMesh[1][i].reset();
      edgeMesh[1][i].primitive(Graphics::LINE_STRIP);

      for (int j = 0; j < edgeRes; ++j) {
        Vec4f left4D, right4D;
        Mat4f::multiply(left4D, camera, edges[edgeRes * i + j]);
        Mat4f::multiply(right4D, eye, left4D);

        // projection onto R3
        Vec3f left3D = Vec3f(
          left4D[1] / (1.f - left4D[0]),
          left4D[2] / (1.f - left4D[0]),
          left4D[3] / (1.f - left4D[0])
          );
        Vec3f right3D = Vec3f(
          right4D[1] / (1.f - right4D[0]),
          right4D[2] / (1.f - right4D[0]),
          right4D[3] / (1.f - right4D[0])
          );

        edgeMesh[0][i].vertex(left3D);
        edgeMesh[0][i].color(meshColor);
        edgeMesh[1][i].vertex(right3D);
        edgeMesh[1][i].color(meshColor);
      }

      // edgeMesh[0][i].generateNormals();
      // edgeMesh[1][i].generateNormals();
    }

    dirty = false;
    busy = false;
  }

  void generateMeshT(const Mat4f& camera, const Mat4f& eye) {
    busy = true;
    hopfThread = std::thread([this, &camera, &eye]{
      this->generateMesh(camera, eye);
    });
    hopfThread.detach();
  }

  void draw(Graphics& g, int eye) {
    for (int i = 0; i < edgeMesh[eye].size(); ++i) {
      g.draw(edgeMesh[eye][i]);
    }
  }

};

#endif