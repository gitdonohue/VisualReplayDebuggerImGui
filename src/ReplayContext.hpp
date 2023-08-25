#pragma once

#include "ReplayData.hpp"

#include <set>

namespace VisualReplayDebugger
{
	struct Entity;
	struct LogEntry;

	// Replay context holds the global time cursor and window.
	class ReplayContext
	{
	public:
		ReplayContext();
		ReplayContext(const ReplayData&);
		
		void SetData(const ReplayData&);
		void Reset();
		inline const ReplayData& GetReplayData() const { return *replayData; }

	private:
		ReplayData const* replayData;
		ReplayData _dummyReplayData;

	public:
		int cursorFrame = 0;
		FrameRange roi;

		const LogEntry* hoveredLogEntry = nullptr;
		std::set<const Entity*> selectedEntities;

		int dataGen = 0;
	};
};