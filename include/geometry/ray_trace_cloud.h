#include <memory>
#include <optional>
#define NANORT_ENABLE_PARALLEL_BUILD 1
#include "3rdparty/nanort.h"
#include "3rdparty/particle_tracing.h"
#include "geometry/ray_trace_mesh.h"
#include "geometry/point_cloud.h"

namespace geometry {
class RayTraceCloud {
private:
  std::shared_ptr<PointCloud> pointCloud;
  nanort::BVHAccel<float> bvh;
  float& pointSize;
public:
  RayTraceCloud(std::shared_ptr<geometry::PointCloud> pc, float& pointCloudPointSize);
  std::optional<Vector3f> traceRay(const Vector3f& origin, const Vector3f& direction) const;
  Intersection traceRayIntersection(const Vector3f& origin, const Vector3f& direction) const;
};
}

