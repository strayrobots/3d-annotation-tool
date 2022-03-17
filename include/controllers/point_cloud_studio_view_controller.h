#pragma once
#include "controllers/studio_view_controller.h"
#include "timeline.h"
#include "scene_model.h"

class PointCloudStudioViewController : public StudioViewController {
public:
  PointCloudStudioViewController(SceneModel& model, Timeline& timeline);
};