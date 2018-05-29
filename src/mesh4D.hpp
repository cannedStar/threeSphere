#ifndef INCLUDE_MESH4D_HPP
#define INCLUDE_MESH4D_HPP

#include "generator.hpp"

struct Mesh4D {
  std::vector<Vec4d> vertices;
  std::vector<Vec3d> normals;
  Mesh mesh;
  unsigned vNum, nNum;
  GroupType currentType;
  double currentScale;

  Mesh4D(Mesh& m) {
    mesh.reset();
    mesh = m;

    vNum = m.vertices().size();
    nNum = m.normals().size();
    
    vertices.resize(vNum);
    normals.resize(nNum);

    for (unsigned i = 0; i < vNum; ++i) {
      double v1 = m.vertices()[i][0];
      double v2 = m.vertices()[i][1];
      double v3 = m.vertices()[i][2];
      double v0 = sqrt(v1*v1 + v2*v2 + v3*v3 + 1.0);
      vertices[i] = Vec4d(v0, v1, v2, v3);

      // if(nNum > 0) normals[i] = m.normals()[i];
    }

    currentScale = 1.0;
  }

  void update(Mat4d mat, GroupType type, double scale=1.0, bool uhsProj=false) {
    setType(type, scale);

    Vec4d newVert;

    for (unsigned i = 0; i < vNum; ++i) {
      Mat4d::multiply(newVert, mat, vertices[i]);
      switch(type) {
        case GroupType::HYPERBOLIC: if(uhsProj) mesh.vertices()[i] = uhs(klein(newVert));
                         else mesh.vertices()[i] = klein(newVert); break;
        case GroupType::SPHERICAL: mesh.vertices()[i] = s3(newVert); break;
        case GroupType::EUCLEADIAN: mesh.vertices()[i] = eucl(newVert); break;
      }
    }

    // busy = false;
  }

  // void updateT(Mat4d mat, GroupType type, bool uhsProj) {
  //   busy = true;
  //   polyThread = std::thread([this, mat, type, uhsProj] {
  //     this->update(mat, type, uhsProj);
  //   });
  //   polyThread.detach();
  // }

  void setType(GroupType type, double scale) {
    if(currentType != type || currentScale != scale) {
      for (unsigned i = 0; i < vNum; ++i) {
        Vec4d& v = vertices[i];
        v *= scale / currentScale;
        switch(type) {
          case GroupType::SPHERICAL: v[0] = sqrt(1.0 - v[1]*v[1] - v[2]*v[2] - v[3]*v[3]); break;
          case GroupType::HYPERBOLIC: v[0] = sqrt(v[1]*v[1] + v[2]*v[2] + v[3]*v[3] + 1.0); break;
          case GroupType::EUCLEADIAN: v[0] = 1.0; break;
        }
      }
      currentType = type;
      currentScale = scale;
    }
  }
};

#endif