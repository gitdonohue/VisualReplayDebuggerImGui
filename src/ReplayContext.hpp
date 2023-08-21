#pragma once

#include <ReplayData.hpp>

namespace VisualReplayDebugger
{
	class ReplayContext
	{
	public:
		ReplayContext(const ReplayData& reader);

	public:
		const ReplayData& reader;
	};
};