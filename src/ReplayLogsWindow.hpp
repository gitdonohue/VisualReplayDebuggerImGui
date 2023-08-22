#pragma once

#include "ReplayContext.hpp"

#include <string>
#include <vector>

namespace VisualReplayDebugger
{
	class ReplayLogsWindow
	{
	public:
		ReplayLogsWindow(ReplayContext&);
		void Draw();
	private:
		ReplayContext& replayContext;

		std::vector<std::string> log_headers;
	};
};