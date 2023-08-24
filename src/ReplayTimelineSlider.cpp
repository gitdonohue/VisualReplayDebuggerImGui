#include "ReplayTimelineSlider.hpp"

#include "ReplayContext.hpp"

#include <imgui.h>
#include <string>
#include <algorithm>

using namespace VisualReplayDebugger;


void ReplayTimelineSlider::DrawImpl()
{
    ImGui::PushItemWidth(-FLT_MIN);
    ImGui::SliderInt("Frame", &replayContext.cursorFrame, 0, replayContext.GetReplayData().FrameCount(), "frame = %d", ImGuiSliderFlags_AlwaysClamp);
    if (ImGui::SliderInt("RoiStart", &replayContext.roi.start, 0, replayContext.GetReplayData().FrameCount(), "frame = %d", ImGuiSliderFlags_AlwaysClamp))
    {
        replayContext.roi.end = std::clamp(replayContext.roi.end, replayContext.roi.start, replayContext.GetReplayData().FrameCount());
    }
    if (ImGui::SliderInt("RoiEnd", &replayContext.roi.end, 0, replayContext.GetReplayData().FrameCount(), "frame = %d", ImGuiSliderFlags_AlwaysClamp))
    {
        replayContext.roi.start = std::clamp(replayContext.roi.start, 0, replayContext.roi.end);
    }
    ImGui::PopItemWidth();
}

//
// 
//


void ReplayTimelineWindow::DrawImpl()
{
    if (!ImGui::Begin("ReplayTimelineWindow"))
    {
        ImGui::End();
        return;
    }

    slider.Draw();

    ImGui::End();
}