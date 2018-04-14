#ifndef INCLUDE_GENERATOR_HPP
#define INCLUDE_GENERATOR_HPP

#include <iostream>

#include "allocore/al_Allocore.hpp"
#include "transformations.hpp"

using namespace al;
using namespace std;

enum GroupType {
  EUCLEADIAN = 0,
  SPHERICAL,
  HYPERBOLIC
};

struct Transform {
  Mat4d mat;
  int depth;
};

struct Generator {
  std::vector<Mat4d> gen;
  std::vector<Transform> transforms;
  GroupType type;
  int maxDepth;

  Generator(Mat4d& a, int depth = 4, GroupType t=GroupType::EUCLEADIAN) {
    type = t;
    maxDepth = depth;

    Mat4d a_inv = a;
    invert(a_inv);

    gen.resize(2);
    gen[0] = a;
    gen[1] = a_inv;

    genTransforms(depth);
  }

  Generator(Mat4d& a, Mat4d& b, int depth = 4, GroupType t=GroupType::EUCLEADIAN) {
    type = t;
    maxDepth = depth;

    Mat4d a_inv = a;
    Mat4d b_inv = b;
    invert(a_inv);
    invert(b_inv);

    gen.resize(4);
    gen[0] = a;
    gen[1] = b;
    gen[2] = a_inv;
    gen[3] = b_inv;

    genTransforms(depth);
  }

  Generator(Mat4d& a, Mat4d& b, Mat4d& c, int depth = 4, GroupType t=GroupType::EUCLEADIAN) {
    type = t;
    maxDepth = depth;

    Mat4d a_inv = a;
    Mat4d b_inv = b;
    Mat4d c_inv = c;
    invert(a_inv);
    invert(b_inv);
    invert(c_inv);

    gen.resize(6);
    gen[0] = a;
    gen[1] = b;
    gen[2] = c;
    gen[3] = a_inv;
    gen[4] = b_inv;
    gen[5] = c_inv;

    genTransforms(depth);
  }

  void genTransforms(int& depth) {
    Transform newTrans;
    newTrans.mat = Mat4d::identity();
    newTrans.depth = 0;
    transforms.push_back(newTrans);

    int idx = 0;
    for (int d = 0; d < depth; ++d) {
      int oldIdx = idx;
      idx = transforms.size();

      for (int i = oldIdx; i < idx; ++i) {
        Transform& old = transforms[i];
        for (int j = 0; j < gen.size(); ++j) {
          newTrans.mat = gen[j] * old.mat;
          newTrans.depth = old.depth + 1;

          bool unique = true;

          for (int k = 0; k < transforms.size(); ++k) {
            if (compare(newTrans.mat, transforms[k].mat)) {
              unique = false;
              break;
            }
          }
          if (unique) transforms.push_back(newTrans);
        }
      }
    }
    cout << transforms.size() << endl;
  }

  unsigned size() { return transforms.size(); }

  Mat4d& get(int n) { return transforms[n].mat; }

  int getDepth(int n) { return transforms[n].depth; }
};

struct Group {
  std::vector<Generator> generators;

  void init() {
    Mat4d a, b, c;

    // Figure 8 knot complement
    a = Mat4d(
      1.5, 1.0, 0.0, -0.5,
      1.0, 1.0, 0.0, -1.0,
      0.0, 0.0, 1.0, 0.0,
      0.5, 1.0, 0.0, 0.5);
    b = Mat4d(
      1.5, 0.5, -sqrt(3)/2.0, 0.5,
      0.5, 1.0, 0.0, 0.5,
      -sqrt(3)/2.0, 0.0, 1.0, -sqrt(3)/2.0,
      -0.5, -0.5, sqrt(3)/2.0, 0.5);

    generators.emplace_back(a, b, 8, GroupType::HYPERBOLIC);


    // Binary Tetrahedral
    a = 0.5 * Mat4d(
      1, -1, -1, -1,
      1, 1, -1, 1,
      1, 1, 1, -1,
      1, -1, 1, 1);
    b = 0.5 * Mat4d(
      1, -1, -1, 1,
      1, 1, 1, 1,
      1, -1, 1, -1,
      -1, -1, 1, 1);

    generators.emplace_back(a, b, 4, GroupType::SPHERICAL);

    // // 3-Torus
    // a = Mat4d(
    //   1, 0, 0, 1,
    //   0, 1, 0, 0,
    //   0, 0, 1, 0,
    //   0, 0, 0, 1);
    // b = Mat4d(
    //   )

    // 3-Torus;
    a = Mat4d(
      1, 0, 0, 0,
      1, 1, 0, 0,
      0, 0, 1, 0,
      0, 0, 0, 1);
    // b = Mat4d(
    //   1, 0, 0, 0,
    //   0, 1, 0, 1,
    //   0, 0, 1, 0,
    //   0, 0, 0, 1);
    // c = Mat4d(
    //   1, 0, 0, 0,
    //   0, 1, 0, 0,
    //   0, 0, 1, 1,
    //   0, 0, 0, 1);

    generators.emplace_back(a, 4, GroupType::EUCLEADIAN);
    // generators.emplace_back(a, b, c, 4, GroupType::EUCLEADIAN);
  }
};

#endif