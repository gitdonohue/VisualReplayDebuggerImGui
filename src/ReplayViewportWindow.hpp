#pragma once

#include "ReplayWidgetBase.hpp"

#include <mutil.h>

namespace VisualReplayDebugger
{
	class ReplayViewportWindow : public ReplayWidgetBase
	{
	public:
		inline ReplayViewportWindow(ReplayContext& ctx) : ReplayWidgetBase(ctx) {};
	
	protected:
		void DataChanged() override;
		void DrawImpl() override;
	private:

		mutil::Matrix4 view_matrix;
		mutil::Matrix4 model_matrix;
		mutil::Matrix4 projection_matrix;
	};
};