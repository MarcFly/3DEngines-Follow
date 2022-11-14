#pragma once

extern int e_main(std::shared_ptr<Engine::Application>& engine);
#define EF_QUICK_INIT
#ifdef EF_QUICK_INIT
bool quick_init = true;
#else
bool quick_init = false;
#endif

#ifdef _WIN32
int main(int argc, char** argv) {

	std::shared_ptr<Engine::Application> engine = std::make_shared<Engine::Application>(quick_init);
	e_main(engine);

	return 0;
}
#endif