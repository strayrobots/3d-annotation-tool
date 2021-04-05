#ifndef H_COMMAND
#define H_COMMAND
#include "studio_view.h"
#include "scene_model.h"

namespace commands {

class Command {
  /*
   * A command that changes the state of the scene model.
   * Has to implement both an execute and undo method. The undo method should restore
   * the state of the scene model back to where it was prior to executing the command.
   **/
public:
  virtual ~Command() {};
  virtual void execute(StudioView& studio, SceneModel& sceneModel) = 0;
  virtual void undo(StudioView& studio, SceneModel& sceneModel) = 0;
};
}
#endif
