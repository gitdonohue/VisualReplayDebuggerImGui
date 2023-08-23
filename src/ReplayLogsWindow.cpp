#include "ReplayLogsWindow.hpp"
#include "ReplayContext.hpp"

#include <imgui.h>
#include <imgui_internal.h>

#include <string>

using namespace VisualReplayDebugger;

ReplayLogsWindow::ReplayLogsWindow(ReplayContext& ctx)
    : replayContext(ctx)
{
    // Pre-build log headers
    log_headers.reserve(replayContext.GetReplayData().LogCount());
    for (const LogEntry& entry : replayContext.GetReplayData().GetLogs())
    {
        char buff[1024];
        snprintf(buff, 1024, "%d %f (%d) [%s] ", entry.id, entry.time, entry.frame, entry.category.c_str());
        log_headers.emplace_back(buff);
    }
}

void ReplayLogsWindow::Draw()
{
    auto& g = *GImGui;
    const auto& replayData = replayContext.GetReplayData();

    replayContext.hoveredLogEntry = nullptr;

    bool frameChanged = false;
    if (last_frame != replayContext.cursorFrame)
    {
        last_frame = replayContext.cursorFrame;
        frameChanged = true;
    }

    bool roiChanged = false;
    if (last_roi != replayContext.roi)
    {
        last_roi = replayContext.roi;
        roiChanged = true;
    }

    int firstLine = 0;
    for (const auto& entry : replayData.GetLogs())
    {
        if (entry.frame >= replayContext.roi.start) break;
        ++firstLine;
    }

    int lastLine = 0;
    for (const auto& entry : replayData.GetLogs())
    {
        if (entry.frame > replayContext.roi.end) break;
        ++lastLine;
    }
    int numLines = lastLine - firstLine;

    if (!ImGui::Begin("ReplayLogsWindow"))
    {
        ImGui::End();
        return;
    }

    // Main window
    if (ImGui::Button("Options"))
        ImGui::OpenPopup("Options");
    ImGui::SameLine();
    bool clear = ImGui::Button("Clear");
    ImGui::SameLine();
    bool copy = ImGui::Button("Copy");
    //ImGui::SameLine();
    //Filter.Draw("Filter", -100.0f);

    ImGui::Separator();

    if (ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar))
    {
        //if (copy)
        //    ImGui::LogToClipboard();

        const ImU32 col_text = ImGui::GetColorU32(ImGuiCol_Text);
        const ImU32 col_active_frame = ImGui::GetColorU32(ImGuiCol_NavHighlight);
        
        const ImGuiStyle& style = g.Style;
        const auto blockHeight = ImGui::GetTextLineHeight();// +(style.FramePadding.y * 2);
        ImDrawList& draw_list = *ImGui::GetWindowDrawList();

        int line_count = 0;
        int autoscroll_line = -1;

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        {
            ImGuiListClipper clipper;
            clipper.Begin(lastLine - firstLine);
            while (clipper.Step())
            {
                for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
                {
                    const auto& entry = replayData.GetLogEntry(firstLine + line_no);

                    ImVec2 block_origin = ImGui::GetCursorScreenPos();

                    if (entry.frame == replayContext.cursorFrame)
                    {
                        ImVec2 block_bottom = block_origin;
                        block_bottom.x += 300;
                        block_bottom.y += blockHeight;
                        ImColor col = col_active_frame;
                        draw_list.AddRectFilledMultiColor(block_origin, block_bottom, col, col & 0x00FFFFFF, col & 0x00FFFFFF, col);
                    }

                    const char* line_start = entry.message.c_str();
                    const char* line_end = line_start + entry.message.length();

                    ImGui::PushStyleColor(ImGuiCol_Text, col_text);
                    ImGui::TextUnformatted(log_headers.at(firstLine + line_no).c_str());
                    ImGui::PopStyleColor();
                    bool isHeaderHovered = ImGui::IsItemHovered();

                    ImGui::SameLine();
                    ImGui::PushStyleColor(ImGuiCol_Text, ReplayData::GetColorValue(entry.color));
                    ImGui::TextUnformatted(line_start, line_end);
                    ImGui::PopStyleColor();
                    bool isTextHovered = ImGui::IsItemHovered();

                    if (isHeaderHovered || isTextHovered)
                    {
                        replayContext.hoveredLogEntry = &entry;

                        if (ImGui::IsMouseClicked(0, false))
                        {
                            replayContext.cursorFrame = entry.frame;
                        }
                        //ImVec2 block_bottom = block_origin;
                        //block_bottom.x += 300;
                        //block_bottom.y += blockHeight;
                        //ImColor col = col_active_frame;
                        //draw_list.AddRectFilledMultiColor(block_origin, block_bottom, col, col & 0x00FFFFFF, col & 0x00FFFFFF, col);
                    }

                    ++line_count;
                    if (entry.frame <= replayContext.cursorFrame)
                    {
                        autoscroll_line = line_count;
                    }
                }
            }
            clipper.End();
        }
        ImGui::PopStyleVar();

        // Auto-scroll
        if (frameChanged)// && entry.frame == replayContext.cursorFrame)
        {
            ImGui::SetScrollFromPosY(autoscroll_line * blockHeight, 0.5f);
        }
    }
    ImGui::EndChild();
    ImGui::End();
    
}

