#pragma once
#include "ReplayContext.hpp"

namespace VisualReplayDebugger
{
	class ReplayWidgetBase
	{
	public:
		inline ReplayWidgetBase(ReplayContext& replayContext) : replayContext(replayContext){}
		
	protected:
		virtual void DataChanged() = 0;
		virtual void DrawImpl() = 0;

	public:
		inline void Draw()
		{
			if (replayContext.dataGen != lastDataGen)
			{
				lastDataGen = replayContext.dataGen;
				DataChanged();
			}
			DrawImpl();
		}

	protected:
		ReplayContext& replayContext;

	private:
		int lastDataGen = -1;
	};
}