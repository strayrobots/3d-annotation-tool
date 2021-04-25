#include "tools/tool.h"

namespace tools {
Tool::Tool(SceneModel& model, CommandStack& stack) : sceneModel(model), commandStack(stack) {}

} // namespace tools
