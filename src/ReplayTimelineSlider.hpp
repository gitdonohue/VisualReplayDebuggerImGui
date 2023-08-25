#pragma once

#include "ReplayWidgetBase.hpp"

namespace VisualReplayDebugger
{
	class ReplayTimelineSlider : public ReplayWidgetBase
	{
	public:
		inline ReplayTimelineSlider(ReplayContext& ctx) : ReplayWidgetBase(ctx) {};
	protected:
		inline void DataChanged() override {}
		void DrawImpl() override;
	};

	class ReplayTimelineWindow : public ReplayWidgetBase
	{
	public:
		inline ReplayTimelineWindow(ReplayContext& ctx) : ReplayWidgetBase(ctx), slider(ctx) {};
	protected:
		inline void DataChanged() override {}
		void DrawImpl() override;
	private:
		ReplayTimelineSlider slider;
	};
};