#include <vidd/input.hpp>

std::size_t Input::getLine(std::string& buffer) {
	while (mLineLimit == 0 || mLinesRead < mLineLimit) {
		std::size_t lbPos = mBackBuffer.find('\n');
		if (lbPos == std::string::npos) {
			char buf[512];
			std::size_t len = std::fread(buf, 1, sizeof(buf), mFp);
			if (len == 0) {
				buffer.insert(
					buffer.end(),
					mBackBuffer.begin(),
					mBackBuffer.end()
				);
				std::size_t length = mBackBuffer.size();
				mBackBuffer.clear();
				return length;
			}
			mBackBuffer.insert(mBackBuffer.end(), buf, buf + len);
		} else {
			buffer.clear();
			buffer.insert(
				buffer.end(),
				mBackBuffer.begin(),
				mBackBuffer.begin() + lbPos
			);
			mBackBuffer.erase(0, lbPos + 1);
			mLinesRead += 1;
			return lbPos + 1;
		}
	}
	return 0;
}
