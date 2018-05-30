#ifndef INCLUDE_GRAPH4D_HPP
#define INCLUDE_GRAPH4D_HPP

#include "generator.hpp"
#include "mesh4D.hpp"
#include "transformations.hpp"

struct Graph4D {
  std::vector<Mesh> meshes;

  GroupType currentType;

  Graph4D() {}

  void update(Mat4d& camera, Generator& gen, int depth=0, int edgeRes=10, bool uhsProj=false) {
    meshes.clear();

    for(unsigned i = 0; i < gen.size(); ++i) {
      if (gen.getDepth(i) > depth) break;

      Vec4d left = camera * gen.getOld(i) * Vec4d(1, 0, 0, 0);
      Vec4d right = camera * gen.get(i) * Vec4d(1, 0, 0, 0);

      Mesh mesh;
      mesh.primitive(Graphics::LINE_STRIP);

      for (unsigned j = 0; j < edgeRes; ++j) {
        double t = (double)j / ((double)edgeRes - 1.0);
        Vec4d newPt = left + t * (right - left);
        Vec3d newProj;
        switch(gen.type) {
          case GroupType::HYPERBOLIC: if(uhsProj) newProj = uhs(klein(newPt));
                           else newProj = klein(newPt); break;
          case GroupType::SPHERICAL: newProj = s3(newPt); break;
          case GroupType::EUCLEADIAN: newProj = eucl(newPt); break;
        }

        mesh.vertex(newProj);
        mesh.color(1, 1, 1, 1);
      }
      
      meshes.push_back(mesh);
    }
  }

  unsigned int size() { return meshes.size(); }
};

#endif