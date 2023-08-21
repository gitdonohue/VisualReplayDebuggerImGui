#pragma once

#include <ReplayContext.hpp>

namespace VisualReplayDebugger
{
	class ReplayLogsWindow
	{
	public:
		ReplayLogsWindow(ReplayContext&);
		void Draw();
	private:
		ReplayContext& replayContext;
	};
};