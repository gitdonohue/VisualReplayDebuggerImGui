#pragma once

#include "ReplayWidgetBase.hpp"

#include <string>
#include <vector>

namespace VisualReplayDebugger
{
	class ReplayLogsWindow : public ReplayWidgetBase
	{
	public:
		inline ReplayLogsWindow(ReplayContext& ctx) : ReplayWidgetBase(ctx) {};
	protected:
		void DataChanged() override;
		void DrawImpl() override;
	private:
		std::vector<std::string> log_headers;
		int last_frame = -1;
		FrameRange last_roi = {};
	};
};