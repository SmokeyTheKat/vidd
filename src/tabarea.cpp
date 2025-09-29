#include <vidd/tabarea.hpp>

#include <vidd/tab.hpp>
#include <vidd/vidd.hpp>
#include <vidd/display.hpp>
#include <vidd/texteditorclient.hpp>
#include <vidd/utils.hpp>
#include <vidd/format.hpp>
#include <vidd/log.hpp>

#include <cmath>

TabArea::TabArea(void)
: Component(Vec2(0, 0), Terminal::getSize()) {
	for (std::size_t i = 0; i < 10; i++) {
		mTabs.push_back(std::make_unique<Tab>(this));
	}
	resizeTabs();
	setPassiveSelectable(true);
}

int TabArea::getTabId(Tab* tab) {
	for (std::size_t i = 0; i < mTabs.size(); i++) {
		if (mTabs[i].get() == tab) return i;
	}
	return 0;
}

void TabArea::setNextActive(void) {
	for (int i = mActiveTab + 1; i < mTabs.size(); i++) {    
		if (mTabs[i]->clientCount() > 0) {
			setActive(i);
			break;
		}
	}
}

void TabArea::setPrevActive(void) {
	for (int i = mActiveTab - 1; i >= 0; i--) {    
		if (mTabs[i]->clientCount() > 0) {
			setActive(i);
			break;
		}
	}
}

void TabArea::setLastActive(void) {
	setActive(mLastTab);
}

void TabArea::setActive(Tab* tab) {
	setActive(getTabId(tab));
}

void TabArea::setActive(int idx) {
	if (idx < 0 || idx >= tabCount()) return;
	mLastTab = mActiveTab;
	mActiveTab = idx;
	setChild();
	Tab* tab = getActive();
	if (tab->clientCount() == 0) {
		tab->addAndSelectClient<TextEditorClient>(Input(PREFIX "/share/vidd/default"));
	}
	getActive()->activate();
	requireRedraw();
}

void TabArea::setChild(void) {
	removeAllChildren();
	addChild(getActive());
}

std::vector<Client*> TabArea::getAllClients(void) {
	std::vector<Client*> clients;
	for (auto& tab : mTabs) {
		std::vector<Client*> tabClients = tab->getClients();
		clients.insert(clients.end(), tabClients.begin(), tabClients.end());
	}
	return clients;
}

void TabArea::resizeTabs(void) const {
	Vec2 pos;
	Vec2 size;
	if (mShowTabBar) {
		pos = Vec2(0, 1);
		size = mSize - Vec2(0, 1);
	} else {
		pos = Vec2(0, 0);
		size = mSize;
	}

	for (const auto& tab : mTabs) {
		tab->setPos(pos);
		tab->setSize(size);
	}
}

void TabArea::openNextTabWithClients(void) {
	int i = 0;
	for (auto& tab : mTabs) {
		if (tab->clientCount() > 0) {
			setActive(i);
			return;
		}
		i++;
	}

	getDisplay()->quit();
}

void TabArea::onResize(void) {
	resizeTabs();
}

void TabArea::onAttach(void) {
	if (Vidd::hasFlag("-htb")) {
		showTabBar(false);
	}
	setChild();
}

void TabArea::onLeftMouseButtonDown(Vec2 pos) {
	if (pos.y != 0) return;
	int i = pos.x / mTabWidth;
	if (i >= tabCount()) return;
	setActive(i);
	requireRedraw();
}

void TabArea::onPrerender(void) {
	requireSelfRedraw();
}

void TabArea::render(void) {
	if (mShowTabBar == false) return;

	const Theme* theme = Vidd::getTheme();

	Style activeStyle = theme->topBarActive;
	Style inactiveStyle = theme->topBarInactive;

	Vec2 pos = Vec2::zero();

	Draw::style(activeStyle);
	drawFilledBox(pos, Vec2(mSize.x, 1), " ");

	Draw::style(activeStyle);
	drawTextReverse(Vec2(mSize.x - 1, 0), WStringView(Format::format(
		"[{}]", Utils::getTimeString()
	)));
	drawTextReverse(WStringView(Format::format(
		"[{}]", Utils::getPwd()
	)));

	Draw::style(inactiveStyle);
	int i = 0;
	for (const auto& tab : mTabs) {
		if (i == mActiveTab) {
			Draw::style(activeStyle);
		} else {
			Draw::style(inactiveStyle);
		}

		const char* status = " ";
		if (tab->clientCount() > 0) {
			status = "▘";
		}
		drawText(pos, WString(Format::format("{}{} ", status, (i + 1) % 10)));
		pos.x += mTabWidth;
		i++;
	}

	Draw::style(inactiveStyle);
	drawText(pos, WString(Format::format(" [{}] ", getActive()->clientCount())));

	Draw::style(activeStyle);
	drawText(" "_ws);
	drawText(getActive()->getSelected()->getTitle());
	drawText("  "_ws);
}
