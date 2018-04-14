#ifndef INCLUDE_MESH4D_HPP
#define INCLUDE_MESH4D_HPP

#include "generator.hpp"

struct Mesh4D {
  std::vector<Vec4d> vertices;
  std::vector<Vec3d> normals;
  Mesh mesh;
  unsigned vNum, nNum;
  GroupType currentType;

  void init(Mesh& m) {
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
  }

  void update(Mat4d mat, GroupType type, int proj) {
    setType(type);

    Vec4d newVert;

    for (unsigned i = 0; i < vNum; ++i) {
      Mat4d::multiply(newVert, mat, vertices[i]);
      switch(proj) {
        case 0: mesh.vertices()[i] = klein(newVert); break;
        case 1: mesh.vertices()[i] = uhs(klein(newVert)); break;
        case 2: mesh.vertices()[i] = s3(newVert); break;
        case 3: mesh.vertices()[i] = eucl(newVert); break;
      }
    }
  }

  

  void setType(GroupType type) {
    if(currentType != type) {
      for (unsigned i = 0; i < vNum; ++i) {
        Vec4d& v = vertices[i];
        switch(type) {
          case GroupType::SPHERICAL: v[0] = sqrt(1.0 - v[1]*v[1] - v[2]*v[2] - v[3]*v[3]); break;
          case GroupType::HYPERBOLIC: v[0] = sqrt(v[1]*v[1] + v[2]*v[2] + v[3]*v[3] + 1.0); break;
          case GroupType::EUCLEADIAN: v[0] = 1.0; break;
        }
      }
      currentType = type;
    }
  }
};

#endif