#include "ReplayPropertiesWindow.hpp"

#include <imgui.h>
#include <string>
#include <imgui_internal.h>

using namespace VisualReplayDebugger;

void ReplayPropertiesWindow::DataChanged() {}

void ReplayPropertiesWindow::DrawImpl()
{
    if (!ImGui::Begin("ReplayPropertiesWindow"))
    {
        ImGui::End();
        return;
    }

    for (const Entity* entity : replayContext.selectedEntities)
    {
        // section title: name
        ImGui::SeparatorText(entity->Name.c_str());

        // dynamic values
        for (const auto& valueEntry : entity->DynamicValues)
        {
            const float& valueAtFrame = valueEntry.second.AtFrame(replayContext.cursorFrame);
            char buf[32];
            snprintf(buf, 32, "%g", valueAtFrame);
            ImGui::LabelText(buf, "%s", valueEntry.first.c_str());
        }

        // dynamic properties
        for (const auto& valueEntry : entity->DynamicProperties)
        {
            const std::string& valueAtFrame = valueEntry.second.AtFrame(replayContext.cursorFrame);
            ImGui::LabelText(valueAtFrame.c_str(), "%s", valueEntry.first.c_str());
        }

        // static properties
        for (const auto& staticParam : entity->StaticParameters)
        {
            ImGui::LabelText(staticParam.first.c_str(), "%s", staticParam.second.c_str());
        }
    }

    ImGui::End();
}