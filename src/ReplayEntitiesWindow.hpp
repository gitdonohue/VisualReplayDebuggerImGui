#pragma once

#include "ReplayWidgetBase.hpp"

#include <string>
#include <vector>

namespace VisualReplayDebugger
{
	class ReplayEntitiesWindow : public ReplayWidgetBase
	{
	public:
		inline ReplayEntitiesWindow(ReplayContext& ctx) : ReplayWidgetBase(ctx) {}
	protected:
		inline void DataChanged() override {}
		void DrawImpl() override;
	private:
		std::vector<std::string> log_headers;
	};
};