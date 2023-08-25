#include "ReplayContext.hpp"

using namespace VisualReplayDebugger;

ReplayContext::ReplayContext() : replayData(&_dummyReplayData) {}
ReplayContext::ReplayContext(const ReplayData& replayData) : replayData(&replayData) {}

void ReplayContext::SetData(const ReplayData& _replayData)
{
	replayData = &_replayData;
	++dataGen;
	Reset();
}

void ReplayContext::Reset()
{
	cursorFrame = 0;
	roi.start = 0;
	roi.end = replayData->FrameCount();
	hoveredLogEntry = nullptr;
	selectedEntities.clear();

	// test: select first entry
	if (replayData->GetEntities().size() > 0)
	{
		const Entity* entry = replayData->GetEntity(1);
		selectedEntities.insert(entry);
	}
}