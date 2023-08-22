#pragma once

#include "ReplayContext.hpp"

#include <string>
#include <vector>

namespace VisualReplayDebugger
{
	class ReplayTimelineSlider
	{
	public:
		ReplayTimelineSlider(ReplayContext&);
		void Draw();
	private:
		ReplayContext& replayContext;
	};

	class ReplayTimelineWindow
	{
	public:
		ReplayTimelineWindow(ReplayContext&);
		void Draw();
	private:
		ReplayContext& replayContext;

		ReplayTimelineSlider slider;
	};
};