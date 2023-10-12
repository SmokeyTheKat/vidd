#ifndef __VIDD_SCROLLBAR_HPP__
#define __VIDD_SCROLLBAR_HPP__

#include <vidd/component.hpp>

#include <functional>
#include <utility>

class ScrollBar : public Component {
public:
	struct SizeDetails {
		double pos;
		double size;

		bool operator==(const SizeDetails& other) const {
			return pos == other.pos && size == other.size;
		};
		bool operator!=(const SizeDetails& other) const {
			return !(*this == other);
		};
	};

	enum class Side {
		Left,
		Right,
	};

private:
	using CalculateSizeCallback = std::function<SizeDetails(void)>;
	using OnChangeCallback = std::function<void(float)>;

	Side mSide;
	CalculateSizeCallback mCalculateSize;
	OnChangeCallback mOnChange;
	SizeDetails mDetails;

public:
	ScrollBar(Side side, CalculateSizeCallback cs, OnChangeCallback oc);

	void onPrerender(void) override;
	void render(void) override;

private:
	std::pair<int, int> getScrollBarPosAndSize(void);
};

#endif
