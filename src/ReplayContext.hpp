#pragma once

#include "ReplayData.hpp"

namespace VisualReplayDebugger
{
	class ReplayContext
	{
	public:
		ReplayContext(const ReplayData& replayData);
		
		inline const ReplayData& GetReplayData() { return replayData; }

	private:
		const ReplayData& replayData;
	};
};