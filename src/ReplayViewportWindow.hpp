#pragma once

#include "ReplayContext.hpp"

#include <string>
#include <vector>
#include <mutil.h>

namespace VisualReplayDebugger
{
	class ReplayViewportWindow
	{
	public:
		ReplayViewportWindow(ReplayContext&);
		void Draw();
	private:
		ReplayContext& replayContext;

		mutil::Matrix4 view_matrix;
		mutil::Matrix4 model_matrix;
		mutil::Matrix4 projection_matrix;
	};
};