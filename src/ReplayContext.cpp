#include "ReplayContext.hpp"

using namespace VisualReplayDebugger;

ReplayContext::ReplayContext(const ReplayData& replayData)
	: replayData(replayData)
{
	roi.end = replayData.FrameCount();
}
