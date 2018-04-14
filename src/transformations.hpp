#ifndef INCLUDE_POLY_HPP
#define INCLUDE_POLY_HPP

#include <iostream>

#include "allocore/al_Allocore.hpp"

using namespace al;
using namespace std;

Vec3d eucl(const Vec4d& src) {
  return Vec3d(src[1], src[2], src[3]);
}

Vec3d s3(const Vec4d& src) {
  return Vec3d(src[1] / (1.f - src[0]),
               src[2] / (1.f - src[0]),
               src[3] / (1.f - src[0]));
}

Vec3d klein(const Vec4d& src) {
  return Vec3d(
    src[1] / src[0],
    src[2] / src[0],
    src[3] / src[0]
    );
}

Vec3d uhs(const Vec3d& src) {
  // return src;
  double a = 2.0 / (1.0 - 2.0*src[2] + src[0]*src[0] + src[1]*src[1] + src[2]*src[2]);
  return Vec3d(
    a * src[0],
    a * src[1],
    a * (1.0 - src[2]) - 1.0
    );
}

Mat4d rotate3s(Mat4d& srcMat, double& theta, double& phi) {
  Mat4d rotate = Mat4d(cos(theta), 0, 0, -sin(theta),
                     0, cos(theta+phi), -sin(theta+phi), 0,
                     0, sin(theta+phi), cos(theta+phi), 0,
                     sin(theta), 0, 0, cos(theta));
  return rotate * srcMat;
}

Mat4d rotateTheta(Mat4d& srcMat, double& angle) {
  Mat4d rotate = Mat4d(
    cosh(angle), sinh(angle), 0.0, 0.0,
    sinh(angle), cosh(angle), 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    0.0, 0.0, 0.0, 1.0);

  return rotate * srcMat;
}

Mat4d rotatePhi(Mat4d& srcMat, double& angle) {
  Mat4d rotate = Mat4d(
    cosh(angle), 0.0, sinh(angle), 0.0,
    0.0, 1.0, 0.0, 0.0,
    sinh(angle), 0.0, cosh(angle), 0.0,
    0.0, 0.0, 0.0, 1.0);

  return rotate * srcMat;
}

Mat4d rotateEpsilon(Mat4d& srcMat, double& angle) {
  Mat4d rotate = Mat4d(
    cosh(angle), 0.0, 0.0, sinh(angle),
    0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    sinh(angle), 0.0, 0.0, cosh(angle));

  return rotate * srcMat;
}

Mat4d para(Mat4d& srcMat, double& p, double& q) {
  Mat4d par = Mat4d(
    0.5*(2.0 + p*p + q*q), p, -q, -0.5*(p*p + q*q),
    p, 1.0, 0.0, -p,
    -q, 0.0, 1.0, q,
    0.5*(p*p + q*q), p, -q, 0.5*(2.0 - p*p - q*q));

  return par * srcMat;
}

Vec4d loxodromic(const Vec4d& src, const double& s, const double& t, const double& x) {
  Mat4d s_mat(
    cosh(2.0*t), 0, 0, sinh(2.0*t),
    0, cos(2.0*s), sin(2.0*s), 0,
    0, -sin(2.0*s), cos(2.0*s), 0,
    sinh(2.0*t), 0, 0, cosh(2.0*t)
    );

  Mat4d x_mat(
    (1.0+pow(x,2)+pow(x,4))/(2.0*x*x), pow(x,-1), 0.0, (-1.0+pow(x,2)+pow(x,4))/(2.0*x*x),
    x, 1.0, 0.0, x,
    0.0, 0.0, 1.0, 0.0,
    -(1.0+pow(x,2)-pow(x,4))/(2.0*x*x), -pow(x,-1), 0.0, (1.0-pow(x,2)+pow(x,4))/(2.0*x*x)
    );

  Mat4d x_inv(x_mat);
  invert(x_inv);

  Mat4d result;

  Mat4d::multiply(result, s_mat, x_inv);
  Mat4d::multiply(result, x_mat, result);

  return result*src;
}

bool compare(Mat4d& a, Mat4d& b) {
  for(int i = 0; i < a.size(); ++i) {
    if (abs(a[i] - b[i]) > 1e-7) return false;
  }

  return true;
}

#endif