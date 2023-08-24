#include "ReplayGraphWindow.hpp"

#include "ReplayContext.hpp"

#include <imgui.h>
#include <string>
#include <algorithm>

using namespace VisualReplayDebugger;


void ReplayGraphWindow::DrawImpl()
{
    if (!ImGui::Begin("ReplayGraphWindow"))
    {
        ImGui::End();
        return;
    }



    ImGui::End();
}

//
//
//

void ReplayGraph::DrawImpl()
{
}