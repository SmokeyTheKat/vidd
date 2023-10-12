#ifndef __VIDD_TEXTPROMPT_HPP__
#define __VIDD_TEXTPROMPT_HPP__

#include <vidd/charsets.hpp>

#include <string>
#include <string_view>
#include <functional>
#include <vector>

class TextPrompt {
	struct CallbackList {
		std::function<void(void)> change = []{};
		std::function<void(void)> submit = []{};
		std::function<void(void)> exit = []{};
	};

	std::string mText;
	std::string mPrefix;
	CallbackList mCallbacks;
	std::size_t mCursor = 0;


public:
	TextPrompt(void) = default;

	TextPrompt(const CallbackList& callbacks)
	: mCallbacks(callbacks) {};

	TextPrompt(const std::string& prefix, const CallbackList& callbacks)
	: mPrefix(prefix), mCallbacks(callbacks) {};

	std::string& get(void) {
		return mText;
	};

	std::string getLine(void) {
		return mPrefix + mText;
	};

	std::size_t getCursor(void) {
		return mPrefix.length() + mCursor;
	};

	void submit(void) {
		mCallbacks.submit();
	}

	void exit(void) {
		mCallbacks.exit();
	}

	void input(Key key) {
		if (key == Keys::Return) {
			mCallbacks.submit();
		} else if (key == Keys::Escape || key == Keys::ctrl('c')) {
			mCallbacks.exit();
		} else if (key == Keys::Left) {
			if (mCursor > 0) {
				mCursor -= 1;
			}
		} else if (key == Keys::Right) {
			if (mCursor < mText.length()) {
				mCursor += 1;
			}
		} else if (key == Keys::Backspace) {
			if (mText.length() > 0 && mCursor > 0) {
				mText.erase(mText.begin() + mCursor - 1);
				mCursor -= 1;
				mCallbacks.change();
			} else {
				mCallbacks.exit();
			}
		} else if (CharSets::nonescape.contains(key)) {
			mText.insert(mText.begin() + mCursor, key);
			mCursor += 1;
			mCallbacks.change();
		}
	};
};

#endif
