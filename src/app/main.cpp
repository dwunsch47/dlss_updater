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
#if _DEBUG
	std::cout << "Began app" << std::endl;

	if (string(argv[1]) == "test") {
		std::cout << "Running tests" << std::endl;
		TestPathStorage();
		return 1;
	}
#endif

	unique_ptr<Application> app = make_unique<Application>(argc, argv);
}