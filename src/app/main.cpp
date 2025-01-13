#include "application.h"

#include <iostream>
#include <filesystem>
#include <vector>
#include <memory>

#if _DEBUG
#include "../tests/tests.h"
#endif

using namespace std;

int main(int argc, char* argv[]) {
	const std::string mode = argv[1];
	std::vector<std::filesystem::path> arguments;

	if (argc > 2) {
		for (int i = 2; i < argc; ++i) {
			arguments.push_back(argv[i]);
		}
	}

#if _DEBUG
	std::cout << "Began app" << std::endl;

	if (mode == "test") {
		std::cout << "Running tests" << std::endl;
		TestPathStorage();
		return 1;
	}
#endif

	unique_ptr<Application> app = make_unique<Application>(argc, argv);
}