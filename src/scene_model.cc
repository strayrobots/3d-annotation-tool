#include <fstream>
#include <iostream>
#include "scene_model.h"

SceneModel::SceneModel(const std::string& datasetFolder) : camera(Vector3f(0.0, 1.0, 0.0)), datasetPath(datasetFolder) {
  auto scenePath = datasetPath / "scene" / "integrated.ply";
  mesh = std::make_shared<geometry::Mesh>(scenePath.string());
  initRayTracing();
}

std::shared_ptr<geometry::TriangleMesh> SceneModel::getMesh() const { return mesh; }
const Camera& SceneModel::getCamera() const { return camera; }

std::optional<Vector3f> SceneModel::traceRay(double x, double y) {
  nanort::Ray<float> ray;
  ray.min_t = 0.0;
  ray.max_t = 1e9f;

  float aspectRatio = float(Width) / float(Height);
  float fov = camera.fov;
  float pX = (2.0f * (x / Width) - 1.0f) * std::tan(fov / 2.0f * M_PI / 180) * aspectRatio;
  float pY = (1.0f - 2.0f * (y / Height)) * std::tan(fov / 2.0f * M_PI / 180);
  Vector3f cameraRay(pX, pY, 1.0f);
  cameraRay = camera.getOrientation() * cameraRay.normalized();

  Vector3f rayOrigin = camera.getPosition();
  ray.org[0] = rayOrigin[0];
  ray.org[1] = rayOrigin[1];
  ray.org[2] = rayOrigin[2];

  ray.dir[0] = cameraRay[0];
  ray.dir[1] = cameraRay[1];
  ray.dir[2] = cameraRay[2];
  const auto& faces = mesh->faces();
  nanort::TriangleIntersector<float, nanort::TriangleIntersection<float>> triangleIntersector(mesh->vertices().data(), faces.data(), sizeof(float) * 3);
  nanort::TriangleIntersection<float> isect;
  bool pointingAtMesh = bvh.Traverse(ray, triangleIntersector, &isect);
  if (pointingAtMesh) {
    uint32_t faceId = isect.prim_id;
    const auto& face = faces.row(faceId);
    const Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor>& vertices = mesh->vertices();
    auto vertex1 = vertices.row(face[0]);
    auto vertex2 = vertices.row(face[1]);
    auto vertex3 = vertices.row(face[2]);
    Vector3f point = (1.0f - isect.u - isect.v) * vertex1 + isect.u * vertex2 + isect.v * vertex3;
    return std::make_optional(point);
  }
  return {};
}

void SceneModel::popKeypoint() {
  if (keypoints.empty()) return;
  keypoints.pop_back();
}

void SceneModel::setCameraOrientation(const Quaternionf& rotation) {
  camera.setOrientation(rotation);
}
void SceneModel::setCameraPosition(const Vector3f& position) {
  camera.setPosition(position);
}

void SceneModel::save() const {
  auto keypointPath = datasetPath / "keypoints.json";
  nlohmann::json json = nlohmann::json::array();
  for (size_t i = 0; i < keypoints.size(); i++) {
    json[i] = { {"x", keypoints[i][0]}, {"y", keypoints[i][1]}, {"z", keypoints[i][2]} };
  }
  std::ofstream file(keypointPath.string());
  file << json;
  std::cout << "Saved keypoints to keypoints.json" << std::endl;
}

void SceneModel::initRayTracing() {
  const auto& faces = mesh->faces();
  const float* vertices = mesh->vertices().data();
  const uint32_t* indices = faces.data();
  nanoMesh = std::make_unique<nanort::TriangleMesh<float>>(vertices, indices, sizeof(float) * 3);
  triangle_pred = std::make_unique<nanort::TriangleSAHPred<float>>(vertices, indices, sizeof(float) * 3);
  nanort::BVHBuildOptions<float> build_options;
  std::cout << "Building bounding volume hierarchy.\r" << std::flush;
  auto ret = bvh.Build(faces.rows(), *nanoMesh.get(), *triangle_pred.get(), build_options);
  assert(ret);
  std::cout << "Done building bounding volume hierarchy." << std::endl;
  nanort::BVHBuildStatistics stats = bvh.GetStatistics();
}

