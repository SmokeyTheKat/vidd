#include <vidd/display.hpp>

#include <vidd/style.hpp>
#include <vidd/log.hpp>
#include <vidd/timer.hpp>

Display::Display(void) {
	mTimer.start();
}

void Display::setRoot(Component* com) {
	mRoot = com;
	if (com) {
		com->setDisplay(this);
		com->onAttach();
	}
}

void Display::setSelected(Component* com) {
	if (mSelected != com) {
		if (mSelected) {
			mSelected->onDeselect();
		}
		mSelected = com;
		if (com) {
			com->onSelect();
		}
	}
}

void Display::setSelectedFromRemoved(Component* com) {
	if (mSelected != com) {
		mSelected = com;
		if (com) {
			com->onSelect();
		}
	}
}

Component* Display::getComponentAt(Vec2 pos) {
	Component* found = nullptr;
	mRoot->forEachChildByLayer([pos, &found](Component* com) {
		if ((com->isSelectable() || com->isPassiveSelectable()) && com->isInBounds(com->getPosFromReal(pos))) {
			found = com;
			return false;
		}
		return true;
	});
	return found;
}

bool Display::canMouseEventSelect(MouseEvent me) {
	return me.type == MouseEventType::Move ||
		   me.type == MouseEventType::LeftButtonDown ||
		   me.type == MouseEventType::RightButtonDown;
}

bool Display::canMouseEventLatch(MouseEvent me) {
	return me.type == MouseEventType::RightButtonDown ||
		   me.type == MouseEventType::LeftButtonDown;
}

Component* Display::getMouseEventTarget(MouseEvent me) {
	Component* target = mSelected;

	if (mLatchedSelected) {
		target = mLatchedSelected;
		if (me.type == MouseEventType::ButtonUp) {
			mLatchedSelected = nullptr;
		}
	} else if (canMouseEventSelect(me)) {
		if (Component* found = getComponentAt(me.pos)) {
			if (found->isSelectable() && getSelected() != found) {
				setSelected(found);
			}

			target = found;
			if (canMouseEventLatch(me)) {
				mLatchedSelected = target;
			}
		}
	}

	return target;
}

void Display::handleMouseEvent(MouseEvent me) {
	Component* target = getMouseEventTarget(me);

	if (!target) return;

	target->mCtrl = me.ctrl;

	switch (me.type) {
	case MouseEventType::Move: {
		target->onMouseMove(target->getPosFromReal(me.pos));
	} break;
	case MouseEventType::LeftButtonDown: {
		target->onLeftMouseButtonDown(target->getPosFromReal(me.pos));
	} break;
	case MouseEventType::LeftButtonDoubleDown: {
		target->onLeftMouseButtonDoubleDown(target->getPosFromReal(me.pos));
	} break;
	case MouseEventType::LeftButtonTripleDown: {
		target->onLeftMouseButtonTripleDown(target->getPosFromReal(me.pos));
	} break;
	case MouseEventType::LeftButtonDrag: {
		target->onLeftMouseButtonDrag(target->getPosFromReal(me.pos));
	} break;
	case MouseEventType::LeftButtonDoubleDrag: {
		target->onLeftMouseButtonDoubleDrag(target->getPosFromReal(me.pos));
	} break;
	case MouseEventType::LeftButtonTripleDrag: {
		target->onLeftMouseButtonTripleDrag(target->getPosFromReal(me.pos));
	} break;
	case MouseEventType::RightButtonDown: {
		target->onRightMouseButtonDown(target->getPosFromReal(me.pos));
	} break;
	case MouseEventType::RightButtonDrag: {
		target->onRightMouseButtonDrag(target->getPosFromReal(me.pos));
	} break;
	case MouseEventType::ButtonUp: {
		target->onMouseButtonUp(target->getPosFromReal(me.pos));
	} break;
	default: break;
	}
}

void Display::handleKeyEvent(KeyEvent ke) {
	if (!mSelected) return;
	mSelected->onKeyDown(ke.key);
}

void Display::handlePasteEvent(PasteEvent pe) {
	if (!mSelected) return;

	switch (pe.type) {
	case PasteEventType::Start: {
		WString data = Terminal::getPasteData();
		mSelected->onPaste(data);
	} break;
	default: break;
	}
}

void Display::handleEvent(Event e) {
	switch (e.type) {
	case EventType::Key: {
		handleKeyEvent(std::get<KeyEvent>(e.event));
	} break;
	case EventType::Mouse: {
		handleMouseEvent(std::get<MouseEvent>(e.event));
	} break;
	case EventType::Paste: {
		handlePasteEvent(std::get<PasteEvent>(e.event));
	} break;
	case EventType::ForceQuit: {
		quit();
	} break;
	default: break;
	}
}

void Display::nextEvent(void) {
	handleEvent(Terminal::getEvent());
}

void Display::mainLoop(void) {
	static Vec2 lateSize = mRoot->getSize();

	while (mRunning) {
		if (Vec2 newSize = Terminal::getSize(); newSize != lateSize) {
			Terminal::setSize(newSize);
			mRoot->setSize(newSize);
			mRoot->requireRedraw();
			lateSize = newSize;
		}

		if (mSelected) {
			Vec2 newCursorPos = mSelected->getCursor();
			if (newCursorPos != mCursorPos) {
				mCursorPos = newCursorPos;
				Terminal::Cursor::moveTo(newCursorPos);
			}
		}

		Timer renderTime;
		renderTime.start();
		render();
		mDelayTime = renderTime.stop();

		nextEvent();

		for (const auto& callback : mDefers) {
			callback();
		}
		runDelays();
		mDefers.clear();
	}
}

void Display::defer(const std::function<void(void)>& callback) {
	mDefers.push_back(callback);
}

void Display::delay(double length, const std::function<void(void)>& callback, std::size_t id) {
	mDelays.push_back({ mTimer.now() + length, callback, id });
}

void Display::clearDelay(std::size_t id) {
	if (mDelays.size() == 0) return;

	mDelays.erase(std::remove_if(
		mDelays.begin(),
		mDelays.end(),
		[id](const DelayData& delay) { return std::get<2>(delay) == id; }
	));
}

void Display::runDelays(void) {
	bool toRemove = false;
	double now = mTimer.now();
	for (const DelayData& delay : mDelays) {
		if (now > std::get<0>(delay)) {
			std::get<1>(delay)();
			toRemove = true;
		}
	}

	if (toRemove == false) return;

	mDelays.erase(std::remove_if(
		mDelays.begin(),
		mDelays.end(),
		[now](const DelayData& delay) { return now > std::get<0>(delay); }
	));
}

void Display::quit(void) {
	mRunning = false;
}

bool Display::render(void) {
	mRoot->forEachChild([](Component* com) {
		com->onPrerender();
		return true;
	});

	if (!mRerenderRequired) return false;

	FrameBuffer* fb = Terminal::getFrameBuffer();
	mRoot->forEachChild([fb](Component* com) {
		fb->merge(*com->getRenderedData(), com->getRealPos());
		return true;
	});

	mRoot->forEachChild([](Component* com) {
		com->onPostRender();
		return true;
	});

	mRerenderRequired = false;

	return Terminal::flush();
}

