#ifndef __VIDD_WSTRINGVIEW_IPP__
#define __VIDD_WSTRINGVIEW_IPP__

constexpr inline WStringView::WStringView(void)
: mData(nullptr), mLength(0) {};

constexpr inline WStringView::WStringView(const WStringView& string)
: mData(string.mData), mLength(string.length()) {};

constexpr inline WStringView::WStringView(const WString& string)
: mData(string.data()), mLength(string.length()) {};

template <class IT>
constexpr inline WStringView::WStringView(IT begin, IT end)
: mData(std::to_address(begin)), mLength(end - begin) {};

inline WStringView& WStringView::operator=(const WStringView& other) {
	mData = other.mData;
	mLength = other.mLength;
	return *this;
}

constexpr inline std::size_t WStringView::length(void) const {
	return mLength;
};

constexpr inline const WChar* WStringView::data(void) const {
	return mData;
};

constexpr inline const WChar& WStringView::at(std::size_t idx) const {
	return mData[idx];
};

constexpr inline const WChar& WStringView::operator[](std::size_t idx) const {
	return at(idx);
};

constexpr inline const WChar& WStringView::front(void) const {
	return mData[0];
};

constexpr inline const WChar& WStringView::back(void) const {
	return mData[mLength - 1];
};

constexpr inline void WStringView::removePrefix(std::size_t count) {
	mData += count;
	mLength -= count;
}

constexpr inline void WStringView::removeSuffix(std::size_t count) {
	mLength -= count;
}

inline std::size_t WStringView::findSubString(WStringView find, std::size_t start) const {
	for (std::size_t i = start; i + find.length() - 1 < mLength; i++) {
		if (subString(i, i + find.length()) == find) {
			return i;
		}
	}
	return WStringView::npos;
};

inline std::size_t WStringView::findSubStringRtl(WStringView find, std::size_t start) const {
	if (start == WStringView::npos) {
		start = mLength;
	}
	start = std::min(mLength - find.length() + 1, start);
	for (std::size_t i = start; i-- > 0;) {
		if (subString(i, i + find.length()) == find) {
			return i;
		}
	}
	return WStringView::npos;
};

constexpr bool WStringView::startsWith(WStringView with) const {
	if (with.length() > mLength) return false;
	for (std::size_t i = 0; i < with.length(); i++) {
		if (mData[i] != with[i]) {
			return false;
		}
	}
	return true;
}

inline const WStringView WStringView::subString(std::size_t cstart, std::size_t cend) const {
	return WStringView(begin() + cstart, begin() + std::min(cend, mLength));
};

constexpr inline const WChar* WStringView::begin(void) const {
	return mData;
};

constexpr inline const WChar* WStringView::end(void) const {
	return mData + mLength;
};

inline bool WStringView::operator==(std::string_view other) const {
	if (mLength != other.length()) return false;
	for (std::size_t i = 0; i < mLength; i++) {
		if (mData[i] != other[i]) return false;
	}
	return true;
};

inline bool WStringView::operator==(WStringView other) const {
	if (mLength != other.length()) return false;
	for (std::size_t i = 0; i < mLength; i++) {
		if (mData[i] != other[i]) return false;
	}
	return true;
};

#endif
