#ifndef INCLUDE_OBJ4D_HPP
#define INCLUDE_OBJ4D_HPP

#include <thread>
#include <atomic>

#include "allocore/graphics/al_Asset.hpp"

#include "generator.hpp"
#include "transformations.hpp"
#include "mesh4D.hpp"

std::thread polyThread;

struct Obj4D {
  std::vector<Mesh4D> meshes4D;
  
  Mat4d currentTrans;
  GroupType currentType;
  double currentScale;
  bool currentProj;

  bool busy { false };
  bool dirty { false };

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

    currentTrans.setIdentity();
  }

  void update(Mat4d trans, GroupType type, double scale=1.0, bool uhsProj=false) {
    for (unsigned i = 0; i < meshes4D.size(); ++i) {
      Mesh4D& mesh4D = meshes4D[i];

      mesh4D.update(trans, type, scale, uhsProj);
    }

    currentTrans = trans;
    currentType = type;
    currentScale = scale;
    currentProj = uhsProj;
    busy = false;
    dirty = false;
  }

  void updateT(Mat4d trans, GroupType type, double scale, bool uhsProj) {
    if(!compare(currentTrans, trans) || currentScale != scale) {
      busy = true;
      if(currentType != type || currentProj != uhsProj) dirty = true;
      polyThread = std::thread([this, trans, type, scale, uhsProj] {
        this->update(trans, type, scale, uhsProj);
      });
      polyThread.detach();
    }
  }

  void draw(Graphics& g) {
    for (unsigned i = 0; i < meshes4D.size(); ++i)
      g.draw(meshes4D[i].mesh);
  }

  unsigned int size() { return meshes4D.size(); }
};

#endif