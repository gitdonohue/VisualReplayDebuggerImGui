#pragma once

#include "ReplayData.hpp"

namespace VisualReplayDebugger
{
	struct LogEntry;

	// Replay context holds the global time cursor and window.
	class ReplayContext
	{
	public:
		ReplayContext(const ReplayData& replayData);
		
		inline const ReplayData& GetReplayData() { return replayData; }

	private:
		const ReplayData& replayData;

	public:
		int cursorFrame = 0;
		FrameRange roi;

		const LogEntry* hoveredLogEntry;
	};
};