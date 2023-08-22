#include "ReplayEntitiesWindow.hpp"
#include "ReplayContext.hpp"

#include <imgui.h>
#include <string>

using namespace VisualReplayDebugger;

ReplayEntitiesWindow::ReplayEntitiesWindow(ReplayContext& ctx)
    : replayContext(ctx)
{

}

void ReplayEntitiesWindow::Draw()
{
    const auto& replayData = replayContext.GetReplayData();
}
