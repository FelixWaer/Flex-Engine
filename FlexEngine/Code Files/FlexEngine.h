#pragma once

#include "Rendering/EngineApplication.h"

namespace FXE
{
	class FlexEngine
	{
	public:
		FlexEngine() = default;

		Renderer GameRenderer;

		void init_Engine();
		void cleanup_Engine();
		void game_Tick();

	private:

	};
}


