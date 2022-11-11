#pragma once

extern int e_main(Engine::Application* engine);
#define EF_QUICK_INIT
#ifdef EF_QUICK_INIT
bool quick_init = true;
#else
bool quick_init = false;
#endif

#ifdef _WIN32
int main(int argc, char** argv) {

	Engine::Application* engine = new Engine::Application(quick_init);
	e_main(engine);

	delete engine;
	return 0;
}
#endif