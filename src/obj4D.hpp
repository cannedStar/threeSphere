#ifndef INCLUDE_OBJ4D_HPP
#define INCLUDE_OBJ4D_HPP

#include <thread>
#include <atomic>

#include "allocore/graphics/al_Asset.hpp"

#include "generator.hpp"
#include "mesh4D.hpp"

std::thread polyThread;
atomic<bool> busy { false };

struct Obj4D {
  std::vector<Mesh4D> meshes4D;

  GroupType currentType;

  Obj4D() {}

  void load(Scene* ascene, Vec3f& scene_center, float& scene_scaleInv) {
    for(unsigned i = 0; i < ascene->meshes(); ++i) {
      Mesh mesh;
      ascene->mesh(i, mesh);

      for(unsigned j = 0; j < mesh.vertices().size(); ++j) {
        Vec3f& v = mesh.vertices()[j];

        v -= scene_center;
        v *= scene_scaleInv;
        v *= 0.5;
      }

      meshes4D.emplace_back(mesh);
    }
  }

  unsigned int size() { return meshes4D.size(); }
};

#endif