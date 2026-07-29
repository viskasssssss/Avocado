#include "pch.h"

#ifdef AVO_PLATFORM_WINDOWS

#include "application.h"

using namespace avocado;

int main(int argc, char** argv)
{

	// startup is not final | changes will be made

	logger::init();
	AVO_ERROR("TEST");
	AVO_WARN("TEST");
	AVO_INFO("TEST");
	AVO_TRACE("TEST");
	AVO_CRITICAL("TEST");

	AVO_INFO("Creating application");

	application app = application();
	app.run();
	
	return 0;
}

#endif