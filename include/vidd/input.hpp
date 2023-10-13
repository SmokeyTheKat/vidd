#ifndef __VIDD_INPUT_HPP__
#define __VIDD_INPUT_HPP__

#include <string>
#include <string_view>
#include <utility>

class Input {
	bool mOwning;
	std::FILE* mFp;
	std::string mName;
	std::string mBackBuffer;
	std::size_t mLineLimit = 0;
	std::size_t mLinesRead = 0;

public:
	template <std::convertible_to<std::string_view> T>
	Input(const T& file)
	: mOwning(true),
	  mFp(std::fopen(std::string(file).c_str(), "rb")),
	  mName(file) {
		mBackBuffer.reserve(512);
	}

	Input(FILE* fp, const std::string& name)
	: mOwning(false), mFp(fp), mName(name) {}

	~Input(void) {
		if (mOwning && mFp != nullptr) {
			std::fclose(mFp);
		}
	}

	Input(Input&& other) {
		mFp = other.mFp;
		mOwning = other.mOwning;
		mName = std::move(other.mName);
		other.mOwning = false;
		other.mFp = nullptr;
	}

	Input(const Input&) = delete;
	Input& operator=(const Input&) = delete;

	const std::string& getName(void) { return mName; }

	Input& limitLines(std::size_t lineLimit) {
		mLineLimit = lineLimit;
		return *this;
	};

	std::size_t getLine(std::string& buffer);
};

#endif
