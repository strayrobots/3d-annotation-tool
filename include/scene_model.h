#ifndef H_SCENE_MODEL
#define H_SCENE_MODEL
#include <memory>
#include <optional>
#include <filesystem>
#include <omp.h>
#include "camera.h"
#include "geometry/mesh.h"
#include "geometry/ray_trace_mesh.h"

struct Keypoint {
  int id;
  Vector3f position;
  Keypoint(int id, const Vector3f& p) : id(id), position(p) {}
  Keypoint(int id) : id(id), position(Vector3f::Zero()) {}
};

enum ActiveTool {
  AddKeypointToolId,
  MoveKeypointToolId
};

class SceneModel {
private:
  std::filesystem::path datasetPath;

  std::shared_ptr<geometry::TriangleMesh> mesh;
  const geometry::RayTraceMesh rtMesh;

  // Keypoints.
  std::vector<Keypoint> keypoints;
public:
  int activeKeypoint = -1;
  ActiveTool activeToolId = AddKeypointToolId;

  SceneModel(const std::string& datasetFolder);

  std::shared_ptr<geometry::TriangleMesh> getMesh() const;
  std::optional<Vector3f> traceRay(const Vector3f& origin, const Vector3f& direction);

  const std::vector<Keypoint>& getKeypoints() const { return keypoints; };
  Keypoint addKeypoint(const Vector3f& p);
  void removeKeypoint(const Keypoint& keypoint);
  void updateKeypoint(int keypointId, Keypoint kp);
  std::optional<Keypoint> getKeypoint(int keypointId) const;
  void setKeypoint(const Keypoint& keypoint);
  void setActiveKeypoint(int id) { activeKeypoint = id; }
  void save() const;

private:
  void initRayTracing();
};
#endif
