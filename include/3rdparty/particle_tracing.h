#pragma once
#include "3rdparty/nanort.h"

#include <stdint.h>
/*
MIT licensed
Copyright (c) 2015-2018 Light Transport Entertainment, Inc.
Modified from https://github.com/lighttransport/nanort/blob/master/examples/particle_primitive/main.cc
*/

namespace particle_tracing {

typedef nanort::real3<float> float3;

// Predefined SAH predicator for sphere.
class SpherePred {
 public:
  SpherePred(const float *vertices)
      : axis_(0), pos_(0.0f), vertices_(vertices) {}

  void Set(int axis, float pos) const {
    axis_ = axis;
    pos_ = pos;
  }

  bool operator()(unsigned int i) const {
    int axis = axis_;
    float pos = pos_;

    float3 p0(&vertices_[3 * i]);

    float center = p0[axis];

    return (center < pos);
  }

 private:
  mutable int axis_;
  mutable float pos_;
  const float *vertices_;
};

// -----------------------------------------------------

class SphereGeometry {
 public:
  SphereGeometry(const float *vertices, const float radius)
      : vertices_(vertices), radius_(radius) {}

  /// Compute bounding box for `prim_index`th sphere.
  /// This function is called for each primitive in BVH build.
  void BoundingBox(float3 *bmin, float3 *bmax, unsigned int prim_index) const {
    (*bmin)[0] = vertices_[3 * prim_index + 0] - radius_;
    (*bmin)[1] = vertices_[3 * prim_index + 1] - radius_;
    (*bmin)[2] = vertices_[3 * prim_index + 2] - radius_;
    (*bmax)[0] = vertices_[3 * prim_index + 0] + radius_;
    (*bmax)[1] = vertices_[3 * prim_index + 1] + radius_;
    (*bmax)[2] = vertices_[3 * prim_index + 2] + radius_;
  }

  const float *vertices_;
  const float radius_;
  mutable float3 ray_org_;
  mutable float3 ray_dir_;
  mutable nanort::BVHTraceOptions trace_options_;
};

class SphereIntersection {
 public:
  SphereIntersection() {}

  float u;
  float v;

  // Required member variables.
  float t;
  unsigned int prim_id;
};

template <class I>
class SphereIntersector {
 public:
  SphereIntersector(const float *vertices, const float radius)
      : vertices_(vertices), radius_(radius) {}

  /// Do ray interesection stuff for `prim_index` th primitive and return hit
  /// distance `t`,
  /// varycentric coordinate `u` and `v`.
  /// Returns true if there's intersection.
  bool Intersect(float *t_inout, unsigned int prim_index) const {
    if ((prim_index < trace_options_.prim_ids_range[0]) ||
        (prim_index >= trace_options_.prim_ids_range[1])) {
      return false;
    }

    // http://wiki.cgsociety.org/index.php/Ray_Sphere_Intersection

    const float3 center(&vertices_[3 * prim_index]);
    const float radius = radius_;

    float3 oc = ray_org_ - center;

    float a = vdot(ray_dir_, ray_dir_);
    float b = 2.0 * vdot(ray_dir_, oc);
    float c = vdot(oc, oc) - radius * radius;

    float disc = b * b - 4.0 * a * c;

    float t0, t1;

    if (disc < 0.0) {  // no roots
      return false;
    } else if (disc == 0.0) {
      t0 = t1 = -0.5 * (b / a);
    } else {
      // compute q as described above
      float distSqrt = sqrt(disc);
      float q;
      if (b < 0)
        q = (-b - distSqrt) / 2.0;
      else
        q = (-b + distSqrt) / 2.0;

      // compute t0 and t1
      t0 = q / a;
      t1 = c / q;
    }

    // make sure t0 is smaller than t1
    if (t0 > t1) {
      // if t0 is bigger than t1 swap them around
      float temp = t0;
      t0 = t1;
      t1 = temp;
    }

    // if t1 is less than zero, the object is in the ray's negative direction
    // and consequently the ray misses the sphere
    if (t1 < 0) {
      return false;
    }

    float t;
    if (t0 < 0) {
      t = t1;
    } else {
      t = t0;
    }

    if (t > (*t_inout)) {
      return false;
    }

    (*t_inout) = t;

    return true;
  }

  /// Returns the nearest hit distance.
  float GetT() const { return t_; }

  /// Update is called when a nearest hit is found.
  void Update(float t, unsigned int prim_idx) const {
    t_ = t;
    prim_id_ = prim_idx;
  }

  /// Prepare BVH traversal(e.g. compute inverse ray direction)
  /// This function is called only once in BVH traversal.
  void PrepareTraversal(const nanort::Ray<float> &ray,
                        const nanort::BVHTraceOptions &trace_options) const {
    ray_org_[0] = ray.org[0];
    ray_org_[1] = ray.org[1];
    ray_org_[2] = ray.org[2];

    ray_dir_[0] = ray.dir[0];
    ray_dir_[1] = ray.dir[1];
    ray_dir_[2] = ray.dir[2];

    trace_options_ = trace_options;
  }

  /// Post BVH traversal stuff(e.g. compute intersection point information)
  /// This function is called only once in BVH traversal.
  /// `hit` = true if there is something hit.
  void PostTraversal(const nanort::Ray<float> &ray, bool hit,
                     SphereIntersection *isect) const {
    if (hit) {
      float3 hitP = ray_org_ + t_ * ray_dir_;
      float3 center = float3(&vertices_[3 * prim_id_]);
      float3 n = vnormalize(hitP - center);

      isect->t = t_;
      isect->prim_id = prim_id_;
      isect->u = (atan2(n[0], n[2]) + M_PI) * 0.5 * (1.0 / M_PI);
      isect->v = acos(n[1]) / M_PI;
    }
  }

  const float *vertices_;
  const float radius_;
  mutable float3 ray_org_;
  mutable float3 ray_dir_;
  mutable nanort::BVHTraceOptions trace_options_;

  mutable float t_;
  mutable unsigned int prim_id_;
};

}  // namespace

