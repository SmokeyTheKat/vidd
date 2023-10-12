#ifndef __VIDD_DISPLAY_HPP__
#define __VIDD_DISPLAY_HPP__

#include <vidd/pos.hpp>
#include <vidd/component.hpp>
#include <vidd/terminal.hpp>
#include <vidd/event.hpp>
#include <vidd/timer.hpp>

#include <vector>
#include <functional>
#include <utility>

class Display {
	using DelayData = std::tuple<double, std::function<void(void)>, std::size_t>;

	Component* mRoot = nullptr;
	Component* mSelected = nullptr;
	Component* mLatchedSelected = nullptr;

	bool mRunning = true;
	bool mRerenderRequired = true;

	Vec2 mCursorPos;

	std::vector<std::function<void(void)>> mDefers;
	std::vector<DelayData> mDelays;

	Timer mTimer;
	double mDelayTime = 0;

public:
	Display(void);

	void setRoot(Component* dwb);
	void setSelected(Component* dwb);
	void setSelectedFromRemoved(Component* dwb);

	double getDelayTime(void) const { return mDelayTime; };

	bool render(void);

	void quit(void);

	Component* getSelected(void) { return mSelected; };

	Component* getComponentAt(Vec2 pos);

	void defer(const std::function<void(void)>& callback);
	void delay(double length, const std::function<void(void)>& callback, std::size_t id = 0);
	void clearDelay(std::size_t id);

	void requireRerender(void) { mRerenderRequired = true; };

	void nextEvent(void);
	void handleEvent(Event e);

	void mainLoop(void);

private:
	void handleKeyEvent(KeyEvent ke);
	void handleMouseEvent(MouseEvent me);
	void handlePasteEvent(PasteEvent pe);

	bool canMouseEventSelect(MouseEvent me);
	bool canMouseEventLatch(MouseEvent me);

	void runDelays(void);

	Component* getMouseEventTarget(MouseEvent me);
};

#endif
