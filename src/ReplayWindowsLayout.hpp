#pragma once
#include "ReplayWidgetBase.hpp"

#include <ReplayLogsWindow.hpp>
#include <ReplayEntitiesWindow.hpp>
#include <ReplayTimelineSlider.hpp>
#include <ReplayViewportWindow.hpp>
#include <ReplayGraphWindow.hpp>

namespace VisualReplayDebugger
{
	class ReplayWindowsLayout : public ReplayWidgetBase
	{
	public:
		ReplayWindowsLayout(ReplayContext&);
	protected:
		inline void DataChanged() override {}
		void DrawImpl() override;

	private:
		VisualReplayDebugger::ReplayLogsWindow logsWindow;
		VisualReplayDebugger::ReplayEntitiesWindow entitiesWindow;
		VisualReplayDebugger::ReplayTimelineWindow timelineWindow;
		VisualReplayDebugger::ReplayViewportWindow viewport;
		VisualReplayDebugger::ReplayGraphWindow graphWindow;
	};
}