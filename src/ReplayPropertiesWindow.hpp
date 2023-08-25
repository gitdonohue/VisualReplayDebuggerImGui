#pragma once

#include "ReplayWidgetBase.hpp"

#include <mutil.h>

namespace VisualReplayDebugger
{
	class ReplayPropertiesWindow : public ReplayWidgetBase
	{
	public:
		inline ReplayPropertiesWindow(ReplayContext& ctx) : ReplayWidgetBase(ctx) {};
	
	protected:
		void DataChanged() override;
		void DrawImpl() override;
	};
};