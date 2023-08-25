#pragma once

#include "ReplayWidgetBase.hpp"
#include "mutil.h"

namespace VisualReplayDebugger
{
	class ReplayGraphWindow : public ReplayWidgetBase
	{
	public:
		inline ReplayGraphWindow(ReplayContext& ctx) : ReplayWidgetBase(ctx) {}
	protected:
		void DataChanged() override;
		void DrawImpl() override;

	private:
		std::map<const Entity*, std::map<std::string, std::vector<float>>> dataPoints;
		std::vector<mutil::Vector2> workBuffer;
	};

	class ReplayGraph : public ReplayWidgetBase
	{
	public:
		inline ReplayGraph(ReplayContext& ctx) : ReplayWidgetBase(ctx) {}
	protected:
		inline void DataChanged() override {}
		void DrawImpl() override;
	};
};