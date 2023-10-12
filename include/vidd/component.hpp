#ifndef __VIDD_DRAWABLE_HPP__
#define __VIDD_DRAWABLE_HPP__

#include <vidd/draw.hpp>
#include <vidd/style.hpp>
#include <vidd/pos.hpp>
#include <vidd/event.hpp>
#include <vidd/framebuffer.hpp>
#include <vidd/wchar.hpp>

#include <vector>
#include <set>
#include <string>
#include <string_view>
#include <functional>
#include <algorithm>
#include <sstream>

class Component;
class Display;

bool drawableCompareLayer(const Component* a, const Component* b);

class Component {
	Display* mDisplay = nullptr;
	Component* mParent = nullptr;
	std::vector<Component*> mChildren;

	int mLayer = 0;

	bool mRedrawNeeded = true;
	bool mSelectable = false;
	bool mPassiveSelectable = false;

	Vec2 mTextCursor;

protected:
	Vec2 mPos;
	Vec2 mSize;
//    FrameBuffer* mBuffer = nullptr;
	FrameBuffer mBuffer;

public:
	Component(Vec2 pos, Vec2 size);
	Component(void)
	: Component(Vec2::zero(), Vec2::zero()) {};
	Component(const Component& com) = delete;
	Component(Component&& com) = delete;
	virtual ~Component(void);

	Component& operator=(const Component& com) = delete;
	Component& operator=(Component&& com) = delete;

	virtual void render(void) {};
	virtual Vec2 getCursor(void) { return Vec2(0, 0); };
	virtual void onResize(void) {};
	virtual void onSelect(void) {};
	virtual void onDeselect(void) {};
	virtual void onMove(void) {};
	virtual void onKeyDown(Key key) {};
	virtual void onMouseMove(Vec2 pos) {};
	virtual void onLeftMouseButtonDown(Vec2 pos) {};
	virtual void onLeftMouseButtonDoubleDown(Vec2 pos) {};
	virtual void onLeftMouseButtonTripleDown(Vec2 pos) {};
	virtual void onLeftMouseButtonDrag(Vec2 pos) {};
	virtual void onLeftMouseButtonDoubleDrag(Vec2 pos) {};
	virtual void onLeftMouseButtonTripleDrag(Vec2 pos) {};
	virtual void onRightMouseButtonDown(Vec2 pos) {};
	virtual void onRightMouseButtonDrag(Vec2 pos) {};
	virtual void onMouseButtonUp(Vec2 pos) {};
	virtual void onPaste(WStringView data) {};
	virtual void onAttach(void) {};
	virtual void onPrerender(void) {};
	virtual void onPostRender(void) {};

	void setLayer(int layer) { mLayer = layer; if (mParent) mParent->sortChildren(); };
	int getLayer(void) const { return mLayer; };

	bool isSelected(void);

	Display* getDisplay(void) const { return mDisplay; };
	void setDisplay(Display* display) { mDisplay = display; };

	FrameBuffer* getRenderedData(void);

	bool isRedrawNeeded(void) { return mRedrawNeeded; };

	void requireSelfRedraw(void);
	void requireRedraw(void);
	void requireRedrawUp(void);

	void setPos(Vec2 pos) { mPos = pos; requireRedrawUp(); onMove(); };
	void setPosQuiet(Vec2 pos) { mPos = pos; };
	Vec2 getPos(void) { return mPos; };

	void setSize(Vec2 size) { setSizeQuiet(size); requireRedrawUp(); onResize(); };
	void setSizeQuiet(Vec2 size) { mSize = size; mBuffer.resize(size); };
	Vec2 getSize(void) { return mSize; };

	Vec2 getRealPos(Vec2 pos = Vec2::zero());
	Vec2 getPosFromReal(Vec2 pos);
	Vec2 getPosInOtherComponent(Vec2 pos, Component* other);

	bool isSelectable(void) { return mSelectable; };
	void setSelectable(bool v) { mSelectable = v; };

	bool isPassiveSelectable(void) { return mPassiveSelectable; };
	void setPassiveSelectable(bool v) { mPassiveSelectable = v; };

	bool isInBounds(Vec2 pos);

	void setParent(Component* com);
	Component* getParent(void) { return mParent; };
	void detachFromParent(void);

	void addChild(Component* com);
	void addChildQuiet(Component* com);
	void removeChild(Component* com);

	const std::vector<Component*>& getChildren(void) { return mChildren; };

	bool hasChild(Component* child) {
		return std::find(mChildren.begin(), mChildren.end(), child) != mChildren.end();
	};

	void removeAllChildren(void) {
		mChildren.clear();
	};

	bool forEachChild(std::function<bool(Component*)> callback);
	bool forEachChildByLayer(std::function<bool(Component*)> callback);

	void drawVLine(Vec2 pos, int length, WChar chr);
	void drawVLine(Vec2 pos, int length, Draw::Line line);

	void drawHLine(Vec2 pos, int length, WChar chr);
	void drawHLine(Vec2 pos, int length, Draw::Line line);

	void drawBox(Vec2 pos, Vec2 size, Draw::Line line);
	void drawBox(Vec2 pos, Vec2 size, WChar chr);

	void drawFilledBox(Vec2 pos, Vec2 size, WChar chr);

	void paintFormat(Vec2 pos, Vec2 size, Style format);

	void drawText(Vec2 pos, WStringView text);
	void drawText(WStringView text);
	void drawTextReverse(Vec2 pos, WStringView text);
	void drawTextReverse(WStringView text);

private:
	void sortChildren(void);
};

#endif
