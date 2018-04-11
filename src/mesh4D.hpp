#ifndef INCLUDE_MESH4D_HPP
#define INCLUDE_MESH4D_HPP

struct Mesh4D {
  std::vector<Vec4d> vertices;
  std::vector<Vec3d> normals;
  std::vector<Vec2f> texCoords;
  Mesh mesh;

  void init(Mesh& m) {
    const unsigned Nv = m.vertices().size();
    
    vertices.resize(Nv);
    normals.resize(Nv);
    texCoords.resize(Nv);

    for (unsigned i = 0; i < Nv; ++i) {
      double v1 = m.vertices()[i][0];
      double v2 = m.vertices()[i][1];
      double v3 = m.vertices()[i][2];
      double v0 = sqrt(v1*v1 + v2*v2 + v3*v3 + 1.0);
      vertices[i] = Vec4d(v0, v1, v2, v3);

      if(m.normals().size() > 0) normals[i] = m.normals()[i];
      if(m.texCoord2s().size() > 0) texCoords[i] = m.texCoord2s()[i];
    }

    // for (unsigned i = 0; i < Nv; ++i) {
    //   double v1 = m.vertices()[i][0];
    //   double v2 = m.vertices()[i][1];
    //   double v3 = m.vertices()[i][2];
    //   double v0 = sqrt(1.0 - v1*v1 - v2*v2 - v3*v3);
    //   vertices[i] = Vec4d(v0, v1, v2, v3);

    //   normals[i] = m.normals()[i];
    //   texCoords[i] = m.texCoord2s()[i];
    // }
  }

  void transform(Matrix4d& a) {
    for (unsigned i = 0; i < vertices.size(); ++i) {
      Vec4d r;
      Mat4d::multiply(r, a, vertices[i]);
      vertices[i] = r;
    }
  }
};

#endif