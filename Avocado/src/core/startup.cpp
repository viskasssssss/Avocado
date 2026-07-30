#include "pch.h"

#ifdef AVO_PLATFORM_WINDOWS

#include "application.h"

using namespace avocado;

int main(int argc, char** argv)
{

	// startup is not final | changes will be made

	logger::init();

	std::filesystem::path cwd = std::filesystem::current_path();
	AVO_INFO("Working in {0}", cwd.string().c_str());
	AVO_INFO("Creating application");

	application app = application();
	app.run();
	
	return 0;
}

#endif