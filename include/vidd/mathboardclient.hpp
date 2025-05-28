#ifndef __VIDD_MATH_BOARD_CLIENT_HPP__
#define __VIDD_MATH_BOARD_CLIENT_HPP__

#include <vidd/client.hpp>

class MathBoardClient : public Client {
	enum class InputType {
		None,
		Function,
		Expression,
		Variable,
	};
	struct Input {
		InputType type;
		std::string text;
	};

	std::vector<Input> mInputs;

public:
	MathBoardClient(Tab* tab);

	void render(void) override;
};

#endif
