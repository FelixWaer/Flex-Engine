#include "FlexEngine.h"

#include "FlexLibrary/Flextimer.h"

namespace FXE
{
	void FlexEngine::init_Engine()
	{
		FlexTimer::clear_Timer();
		GameRenderer.initVulkan();
	}

	void FlexEngine::cleanup_Engine()
	{
		GameRenderer.cleanup();
	}

	void FlexEngine::game_Tick()
	{
		GameRenderer.mainLoop();
	}


}
