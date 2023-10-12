#include <vidd/component.hpp>

#include <vidd/draw.hpp>
#include <vidd/display.hpp>
#include <vidd/terminal.hpp>
#include <vidd/utils.hpp>

#include <cstdio>
#include <queue>
#include <algorithm>

Component::~Component(void) {
	detachFromParent();
	if (mDisplay && this == mDisplay->getSelected()) {
		mDisplay->setSelected(nullptr);
	}
}

Component::Component(Vec2 pos, Vec2 size)
: mPos(pos), mSize(size), mBuffer(size) {
}

FrameBuffer* Component::getRenderedData(void) {
	if (mRedrawNeeded == true) {
		render();
	}
	mRedrawNeeded = false;
	return &mBuffer;
};

Vec2 Component::getRealPos(Vec2 pos) {
	Vec2 realPos = mPos + pos;
	for (Component* com = mParent; com != nullptr; com = com->getParent()) {
		realPos += com->getPos();
	}
	return realPos;
}

Vec2 Component::getPosFromReal(Vec2 pos) {
	return pos - getRealPos();
}

Vec2 Component::getPosInOtherComponent(Vec2 pos, Component* other) {
	return other->getPosFromReal(getRealPos(pos));
}

bool Component::isInBounds(Vec2 pos) {
	return pos >= 0 && pos < mSize;
}

bool Component::isSelected(void) {
	return mDisplay->getSelected() == this;
}

void Component::sortChildren(void) {
	std::sort(
		mChildren.begin(),
		mChildren.end(),
		[](const Component* a, const Component* b) {
			return a->getLayer() > b->getLayer();
		}
	);
}

void Component::addChildQuiet(Component* com) {
	if (hasChild(com)) return;

	mChildren.push_back(com);
	com->setParent(this);
	if (mDisplay) {
		com->setDisplay(mDisplay);
	} else if (com->getDisplay()) {
		setDisplay(com->getDisplay());
	}

	sortChildren();
}

void Component::addChild(Component* com) {
	addChildQuiet(com);
	com->onAttach();
}

void Component::detachFromParent(void) {
	if (mParent) {
		mParent->removeChild(this);
	}
}

void Component::removeChild(Component* com) {
	auto it = std::find(mChildren.begin(), mChildren.end(), com);
	if (it == mChildren.end()) return;

	mChildren.erase(it);

	com->setParent(nullptr);
}

void Component::setParent(Component* com) {
	mParent = com;
	if (com != nullptr && com->hasChild(this) == false) {
		com->addChild(this);
	}
}

void Component::requireSelfRedraw(void) {
	if (mDisplay) mDisplay->requireRerender();
	mRedrawNeeded = true;
};

void Component::requireRedraw(void) {
	forEachChild([](Component* com) {
		com->requireSelfRedraw();
		return true;
	});
}

void Component::requireRedrawUp(void) {
	if (mParent) {
		mParent->requireRedraw();
	} else {
		requireRedraw();
	}
}

bool Component::forEachChild(std::function<bool(Component*)> callback) {
	if (callback(this) == false) return false;

	for (auto it = mChildren.rbegin(); it != mChildren.rend(); it++) {
		Component* com = *it;
		if (com->forEachChild(callback) == false) return false;
	}

	return true;
}

bool Component::forEachChildByLayer(std::function<bool(Component*)> callback) {
	for (auto com : mChildren) {
		if (com->forEachChildByLayer(callback) == false) return false;
	}

	if (callback(this) == false) return false;

	return true;
}

void Component::drawVLine(Vec2 pos, int length, WChar chr) {
	Draw::vLine(pos, length, chr, &mBuffer);
};

void Component::drawVLine(Vec2 pos, int length, Draw::Line line) {
	Draw::vLine(pos, length, line, &mBuffer);
};

void Component::drawHLine(Vec2 pos, int length, WChar chr) {
	Draw::hLine(pos, length, chr, &mBuffer);
};

void Component::drawHLine(Vec2 pos, int length, Draw::Line line) {
	Draw::hLine(pos, length, line, &mBuffer);
}

void Component::drawBox(Vec2 pos, Vec2 size, Draw::Line line) {
	Draw::box(pos, size, line, &mBuffer);
}

void Component::drawBox(Vec2 pos, Vec2 size, WChar chr) {
	Draw::box(pos, size, chr, &mBuffer);
}

void Component::drawFilledBox(Vec2 pos, Vec2 size, WChar chr) {
	Draw::filledBox(pos, size, chr, &mBuffer);
}

void Component::paintFormat(Vec2 pos, Vec2 size, Style format) {
	Draw::paintFormat(pos, size, format, &mBuffer);
}

void Component::drawText(Vec2 pos, WStringView text) {
	Draw::text(pos, text, &mBuffer);
	mTextCursor = pos + Vec2(text.length(), 0);
}

void Component::drawText(WStringView text) {
	int length = text.length();
	Draw::text(mTextCursor, text, &mBuffer);
	mTextCursor += Vec2(length, 0);
}

void Component::drawTextReverse(Vec2 pos, WStringView text) {
	mTextCursor = pos - Vec2(text.length() - 1, 0);
	Draw::text(mTextCursor, text, &mBuffer);
	mTextCursor -= Vec2(1, 0);
}

void Component::drawTextReverse(WStringView text) {
	mTextCursor -= Vec2(text.length() - 1, 0);
	Draw::text(mTextCursor, text, &mBuffer);
	mTextCursor -= Vec2(1, 0);
}
