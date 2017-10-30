#ifndef INCLUDE_POLY_HPP
#define INCLUDE_POLY_HPP

#include "allocore/al_Allocore.hpp"

using namespace al;
using namespace std;

struct Poly {
  int maxVert = 800;
  int maxEdge = 1600;
  
  int edgeRes;

  std::vector<Vec4f> verts;
  std::vector<Vec4f> edges;
  std::vector<Mesh> vertMesh[2];
  std::vector<Mesh> edgeMesh[2];

  void init(int er = 128) {
    edgeRes = er;

    verts.reserve(maxVert);
    edges.reserve(maxEdge * edgeRes);
    vertMesh[0].reserve(maxVert);
    vertMesh[1].reserve(maxVert);
    edgeMesh[0].reserve(maxEdge);
    edgeMesh[1].reserve(maxEdge);
  }

  void clear() {
    verts.clear();
    edges.clear();
    vertMesh[0].clear();
    vertMesh[1].clear();
    edgeMesh[0].clear();
    edgeMesh[1].clear();
  }

  void addPermutation(Vec4f seed, bool even) {
    std::vector<Vec4f> temp;

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

            temp.push_back(Vec4f(s0, s1, s2, s3));
            temp.push_back(Vec4f(s0, s3, s1, s2));
            temp.push_back(Vec4f(s0, s2, s3, s1));
            temp.push_back(Vec4f(s1, s0, s3, s2));
            temp.push_back(Vec4f(s1, s3, s2, s0));
            temp.push_back(Vec4f(s1, s2, s0, s3));
            temp.push_back(Vec4f(s2, s0, s1, s3));
            temp.push_back(Vec4f(s2, s3, s0, s1));
            temp.push_back(Vec4f(s2, s1, s3, s0));
            temp.push_back(Vec4f(s3, s0, s2, s1));
            temp.push_back(Vec4f(s3, s2, s1, s0));
            temp.push_back(Vec4f(s3, s1, s0, s2));

            if (!even) {
              temp.push_back(Vec4f(s0, s1, s3, s2));
              temp.push_back(Vec4f(s0, s3, s2, s1));
              temp.push_back(Vec4f(s0, s2, s1, s3));
              temp.push_back(Vec4f(s1, s0, s2, s3));
              temp.push_back(Vec4f(s1, s3, s0, s2));
              temp.push_back(Vec4f(s1, s2, s3, s0));
              temp.push_back(Vec4f(s2, s0, s3, s1));
              temp.push_back(Vec4f(s2, s3, s1, s0));
              temp.push_back(Vec4f(s2, s1, s0, s3));
              temp.push_back(Vec4f(s3, s0, s1, s2));
              temp.push_back(Vec4f(s3, s2, s0, s1));
              temp.push_back(Vec4f(s3, s1, s2, s0));
            }
          }
        }
      }
    }

    for (auto it = temp.begin(); it != temp.end(); ++it) {
      for (auto it2 = std::next(it); it2 != temp.end(); ) {
        if (*it2 == *it) {
          it2 = temp.erase(it2);
        }
        else ++it2;
      }
    }

    for (auto &i : temp) {
      verts.push_back(i);
    }
  }

  void generateEdge(const Vec4f& srcVt1, const Vec4f& srcVt2) {
    for (int i = 0; i < edgeRes; ++i) {
      float t = (float)i / ((float)edgeRes - 1.f);
      Vec4f newPoint = srcVt1 + t * (srcVt2 - srcVt1);

      // projection onto S3
      edges.push_back(newPoint.normalized());
    }
  }

  void generateMesh(const Mat4f& camera, const Mat4f& eye, int showDual = 0) {
    // vertMesh[0].clear();
    // vertMesh[1].clear();
    if (edgeMesh[0].size() != edges.size() / edgeRes) {
      edgeMesh[0].resize(edges.size() / edgeRes);
      edgeMesh[1].resize(edges.size() / edgeRes);
    }

    for (int i = 0; i < edgeMesh[0].size(); ++i) {
      HSV meshColor;
      
      if (showDual == 1)
        meshColor = HSV(0.f, 1.f, 1.f);
      else if (showDual == 2)
        meshColor = HSV(0.5f, 1.f, 1.f);
      else
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
  }

  void setHypercube() {
    clear();

    addPermutation(Vec4f(1, 1, 1, 1), false);

    for (int i = 0; i < verts.size(); ++i) {
      for (int j = i + 1; j < verts.size(); ++j) {
        Vec4f dist = (verts[i] - verts[j]);
        if (dist.mag() < 2.1f) {
          generateEdge(verts[i], verts[j]);
        }
      }
    }
    
    cout << "Hypercube: " << verts.size() << " vertices -> " << edges.size() / edgeRes << " edges" << endl;
  }

  void set16() {
    clear();

    addPermutation(Vec4f(1, 0, 0, 0), false);

    for (int i = 0; i < verts.size(); ++i) {
      for (int j = i + 1; j < verts.size(); ++j) {
        Vec4f dist = (verts[i] - verts[j]);
        if (dist.mag() < 1.5f) {
          generateEdge(verts[i], verts[j]);
        }
      }
    }
    
    cout << "16-cell: " << verts.size() << " vertices -> " << edges.size() / edgeRes << " edges" << endl;
  }

  void set24() {
    clear();

    addPermutation(Vec4f(1, 1, 1, 1), false);
    addPermutation(Vec4f(2, 0, 0, 0), false);

    for (int i = 0; i < verts.size(); ++i) {
      for (int j = i + 1; j < verts.size(); ++j) {
        Vec4f dist = (verts[i] - verts[j]);
        if (dist.mag() < 2.1f) {
          generateEdge(verts[i], verts[j]);
        }
      }
    }

    cout << "24-cell: " << verts.size() << " vertices -> " << edges.size() / edgeRes << " edges" << endl;
  }

  void set120() {
    clear();

    float gold = (1.f + sqrt(5)) / 2.f;

    addPermutation(Vec4f(0, 0, 2, 2), false);
    addPermutation(Vec4f(1, 1, 1, sqrt(5)), false);
    addPermutation(Vec4f(pow(gold, -2), gold, gold, gold), false);
    addPermutation(Vec4f(pow(gold, -1), pow(gold, -1), pow(gold, -1), pow(gold, 2)), false);
    addPermutation(Vec4f(0, pow(gold, -2), 1, pow(gold, 2)), true);
    addPermutation(Vec4f(0, pow(gold, -1), gold, sqrt(5)), true);
    addPermutation(Vec4f(pow(gold, -1), 1, gold, 2), true);

    for (int i = 0; i < verts.size(); ++i) {
      for (int j = i + 1; j < verts.size(); ++j) {
        Vec4f dist = (verts[i] - verts[j]);
        if (dist.mag() < 0.8f) {
          generateEdge(verts[i], verts[j]);
        }
      }
    }
    
    cout << "120-cell: " << verts.size() << " vertices -> " << edges.size() / edgeRes << " edges" << endl;
  }

  void set600() {
    clear();

    float gold = (1.f + sqrt(5)) / 2.f;

    addPermutation(Vec4f(0.5, 0.5, 0.5, 0.5), false);
    addPermutation(Vec4f(0, 0, 0, 1), false);
    addPermutation(Vec4f(0.5 * gold, 0.5, 0.5 * pow(gold, -1), 0), true);

    for (int i = 0; i < verts.size(); ++i) {
      for (int j = i + 1; j < verts.size(); ++j) {
        Vec4f dist = (verts[i] - verts[j]);
        if (dist.mag() < 0.8f) {
          generateEdge(verts[i], verts[j]);
        }
      }
    }
    
    cout << "600-cell: " << verts.size() << " vertices -> " << edges.size() / edgeRes << " edges" << endl;
  }

  void draw(Graphics& g, int eye) {
    for (int i = 0; i < edgeMesh[eye].size(); ++i) {
      g.draw(edgeMesh[eye][i]);
    }
  }

};

#endif