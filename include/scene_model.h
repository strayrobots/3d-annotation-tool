#ifndef H_SCENE_MODEL
#define H_SCENE_MODEL
#include <memory>
#include <optional>
#include <filesystem>
#include <omp.h>
#include "camera.h"
#include "geometry/mesh.h"
#include "geometry/ray_trace_mesh.h"

class SceneModel {
private:
  std::filesystem::path datasetPath;

  std::shared_ptr<geometry::TriangleMesh> mesh;
  const geometry::RayTraceMesh rtMesh;

  // Keypoints.
  std::vector<Eigen::Vector3f> keypoints;
public:
  SceneModel(const std::string& datasetFolder);

  std::shared_ptr<geometry::TriangleMesh> getMesh() const;
  std::optional<Vector3f> traceRay(const Vector3f& origin, const Vector3f& direction);

  const std::vector<Vector3f>& getKeypoints() const { return keypoints; };
  void setKeypoints(std::vector<Vector3f>& points) { keypoints = points; };
  void popKeypoint();
  void save() const;
private:
  void initRayTracing();
};
#endif
