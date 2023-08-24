#pragma once

#include "ReplayWidgetBase.hpp"

namespace VisualReplayDebugger
{
	class ReplayGraphWindow : public ReplayWidgetBase
	{
	public:
		inline ReplayGraphWindow(ReplayContext& ctx) : ReplayWidgetBase(ctx) {}
		inline void DataChanged() override {}
		void DrawImpl() override;
	};

	class ReplayGraph : public ReplayWidgetBase
	{
	public:
		inline ReplayGraph(ReplayContext& ctx) : ReplayWidgetBase(ctx) {}
		inline void DataChanged() override {}
		void DrawImpl() override;
	};
};