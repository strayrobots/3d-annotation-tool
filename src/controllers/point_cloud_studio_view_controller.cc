#include "controllers/point_cloud_studio_view_controller.h"
#include "commands/keypoints.h"

using namespace commands;
using namespace views;

PointCloudStudioViewController::PointCloudStudioViewController(SceneModel& model, Timeline& tl) : StudioViewController(model, tl){};