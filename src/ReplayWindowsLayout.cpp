#include "ReplayWindowsLayout.hpp"

using namespace VisualReplayDebugger;

ReplayWindowsLayout::ReplayWindowsLayout(ReplayContext& ctx)
	: ReplayWidgetBase(ctx),
	logsWindow(ctx),
	entitiesWindow(ctx),
	timelineWindow(ctx),
	viewport(ctx),
	graphWindow(ctx),
	propertiesWindow(ctx)
{

}

void ReplayWindowsLayout::DrawImpl()
{
	timelineWindow.Draw();
	entitiesWindow.Draw();
	logsWindow.Draw();
	graphWindow.Draw();
	viewport.Draw();
	propertiesWindow.Draw();
}