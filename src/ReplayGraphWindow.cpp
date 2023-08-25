#include "ReplayGraphWindow.hpp"

#include "ReplayContext.hpp"

#include <imgui.h>
#include <string>
#include <algorithm>

using namespace VisualReplayDebugger;

void ReplayGraphWindow::DataChanged()
{
    dataPoints.clear();
    const auto& replayData = replayContext.GetReplayData();
    const int frameCount = replayData.FrameCount();
    workBuffer.resize(frameCount);
    for (const Entity* entity : replayData.GetEntities())
    {
        for (const auto& vals : entity->DynamicValues)
        {
            const std::string& label = vals.first;
            const auto& values = vals.second;

            float minValue = 0;
            float maxValue = 1;
            for (const auto& datapoint : values)
            {
                float val = datapoint.second;
                minValue = std::min(minValue, val);
                maxValue = std::max(maxValue, val);
            }
            minValue = std::max(minValue, 0.0f); // clamp min to zero
            
            std::vector<float> pts;
            pts.resize(frameCount);

            // Note: this is highly ineficient O(n^2)
            // TODO: O(n)
            const float yScale = 1.0f / maxValue; // normalize to max height
            for (int i = 0; i < frameCount; ++i)
            {
                pts[i] = values.AtFrame(i) * yScale;
            }
            dataPoints[entity].emplace(label, pts);
        }
    }
}

void ReplayGraphWindow::DrawImpl()
{
    if (!ImGui::Begin("ReplayGraphWindow"))
    {
        ImGui::End();
        return;
    }

    const int frameCount = replayContext.GetReplayData().FrameCount();
    ImDrawList& draw_list = *ImGui::GetWindowDrawList();

    for (const Entity* entity : replayContext.selectedEntities)
    {
        // section title: name
        ImGui::SeparatorText(entity->Name.c_str());

        float graphHeight = 100;

        // dynamic values
        for (const auto& valueEntry : entity->DynamicValues)
        {
            const auto& timestampedList = valueEntry.second;
            const float& valueAtFrame = timestampedList.AtFrame(replayContext.cursorFrame);
            char buf[32];
            snprintf(buf, 32, "%g", valueAtFrame);
            ImGui::LabelText(buf, "%s", valueEntry.first.c_str());

            const auto& pts = dataPoints[entity][valueEntry.first];

            ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();      // ImDrawList API uses screen coordinates!
            ImVec2 canvas_sz(ImGui::GetContentRegionAvail().x, graphHeight);   // Resize canvas to what's available

            ImGui::InvisibleButton(valueEntry.first.c_str(), canvas_sz);

            ImVec2 canvas_p1(canvas_p0.x + canvas_sz.x, canvas_p0.y + graphHeight);
            draw_list.PushClipRect(canvas_p0, canvas_p1, true);

            //draw_list.PathClear();
            
            draw_list.PathRect(canvas_p0, canvas_p1, 5);
            draw_list.PathStroke(IM_COL32(255, 0, 0, 255), 0, 1.5f);
            //draw_list.AddLine(canvas_p0,canvas_p1, IM_COL32(255, 0, 0, 255), 1.0f);

            // Transform all datapoints
            // This could be optimized
            const float biasX = canvas_p0.x;
            const float scaleX = canvas_sz.x / frameCount;
            const float scaleY = graphHeight; // assuming data normalized
            for (int i = 0; i < frameCount; ++i)
            {
                float v = pts[i];
                auto& pt = workBuffer[i];
                pt.x = canvas_p0.x + i * scaleX;
                pt.y = (canvas_p0.y + graphHeight) - ( v * scaleY );
            }

            //draw_list.AddConvexPolyFilled(reinterpret_cast<ImVec2*>(&workBuffer[0]), workBuffer.size(), IM_COL32(0, 255, 0, 64));
            draw_list.AddPolyline(reinterpret_cast<ImVec2*>(&workBuffer[0]), workBuffer.size(), IM_COL32(0, 255, 0, 255), ImDrawFlags_None, 2.0f);
            
            // draw cursor
            float cursorX = canvas_p0.x + canvas_sz.x * replayContext.cursorFrame / frameCount;
            draw_list.AddLine(ImVec2(cursorX, canvas_p0.y), ImVec2(cursorX, canvas_p0.y + graphHeight), IM_COL32(255, 0, 0, 255), 2.0f);

            draw_list.PopClipRect();
        }
    }

    ImGui::End();
}

//
//
//

void ReplayGraph::DrawImpl()
{
}