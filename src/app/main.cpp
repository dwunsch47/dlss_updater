#include "application.h"

#include <iostream>
#include <vector>
#include <memory>

#if _DEBUG
#include "../tests/tests.h"
#endif

int main(int argc, char* argv[]) {
#if _DEBUG
	std::cout << "Began app" << std::endl;

	if (std::string(argv[1]) == "test") {
		std::cout << "Running tests" << std::endl;
		TestPathStorage();
		return 1;
	}
#endif

	std::unique_ptr<Application> app = std::make_unique<Application>(argc, argv);
}