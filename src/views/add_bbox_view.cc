#include "views/add_bbox_view.h"
#include "commands/bounding_box.h"

namespace views {

using namespace geometry;

bool boxRayIntersectionTest(const Vector3f& origin, const Vector3f& direction, const BBox& bbox) {
  Vector3f leftBottom = -bbox.dimensions * 0.5;
  Vector3f rightTop = bbox.dimensions * 0.5;
  float t1 = (leftBottom[0] - origin[0]) / direction[0];
  float t2 = (rightTop[0] - origin[0]) / direction[0];
  float t3 = (leftBottom[1] - origin[1]) / direction[1];
  float t4 = (rightTop[1] - origin[1]) / direction[1];
  float t5 = (leftBottom[2] - origin[2]) / direction[2];
  float t6 = (rightTop[2] - origin[2]) / direction[2];
  float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
  float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));
  if (tmax < 0) return false;
  if (tmin > tmax) return false;
  return true;
}

AddBBoxView::AddBBoxView(SceneModel& model, DatasetMetadata& datasetMetadata, Timeline& timeline, int viewId) : views::View3D(viewId),
                                                                                                                sceneModel(model),
                                                                                                                timeline(timeline),
                                                                                                                datasetMetadata(datasetMetadata) {
  rotateControl = std::make_shared<views::controls::RotateControl>(viewId, [&](const Transform<float, 3, Eigen::Affine>& transform) {
    auto bbox = sceneModel.getBoundingBox(sceneModel.activeBBox);
    Quaternionf newRotation;
    newRotation = transform.rotation();
    if (bbox.has_value()) {
      bbox->orientation = newRotation;
      sceneModel.updateBoundingBox(bbox.value());
      sizeControl->setPosition(bbox->position + bbox->orientation * bbox->dimensions * 0.5);
      sizeControl->setOrientation(newRotation);
      translateControl->setOrientation(newRotation);
      position = bbox->position;
      orientation = bbox->orientation;
    }
  });
  translateControl = std::make_shared<views::controls::TranslateControl>(viewId, [&](const Vector3f& newPosition) {
    auto bbox = sceneModel.getBoundingBox(sceneModel.activeBBox);
    if (bbox.has_value()) {
      bbox->position = newPosition;
      sceneModel.updateBoundingBox(bbox.value());
      sizeControl->setPosition(bbox->position + bbox->orientation * bbox->dimensions * 0.5);
      rotateControl->setPosition(bbox->position);
      position = bbox->position;
    }
  });
  sizeControl = std::make_shared<views::controls::TranslateControl>(viewId, [&](const Vector3f& newPos_W) {
    auto bbox = sceneModel.getBoundingBox(sceneModel.activeBBox);
    if (bbox.has_value()) {
      Vector3f diff_W = newPos_W - (bbox->position + bbox->orientation * bbox->dimensions * 0.5);
      Vector3f pos_W = bbox->position + diff_W * 0.5;
      Vector3f newDims_B = bbox->orientation.inverse() * (newPos_W - pos_W) * 2.0;
      bbox->dimensions = newDims_B;
      bbox->position = pos_W;
      position = pos_W;
      dimensions = newDims_B;
      translateControl->setPosition(pos_W);
      rotateControl->setPosition(pos_W);
      sceneModel.updateBoundingBox(bbox.value());
    }
  });
}

void AddBBoxView::refresh() {
  auto bbox = sceneModel.getBoundingBox(sceneModel.activeBBox);
  if (bbox.has_value()) {
    setBoundingBox(bbox.value());
  } else {
    bboxStart.id = -1;
  }
}

bool AddBBoxView::leftButtonUp(const ViewContext3D& viewContext) {
  if (rotateControl->leftButtonUp(viewContext)) {
    auto command = std::make_unique<commands::MoveBBoxCommand>(bboxStart, position, orientation);
    timeline.pushCommand(std::move(command));
    auto bbox = sceneModel.getBoundingBox(sceneModel.activeBBox);
    if (bbox.has_value()) {
      bboxStart = bbox.value();
    }
    return true;
  }

  if (sizeControl->leftButtonUp(viewContext)) {
    auto command = std::make_unique<commands::ResizeBBoxCommand>(bboxStart, dimensions, position);
    timeline.pushCommand(std::move(command));
    auto bbox = sceneModel.getBoundingBox(sceneModel.activeBBox);
    if (bbox.has_value()) {
      bboxStart = bbox.value();
    }
    return true;
  }
  if (translateControl->leftButtonUp(viewContext)) {
    auto command = std::make_unique<commands::MoveBBoxCommand>(bboxStart, position, bboxStart.orientation);
    timeline.pushCommand(std::move(command));
    auto bbox = sceneModel.getBoundingBox(sceneModel.activeBBox);
    if (bbox.has_value()) {
      bboxStart = bbox.value();
    }
    return true;
  }
  auto r_W = viewContext.rayWorld();
  for (const auto& bbox : sceneModel.getBoundingBoxes()) {
    auto o_B = viewContext.camera.getPosition() - bbox.position;
    bool hit = boxRayIntersectionTest(o_B, r_W, bbox);
    if (hit) {
      setBoundingBox(bbox);
      return true;
    }
  }
  const Vector3f& rayDirection = viewContext.camera.computeRayWorld(viewContext.width, viewContext.height,
                                                                    viewContext.mousePositionX, viewContext.mousePositionY);
  Intersection intersection = sceneModel.traceRayIntersection(viewContext.camera.getPosition(), rayDirection);
  if (intersection.hit) {
    InstanceMetadata metadata = datasetMetadata.instanceMetadata[sceneModel.currentClassId];
    Vector3f halfSizeNormal = intersection.normal * metadata.size[2] * 0.5;
    BBox bbox = {.id = -1,
                 .classId = sceneModel.currentClassId,
                 .position = intersection.point - halfSizeNormal,
                 .orientation = Quaternionf::FromTwoVectors(-Vector3f::UnitZ(), intersection.normal), // Z in the direction of the surface normal.
                 .dimensions = metadata.size};
    auto command = std::make_unique<commands::AddBBoxCommand>(bbox);
    timeline.pushCommand(std::move(command));
    if (sceneModel.getBoundingBox(sceneModel.activeBBox).has_value()) {
      setBoundingBox(sceneModel.getBoundingBox(sceneModel.activeBBox).value());
    }
    return true;
  } else {
    return false;
  }
}

bool AddBBoxView::leftButtonDown(const ViewContext3D& context) {
  return rotateControl->leftButtonDown(context) || sizeControl->leftButtonDown(context) || translateControl->leftButtonDown(context);
}

bool AddBBoxView::mouseMoved(const ViewContext3D& context) {
  return rotateControl->mouseMoved(context) || sizeControl->mouseMoved(context) || translateControl->mouseMoved(context);
}

void AddBBoxView::render(const ViewContext3D& context) const {
  if (bboxStart.id != -1) {
    rotateControl->render(context.camera);
    translateControl->render(context.camera);
    sizeControl->render(context.camera);
  }
}

void AddBBoxView::setBoundingBox(const BBox& bbox) {
  translateControl->setPosition(bbox.position);
  translateControl->setOrientation(bbox.orientation);
  rotateControl->setPosition(bbox.position);
  rotateControl->setOrientation(bbox.orientation);
  Vector3f sizePos = bbox.position + (bbox.orientation * bbox.dimensions) * 0.5;
  sizeControl->setOrientation(bbox.orientation);
  sizeControl->setPosition(sizePos);
  bboxStart = bbox;
  sceneModel.activeBBox = bbox.id;
}
} // namespace views
