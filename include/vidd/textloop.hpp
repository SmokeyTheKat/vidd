#ifndef __VIDD_TEXTLOOP_HPP__
#define __VIDD_TEXTLOOP_HPP__

#include <vidd/component.hpp>
#include <vidd/textprompt.hpp>
#include <vidd/wchar.hpp>

#include <string>
#include <string_view>
#include <functional>
#include <vector>

class LoopText : public Component {
	WString mText;
	int mPos;

public:
	LoopText(WString text, int size);

	void onPrerender(void) override;
	void render(void) override;
};


#endif
