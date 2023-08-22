#include "ReplayLogsWindow.hpp"
#include "ReplayContext.hpp"

#include <imgui.h>
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
    const auto& replayData = replayContext.GetReplayData();

    if (!ImGui::Begin("title"))
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

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        //if (Filter.IsActive())
        //{
        //    // In this example we don't use the clipper when Filter is enabled.
        //    // This is because we don't have random access to the result of our filter.
        //    // A real application processing logs with ten of thousands of entries may want to store the result of
        //    // search/filter.. especially if the filtering function is not trivial (e.g. reg-exp).
        //    for (int line_no = 0; line_no < LineOffsets.Size; line_no++)
        //    {
        //        const char* line_start = buf + LineOffsets[line_no];
        //        const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
        //        if (Filter.PassFilter(line_start, line_end))
        //            ImGui::TextUnformatted(line_start, line_end);
        //    }
        //}
        //else
        {
            ImGuiListClipper clipper;
            clipper.Begin(replayData.LogCount());
            while (clipper.Step())
            {
                for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
                {
                    const auto& entry = replayData.GetLogEntry(line_no);

                    const char* line_start = entry.message.c_str();
                    const char* line_end = line_start + entry.message.length();

                    ImGui::TextUnformatted(log_headers.at(line_no).c_str());
                    ImGui::SameLine();

                    ImGui::PushStyleColor(ImGuiCol_Text, ReplayData::GetColorValue(entry.color));
                    ImGui::TextUnformatted(line_start, line_end);
                    ImGui::PopStyleColor();
                }
            }
            clipper.End();
        }
        ImGui::PopStyleVar();

        // Keep up at the bottom of the scroll region if we were already at the bottom at the beginning of the frame.
        // Using a scrollbar or mouse-wheel will take away from the bottom edge.
        //if (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        //    ImGui::SetScrollHereY(1.0f);
    }
    ImGui::EndChild();
    ImGui::End();
    
}

