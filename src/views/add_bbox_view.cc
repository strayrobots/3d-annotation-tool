#include "views/add_bbox_view.h"
#include "commands/bounding_box.h"

namespace views {

using namespace geometry;

AddBBoxView::AddBBoxView(SceneModel& model, DatasetMetadata& datasetMetadata, Timeline& timeline, int viewId) : views::View3D(viewId), sceneModel(model), timeline(timeline), datasetMetadata(datasetMetadata) {
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
    bboxStart = bbox.value();
    rotateControl->setPosition(bbox->position);
    rotateControl->setOrientation(bbox->orientation);

    translateControl->setPosition(bbox->position);
    translateControl->setOrientation(bbox->orientation);

    sizeControl->setPosition(bbox->position + bbox->orientation * bbox->dimensions * 0.5);
    sizeControl->setOrientation(bbox->orientation);
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
  const Vector3f& rayDirection = viewContext.camera.computeRayWorld(viewContext.width, viewContext.height,
                                                                    viewContext.mousePositionX, viewContext.mousePositionY);
  Intersection intersection = sceneModel.traceRayIntersection(viewContext.camera.getPosition(), rayDirection);
  if (intersection.hit) {
    InstanceMetadata metadata = datasetMetadata.instanceMetadata[sceneModel.currentInstanceId];
    Vector3f halfSizeNormal = intersection.normal * metadata.size[2] * 0.5;
    BBox bbox = {.id = -1,
                 .instanceId = sceneModel.currentInstanceId,
                 .position = intersection.point - halfSizeNormal,
                 .orientation = Quaternionf::FromTwoVectors(-Vector3f::UnitZ(), intersection.normal), // Z in the direction of the surface normal.
                 .dimensions = metadata.size};
    translateControl->setPosition(bbox.position);
    translateControl->setOrientation(bbox.orientation);
    rotateControl->setPosition(bbox.position);
    rotateControl->setOrientation(bbox.orientation);
    Vector3f sizePos = bbox.position + (bbox.orientation * bbox.dimensions) * 0.5;
    sizeControl->setOrientation(bbox.orientation);
    sizeControl->setPosition(sizePos);
    auto command = std::make_unique<commands::AddBBoxCommand>(bbox);
    timeline.pushCommand(std::move(command));
    if (sceneModel.getBoundingBox(sceneModel.activeBBox).has_value()) {
      bboxStart = sceneModel.getBoundingBox(sceneModel.activeBBox).value();
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
} // namespace views
