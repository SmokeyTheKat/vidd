#ifndef __VIDD_WSTRING_IPP__
#define __VIDD_WSTRING_IPP__

constexpr inline WString::WString(void) {};

template<std::convertible_to<std::string_view> T>
constexpr inline WString::WString(T&& svLike) {
	std::string_view str = svLike;

	mData.reserve(str.length());

	for (std::size_t i = 0; i < str.length();) {
		WChar chr = &str[i];
		mData.push_back(chr);
		i += chr.length();
	}
}

constexpr inline WString::WString(WStringView view)
: mData(view.begin(), view.end()) {
}

constexpr inline WString::WString(std::size_t n, WChar chr) {
	mData.resize(n, chr);
}

constexpr inline WString::WString(const WString& other)
: mData(other.mData) {};

constexpr inline WString::WString(WString&& other)
: mData(std::move(other.mData)) {};

inline WString& WString::operator=(const WString& other) {
	mData = other.mData;
	return *this;
}

inline WString& WString::operator=(WString&& other) {
	mData = std::move(other.mData);
	return *this;
}

constexpr inline const WChar* WString::data(void) const {
	return mData.data();
}

constexpr inline std::size_t WString::length(void) const {
	return mData.size();
}

constexpr inline std::string WString::string(void) const {
	std::string out;
	out.reserve(mData.size());
	for (const WChar& c : mData) {
		std::size_t clen = CString::nLength(c.bytes, 4);
		out.append(reinterpret_cast<const char*>(c.bytes), clen);
	}
	return out;
}

inline void WString::pushBack(const WChar& chr) {
	mData.push_back(chr);
}

inline void WString::popBack(void) {
	mData.pop_back();
}

template <class T>
inline void WString::append(const T& str) {
	mData.insert(mData.end(), str.begin(), str.end());
}

template <class IT>
inline void WString::insert(WString::iterator at, IT from, IT to) {
	mData.insert(at, from, to);
}

inline void WString::insert(std::size_t at, WChar chr) {
	mData.insert(mData.begin() + at, chr);
}

inline void WString::insert(std::size_t at, WChar chr, std::size_t n) {
	for (std::size_t i = 0; i < n; i++) {
		mData.insert(mData.begin() + at, chr);
	}
}

inline void WString::remove(WString::iterator begin, WString::iterator end) {
	mData.erase(begin, end);
}

inline void WString::remove(std::size_t begin, std::size_t end) {
	mData.erase(mData.begin() + begin, mData.begin() + end);
}

inline void WString::removeAt(std::size_t at) {
	mData.erase(mData.begin() + at);
}

inline void WString::removeNAt(std::size_t at, std::size_t n) {
	mData.erase(mData.begin() + at, mData.begin() + at + n);
}

inline void WString::removeAfter(std::size_t from) {
	mData.erase(mData.begin() + from, mData.end());
}

inline void WString::clear(void) {
	mData.clear();
}

inline std::size_t WString::findNextIndex(WChar chr, std::size_t start) const {
	return std::find(mData.begin() + start + 1, mData.end(), chr) - mData.begin();
}

inline std::size_t WString::findPrevIndex(WChar chr, std::size_t start) const {
	return mData.size() - (std::find(mData.rbegin() + (mData.size() - start), mData.rend(), chr) - mData.rbegin()) - 1;
}

inline bool WString::startsWith(WStringView with) const {
	if (with.length() > mData.size()) return false;
	for (std::size_t i = 0; i < with.length(); i++) {
		if (mData[i] != with[i]) {
			return false;
		}
	}
	return true;
}

constexpr inline WChar& WString::operator[](std::size_t idx) {
	return mData[idx];
}

constexpr inline const WChar& WString::operator[](std::size_t idx) const {
	return mData[idx];
}

constexpr inline WString::const_iterator WString::begin(void) const {
	return mData.cbegin();
}

constexpr inline WString::const_iterator WString::end(void) const {
	return mData.cend();
}

constexpr inline WString::iterator WString::begin(void) {
	return mData.begin();
}

constexpr inline WString::iterator WString::end(void) {
	return mData.end();
}

inline WString operator+(const WString& lhs, const WString& rhs) {
	WString res(lhs);
	res.append(rhs);
	return res;
}

inline WString operator+(const WString& lhs, const char* rhs) {
	WString res(rhs);
	res.insert(res.begin(), lhs.begin(), lhs.end());
	return res;
}

inline WString operator+(const char* lhs, const WString& rhs) {
	WString res(lhs);
	res.insert(res.end(), rhs.begin(), rhs.end());
	return res;
}

inline WString operator+(const WString& lhs, WChar rhs) {
	WString res(lhs);
	res.pushBack(rhs);
	return res;
}

inline WString operator+(WChar lhs, const WString& rhs) {
	WString res(rhs);
	res.insert(0, lhs);
	return res;
}

#endif
