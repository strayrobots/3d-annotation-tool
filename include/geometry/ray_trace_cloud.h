#include <memory>
#include <optional>
#include <nanoflann.hpp>
#define NANORT_ENABLE_PARALLEL_BUILD 1
#include "3rdparty/nanort.h"
#include "3rdparty/particle_tracing.h"
#include "geometry/ray_trace_mesh.h"
#include "geometry/point_cloud.h"

#include <iostream>
namespace geometry {

using RowMatrixf = Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor>;

template <typename T>
struct PCAdaptor {
  const RowMatrixf& pointCloud;
  PCAdaptor(const RowMatrixf& pc) : pointCloud(pc) {}

  using coord_t = T;
  inline uint32_t kdtree_get_point_count() const { return pointCloud.rows(); };
  inline T kdtree_get_pt(const uint32_t idx, const uint32_t dim) const {
    RowVector3f vec = pointCloud.row(idx);
    if (dim == 0) return vec[0];
    else if(dim == 1) return vec[1];
    else return vec[2];
  }
  template <class BBOX>
  bool kdtree_get_bbox(BBOX& ) const { return false; }
};

using KDTree = nanoflann::KDTreeSingleIndexAdaptor<
  nanoflann::L2_Simple_Adaptor<float, PCAdaptor<float>, float>,
  PCAdaptor<float>, 3>;

class RayTraceCloud {
private:
  std::shared_ptr<PointCloud> pointCloud;
  nanort::BVHAccel<float> bvh;
  float& pointSize;
  PCAdaptor<float> adaptor;
  KDTree index;
public:
  RayTraceCloud(std::shared_ptr<geometry::PointCloud> pc, float& pointCloudPointSize);
  std::optional<Vector3f> traceRay(const Vector3f& origin, const Vector3f& direction) const;
  Intersection traceRayIntersection(const Vector3f& origin, const Vector3f& direction) const;
};
}

