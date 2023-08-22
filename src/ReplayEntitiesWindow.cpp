#include "ReplayEntitiesWindow.hpp"
#include "ReplayContext.hpp"

#include <imgui.h>
#include <string>
#include <imgui_internal.h>

using namespace VisualReplayDebugger;

ReplayEntitiesWindow::ReplayEntitiesWindow(ReplayContext& ctx)
    : replayContext(ctx)
{

}

void ReplayEntitiesWindow::Draw()
{
    const auto& replayData = replayContext.GetReplayData();

    const ImGuiStyle& style = GImGui->Style;
    const auto blockHeight = ImGui::GetTextLineHeight() + (style.FramePadding.y * 2);
    
    const ImU32 col_bg = ImGui::GetColorU32(ImGuiCol_MenuBarBg);
    const ImU32 col_bg_bottom = ImGui::GetColorU32(ImGuiCol_MenuBarBg) & 0x7FFFFFFF;
    const ImU32 col_active_region_top = ImGui::GetColorU32(ImGuiCol_PlotHistogram);
    const ImU32 col_active_region_bottom = ImGui::GetColorU32(ImGuiCol_PlotHistogram) & 0x7FFFFFFF;
    const ImU32 col_tick = ImGui::GetColorU32(ImGuiCol_NavHighlight);
    const ImU32 col_tick_highlighted = IM_COL32(255, 0, 0, 255);

    if (!ImGui::Begin("ReplayEntitiesWindow", nullptr, ImGuiWindowFlags_None))
    {
        ImGui::End();
        return;
    }
    
    ImDrawList& draw_list = *ImGui::GetWindowDrawList();

    ImVec2 window_origin = ImGui::GetCursorScreenPos();

    if (ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_None))
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

        const float totalTime = replayData.GetTotalTime();
        float windowWidth = ImGui::GetWindowSize().x;
        float windowHeight = ImGui::GetWindowSize().y;
        float margin = 300;
        float blockWidth = windowWidth - margin;
        ImVec2 block_size = ImVec2(blockWidth, blockHeight);

        for (const auto& entity_entry : replayData.GetEntities())
        {
            const Entity& entity = entity_entry.second;
            if (entity.Id < 0) continue;

            const float ratio_start = replayData.GetTimeForFrame(entity.CreationFrame) / totalTime;
            const float ratio_end = replayData.GetTimeForFrame(entity.DestructionFrame) / totalTime;

            ImVec2 block_origin = ImGui::GetCursorScreenPos();
            block_origin.x += margin;

            ImGui::TextUnformatted(entity.Name.c_str());

            ImVec2 p1 = ImVec2(block_origin.x + block_size.x, block_origin.y + block_size.y);
            draw_list.AddRectFilledMultiColor(block_origin, p1, col_bg, col_bg, col_bg_bottom, col_bg_bottom);

            ImVec2 pp0(block_origin.x + ratio_start * blockWidth, block_origin.y);
            ImVec2 pp1(block_origin.x + ratio_end * blockWidth, block_origin.y + blockHeight);
            draw_list.AddRectFilledMultiColor(pp0, pp1, col_active_region_top, col_active_region_top, col_active_region_bottom, col_active_region_bottom);

            // Draw logs ticks
            int lastFrame = -1;
            for (const LogEntry* log : entity.Logs)
            {
                const bool isHovered = replayContext.hoveredLogEntry == log;

                int frame = log->frame;
                if (frame != lastFrame || isHovered)
                {
                    lastFrame = frame;

                    const float position_ratio = replayData.GetTimeForFrame(frame) / totalTime;
                    ImVec2 pl0(block_origin.x + position_ratio * blockWidth, block_origin.y);
                    ImVec2 pl1(pl0.x, pl0.y + blockHeight);
                    ImColor col = isHovered ? col_tick_highlighted : col_tick;
                    draw_list.AddLine(pl0, pl1, col);
                }
            }

            //// Draw frame cursor
            //ImVec2 cursorTop = block_origin;
            //cursorTop.x += blockWidth * replayData.GetTimeForFrame(replayContext.cursorFrame) / totalTime;
            //ImVec2 cursorBottom = cursorTop;
            //cursorBottom.y += blockHeight;
            //draw_list.AddLine(cursorTop, cursorBottom, IM_COL32(255, 0, 0, 255));

            ImGui::SameLine();
            ImGui::InvisibleButton("invisible##", block_size);
        }
        
        ImVec2 block_origin = window_origin;
        block_origin.x += margin;

        // Show ROI (left)
        ImVec2 roiLeftP0 = block_origin;
        ImVec2 roiLeftP1 = roiLeftP0;
        roiLeftP1.x += blockWidth * replayData.GetTimeForFrame(replayContext.roi.start) / totalTime;
        roiLeftP1.y += windowHeight;
        draw_list.AddRectFilled(roiLeftP0, roiLeftP1, IM_COL32(0, 0, 0, 64));

        // Show ROI (right)
        ImVec2 roiRightP0 = block_origin;
        roiRightP0.x += blockWidth * replayData.GetTimeForFrame(replayContext.roi.end) / totalTime;
        ImVec2 roiRightP1 = block_origin;
        roiRightP1.x += blockWidth;
        roiRightP1.y += windowHeight;
        draw_list.AddRectFilled(roiRightP0, roiRightP1, IM_COL32(0, 0, 0, 64));

        // Draw cursor
        ImVec2 cursorTop = block_origin;
        cursorTop.x += blockWidth * replayData.GetTimeForFrame(replayContext.cursorFrame) / totalTime;
        ImVec2 cursorBottom = cursorTop;
        cursorBottom.y += windowHeight;
        draw_list.AddLine(cursorTop, cursorBottom, IM_COL32(255, 0, 0, 255));

        ImGui::PopStyleVar();

        // End scolling window
        ImGui::EndChild();
    }



    ImGui::End();
}
