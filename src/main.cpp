#include <vidd/vidd.hpp>
#include <vidd/arguments.hpp>
#include <vidd/terminal.hpp>
#include <vidd/log.hpp>
#include <vidd/wchar.hpp>

#include <string_view>
#include <vector>
#include <string>
#include <unistd.h>
#include <signal.h>
#include <vidd/procstream.hpp>

using namespace std::literals;

int main(int argc, char** argv) {
	std::srand(std::time(0));
	std::setlocale(LC_ALL, "en_US.utf8");

	std::vector<char> buffer(1024*2*2);
	std::setvbuf(stdout, buffer.data(), _IONBF, buffer.size());

	Vidd app(Arguments(argc, argv));
	return app.run();
}
