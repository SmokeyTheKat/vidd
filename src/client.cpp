#include <vidd/client.hpp>

#include <vidd/tab.hpp>
#include <vidd/tabarea.hpp>
#include <vidd/log.hpp>
#include <vidd/format.hpp>
#include <vidd/matcher.hpp>
#include <vidd/vidd.hpp>

Client::~Client(void) {
	if (getDisplay() != nullptr) {
		getDisplay()->clearDelay(std::size_t(this));
	}
}

void ClientFrame::onAttach(void) {
	addChild(mClient.get());
}

void ClientFrame::onResize(void) {
	if (mClient->isFloating()) {
		mClient->setSize((mSize - Vec2(2, 2)).max(0));
		mClient->setPos(Vec2(1, 1));
	} else {
		mClient->setSize(mSize);
		mClient->setPos(Vec2::zero());
	}
	mLateClientHasBorder = mClient->isFloating();
}

void ClientFrame::onPrerender(void) {
	if (mClient->isFloating() != mLateClientHasBorder) {
		onResize();
	}
}

void ClientFrame::render(void) {
	const Theme* theme = Vidd::getTheme();
	if (mClient->isFloating()) {
		Draw::style(theme->text);
		drawBox(Vec2::zero(), mSize, Draw::normalLine);
	}
}

void Client::quit(void) {
	mTab->closeClient(this);
}

void Client::setFloating(void) {
	mIsFloating = true;
	mTab->reorganize();
}

void Client::setNotFloating(void) {
	mIsFloating = false;
	mTab->reorganize();
}

void Client::toggleFloating(void) {
	if (mIsFloating) {
		setNotFloating();
	} else {
		setFloating();
	}
}

void Client::windowMoveX(int dx) {
	Vec2 tabSize = getParent()->getParent()->getSize();
	Vec2 mySize = getParent()->getSize();
	Vec2 myPos = getParent()->getPos();
	Vec2 pos = myPos + Vec2(dx, 0);
	pos.x = std::clamp(pos.x, 0, tabSize.x - mySize.x);
	getParent()->setPos(pos);
}

void Client::windowMoveY(int dy) {
	Vec2 tabSize = getParent()->getParent()->getSize();
	Vec2 mySize = getParent()->getSize();
	Vec2 myPos = getParent()->getPos();
	Vec2 pos = myPos + Vec2(0, dy);
	pos.y = std::clamp(pos.y, 0, tabSize.y - mySize.y);
	getParent()->setPos(pos);
}

void Client::windowResizeX(int dx) {
	Vec2 tabSize = getParent()->getParent()->getSize();
	Vec2 mySize = getParent()->getSize();
	Vec2 myPos = getParent()->getPos();
	Vec2 size = mySize + Vec2(dx, 0);
	size.x = std::clamp(size.x, 6, tabSize.x - myPos.x);
	getParent()->setSize(size);
}

void Client::windowResizeY(int dy) {
	Vec2 tabSize = getParent()->getParent()->getSize();
	Vec2 mySize = getParent()->getSize();
	Vec2 myPos = getParent()->getPos();
	Vec2 size = mySize + Vec2(0, dy);
	size.y = std::clamp(size.y, 3, tabSize.y - myPos.y);
	getParent()->setSize(size);
}

TabArea* Client::getTabArea(void) const {
	return mTab->getTabArea();
}

Tab* Client::getTab(void) const {
	return mTab;
}

std::pair<bool, bool> Client::interpretBindSet(const KeyBinds& bindSet) {
	std::vector<Key> keys(mKeyBuffer.begin(), mKeyBuffer.end());
	auto res = bindSet.get(keys);
	if (res.type == MatcherResult::Exact && res.position == MatcherPosition::End) {
		mKeyBuffer.clear();
		res.value(this);
		return {true, false};
	} else if (res.type == MatcherResult::Closest) {
		Terminal::stageEvent(Event(EventType::Key, KeyEvent(mKeyBuffer.back())));
		mKeyBuffer.clear();
		res.value(this);
		return {true, true};
	} else if (res.type == MatcherResult::Invalid && res.position == MatcherPosition::End) {
		return {false, false};
	}
	return {true, false};
}

bool Client::interpretAliasSet(const AliasBinds& aliasSet) {
	std::vector<Key> keys(mKeyBuffer.begin(), mKeyBuffer.end());
	auto res = aliasSet.get(keys);
	if (res.type == MatcherResult::Exact && res.position == MatcherPosition::End) {
		mKeyBuffer.clear();
		for (auto k : res.value) {
			interpret(k);
		}
		return true;
	} else if (res.type == MatcherResult::Invalid && res.position == MatcherPosition::End) {
		return false;
	}
	return true;
}

bool Client::interpret(Key key) {
	mKeyBuffer.push_back(key);
	bool staged = false;
	if (!interpretAliasSet(*mAliases)) {
		bool ran;
		if ((std::tie(ran, staged) = interpretBindSet(*mKeyBinds), !ran)) {
			mKeyBuffer.clear();
			unhandledKey(key);
			return staged;
		}
	}
	return staged;
}

void Client::close(void) {
	mTab->closeClient(this);
}
