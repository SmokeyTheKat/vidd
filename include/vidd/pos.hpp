#ifndef __VIDD_POS_HPP__
#define __VIDD_POS_HPP__

#include <concepts>

struct Vec2 {
	int x = 0;
	int y = 0;

	Vec2(void)
	: Vec2(0, 0) {};
	Vec2(int x, int y)
	: x(x), y(y) {};

	Vec2 max(int val) {
		Vec2 out = *this;
		if (out.x < val) out.x = val;
		if (out.y < val) out.y = val;
		return out;
	}

	bool operator==(const Vec2& other) const {
		return x == other.x && y == other.y;
	}

	bool operator!=(const Vec2& other) const {
		return !(*this == other);
	}

	Vec2 operator+(const Vec2& other) const {
		return Vec2(other.x + x, other.y + y);
	}

	Vec2 operator+(int val) const {
		return Vec2(x + val, y + val);
	}

	void operator+=(const Vec2& other) {
		*this = *this + other;
	}

	void operator+=(int val) {
		*this = *this + val;
	}

	Vec2 operator-(const Vec2& other) const {
		return Vec2(x - other.x, y - other.y);
	}

	Vec2 operator-(int val) const {
		return Vec2(x - val, y - val);
	}

	void operator-=(const Vec2& other) {
		*this = *this - other;
	}

	void operator-=(int val) {
		*this = *this - val;
	}

	Vec2 operator*(const Vec2& other) const {
		return Vec2(x * other.x, y * other.y);
	}

	template<std::convertible_to<int> T>
	Vec2 operator*(T val) const {
		return Vec2(x * val, y * val);
	}

	void operator*=(const Vec2& other) {
		*this = *this * other;
	}

	template<std::convertible_to<int> T>
	void operator*=(T val) const {
		*this = *this * val;
	}


	Vec2 operator/(const Vec2& other) const {
		return Vec2(x / other.x, y / other.y);
	}

	template<std::convertible_to<int> T>
	Vec2 operator/(T val) const {
		return Vec2(x / val, y / val);
	}

	void operator/=(const Vec2& other) {
		*this = *this / other;
	}

	template<std::convertible_to<int> T>
	void operator/=(T val) const {
		*this = *this / val;
	}

	bool operator>(int value) const {
		return x > value && y > value;
	}

	bool operator>(const Vec2& other) const {
		return x > other.x && y > other.y;
	}

	bool operator>=(int value) const {
		return x >= value && y >= value;
	}

	bool operator>=(const Vec2& other) const {
		return x >= other.x && y >= other.y;
	}

	bool operator<(int value) const {
		return x < value && y < value;
	}

	bool operator<(const Vec2& other) const {
		return x < other.x && y < other.y;
	}

	bool operator<=(int value) const {
		return x <= value && y <= value;
	}

	bool operator<=(const Vec2& other) const {
		return x <= other.x && y <= other.y;
	}

	static Vec2 zero(void) { return Vec2(0, 0); };
	static Vec2 ones(void) { return Vec2(1, 1); };
};

#endif
