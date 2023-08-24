#include "ReplayViewportWindow.hpp"

#include <imgui.h>
#include <string>
#include <imgui_internal.h>

#include "ImGuizmo\ImGuizmo.h"

using namespace VisualReplayDebugger;
using namespace mutil;


void ReplayViewportWindow::DataChanged()
{
    Vector3 eye(1, -1, 1);
    Vector3 target(0, 0, 0);
    Vector3 up(0, 0, 1);
    //Matrix4 model_matrix; // identity
    view_matrix = mutil::lookAt(eye, target, up);
    projection_matrix = mutil::ortho(-1, 1, -1, 1, 0.01f, 100.0f);
}

void ReplayViewportWindow::DrawImpl()
{
    if (!ImGui::Begin("ReplayViewportWindow"))
    {
        ImGui::End();
        return;
    }

    ImDrawList& draw_list = *ImGui::GetWindowDrawList();

    //{
        static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
        static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
        //if (ImGui::IsKeyPressed(90))
        //    mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
        //if (ImGui::IsKeyPressed(69))
        //    mCurrentGizmoOperation = ImGuizmo::ROTATE;
        //if (ImGui::IsKeyPressed(82)) // r Key
        //    mCurrentGizmoOperation = ImGuizmo::SCALE;
        if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
            mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
            mCurrentGizmoOperation = ImGuizmo::ROTATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
            mCurrentGizmoOperation = ImGuizmo::SCALE;
        float matrixTranslation[3], matrixRotation[3], matrixScale[3];
        ImGuizmo::DecomposeMatrixToComponents(model_matrix.mat, matrixTranslation, matrixRotation, matrixScale);
        ImGui::InputFloat3("Tr", matrixTranslation);
        ImGui::InputFloat3("Rt", matrixRotation);
        ImGui::InputFloat3("Sc", matrixScale);
        ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, model_matrix.mat);

        if (mCurrentGizmoOperation != ImGuizmo::SCALE)
        {
            if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
                mCurrentGizmoMode = ImGuizmo::LOCAL;
            ImGui::SameLine();
            if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
                mCurrentGizmoMode = ImGuizmo::WORLD;
        }
    //}

    {
        ImGui::BeginChild("ReplayViewport");

        ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();      // ImDrawList API uses screen coordinates!
        ImVec2 canvas_sz = ImGui::GetContentRegionAvail();   // Resize canvas to what's available
        ImVec2 canvas_p1(canvas_p0.x+canvas_sz.x, canvas_p0.y+canvas_sz.y);

        draw_list.PushClipRect(canvas_p0, canvas_p1, true);

        draw_list.AddCircleFilled(canvas_p0, 200, 0xFFFFFFFF);

        ImGuizmo::BeginFrame();
        ImGuizmo::SetOrthographic(true);
        ImGuizmo::SetRect(canvas_p0.x, canvas_p0.y, canvas_sz.x, canvas_sz.y);
        ImGuizmo::DrawCubes(view_matrix.mat, projection_matrix.mat, model_matrix.mat, 1);
        ImGuizmo::Manipulate(view_matrix.mat, projection_matrix.mat, mCurrentGizmoOperation, mCurrentGizmoMode, model_matrix.mat);
        ImGuizmo::DrawGrid(view_matrix.mat, projection_matrix.mat, model_matrix.mat, 0.1f);

        draw_list.PopClipRect();

        ImGui::EndChild();
    }

    ImGui::End();
}