#ifndef H_SCENE_MODEL
#define H_SCENE_MODEL
#include <memory>
#include <optional>
#include <filesystem>
#include <omp.h>
#include "3rdparty/json.hpp"
#define NANORT_ENABLE_PARALLEL_BUILD 1
#include "3rdparty/nanort.h"
#include "camera.h"
#include "geometry/mesh.h"

const int Width = 800;
const int Height = 600;

class SceneModel {
private:
  std::filesystem::path datasetPath;
  // Geometry.
  std::shared_ptr<geometry::TriangleMesh> mesh;
  std::unique_ptr<nanort::TriangleMesh<float>> nanoMesh;
  std::unique_ptr<nanort::TriangleSAHPred<float>> triangle_pred;
  nanort::BVHAccel<float> bvh;

  std::shared_ptr<Camera> camera;

  // Keypoints.
  std::vector<Eigen::Vector3f> keypoints;
  std::vector<std::shared_ptr<geometry::TriangleMesh>> objects;
public:
  SceneModel(const std::string& datasetFolder);

  std::shared_ptr<geometry::TriangleMesh> getMesh() const;
  std::shared_ptr<Camera> getCamera() const;
  std::optional<Vector3f> traceRay(double x, double y);

  const std::vector<Vector3f>& getKeypoints() const { return keypoints; };
  void setKeypoints(std::vector<Vector3f>& points) { keypoints = points; };
  void popKeypoint();
  void rotateCamera(const Quaternionf& rotation);
  void zoomCamera(float diff);
  void save() const;
private:
  void initRayTracing();
};
#endif
