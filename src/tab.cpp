#include <vidd/tab.hpp>

#include <vidd/tabarea.hpp>
#include <vidd/texteditorclient.hpp>
#include <vidd/display.hpp>
#include <vidd/log.hpp>
#include <vidd/client.hpp>
#include <vidd/vidd.hpp>

#include <algorithm>

Tab::Tab(TabArea* tabArea)
: Component(Vec2(0, 0), Vec2(1, 1)), mTabArea(tabArea) {
	setPassiveSelectable(true);
	mCurrentLayout = LayoutType::Tile;
}

int Tab::getHeight(void) const {
	if (mShowTabBar) {
		return mSize.y - 1;
	} else {
		return mSize.y;
	}
}

int Tab::getYOffset(void) const {
	if (mShowTabBar) {
		return 1;
	} else {
		return 0;
	}
}

TabArea* Tab::getTabArea(void) const {
	return mTabArea;
}

Client* Tab::getSelected(void) const {
	return mClientFrames[mSelected]->getClient();
}

ClientFrame* Tab::getSelectedFrame(void) const {
	return mClientFrames[mSelected].get();
}

void Tab::setSelected(int idx) {
	if (idx < 0 || idx >= clientCount()) return;

	if (idx != mSelected) {
		mLastSelected = getSelected();
	}
	mSelected = idx;

	if (clientCount() > 0) {
		getDisplay()->setSelected(getSelected());
	}
}

void Tab::setSelectedQuiet(int idx) {
	if (idx < 0 || idx >= clientCount()) return;

	mSelected = idx;
}

void Tab::setSelected(Client* client) {
	setSelected(getClientIndex(client));
}

void Tab::setSelectedQuiet(Client* client) {
	setSelectedQuiet(getClientIndex(client));
}

int Tab::getClientIndex(Client* find) {
	int i = 0;
	for (const auto& client : mClientFrames) {
		if (client->getClient() == find) {
			return i;
		}
		i++;
	}
	return -1;
}

void Tab::activate(void) {
	setSelected(mSelected);
}

void Tab::addClientFrame(std::unique_ptr<ClientFrame> pframe) {
	ClientFrame* frame = mClientFrames.insert(mClientFrames.end(), std::move(pframe))->get();
	addChild(frame);
	reorganize();
}

void Tab::addAndSelectClientFrame(std::unique_ptr<ClientFrame> pframe) {
	ClientFrame* frame = mClientFrames.insert(mClientFrames.begin(), std::move(pframe))->get();
	mSelected += 1 * (clientCount() > 1);
	addChild(frame);
	setSelected(0);
	reorganize();
}

std::vector<Client*> Tab::getClients(void) {
	std::vector<Client*> clients;
	for (auto& clientFrame : mClientFrames) {
		clients.push_back(clientFrame->getClient());
	}
	return clients;
}

void Tab::closeClient(Client* client) {
	getDisplay()->defer(
		[this, client](void) {
			auto it = std::find_if(
				mClientFrames.begin(),
				mClientFrames.end(),
				[client](const auto& ptr) { return client == ptr->getClient(); }
			);

			if (it == mClientFrames.end()) return;
			mClientFrames.erase(it);
			mSelected = 0;

			if (clientCount() > 0) {
				reorganize();
				requireRedrawUp();
			} else {
				mTabArea->openNextTabWithClients();
			}
		}
	);
}

void Tab::selectNextClient(void) {
	setSelected((mSelected + 1) % mClientFrames.size());
}

void Tab::selectPrevClient(void) {
	setSelected((mSelected + mClientFrames.size() - 1) % mClientFrames.size());
}

void Tab::moveClientToTab(Client* client, int tab) {
	int idx = getClientIndex(client);
	std::unique_ptr<ClientFrame> frame = std::move(mClientFrames[idx]);
	removeChild(frame.get());
	mClientFrames.erase(mClientFrames.begin() + idx);
	Tab* newTab = mTabArea->getTab(tab);
	frame->getClient()->setTab(newTab);
	newTab->addClientFrame(std::move(frame));

	if (clientCount() > 0) {
		mSelected = 0;
		getDisplay()->setSelected(getSelected());
		reorganize();
		requireRedrawUp();
	} else {
		mTabArea->setActive(tab);
	}
}

void Tab::incrementSplitPos(float inc) {
	float next = mSplitPos + inc;
	if (next < 0 || next > 1) return;
	mSplitPos = next;
	reorganize();
}

void Tab::incrementMasterCount(int inc) {
	int next = mMasterCount + inc;
	if (next < 0) return;
	mMasterCount = next;
	reorganize();
}

void Tab::swapSelectedWithMaster(void) {
	if (mSelected == 0) {
		if (mClientFrames.size() > 1) {
			mSelected = 1;
		} else {
			return;
		}
	}
	std::swap(mClientFrames[0], mClientFrames[mSelected]);
	setSelected(0);
	reorganize();
}

std::vector<ClientFrame*> Tab::getManagedClientFrames(void) {
	std::vector<ClientFrame*> managedClients;
	for (auto& clientFrame : mClientFrames) {
		if (clientFrame->getClient()->isFloating() == false) {
			managedClients.push_back(clientFrame.get());
		}
	}
	return managedClients;
}

void Tab::nextLayout(void) {
	mCurrentLayout = (LayoutType)(((int)mCurrentLayout + 1) % (int)LayoutType::EnumLength);
	reorganize();
	requireRedraw();
}

void Tab::tileOrganize(void) {
	mBarPos = -1;
	std::vector<ClientFrame*> managedClients = getManagedClientFrames();

	auto organizeHalf = [this](auto begin, auto end, int x, int width, int height) {
		int leftovers = getHeight() - (height * std::distance(begin, end));
		Vec2 pos = Vec2(x, getYOffset());
		for (auto it = begin; it != end; it++) {
			ClientFrame* c = *it;
			c->setPos(pos);
	
			Vec2 size(width, height);
			if (leftovers > 0) {
				size.y += 1;
				leftovers -= 1;
			}
			c->setSize(size);
	
			pos.y += size.y;
		}
	};

	int count = managedClients.size();
	if (count == 0) return;

	requireRedraw();

	int width = mSize.x;
	int height = getHeight();

	int masterCount = (mMasterCount > 0)
							? (std::min(mMasterCount, count))
							: (count);

	int masterHeight = (count >= masterCount)
							 ? (height / masterCount)
							 : (height / count);
	int masterWidth = (count > masterCount)
							? ((float)width * mSplitPos)
							: (width);

	organizeHalf(
		managedClients.begin(),
		managedClients.begin() + masterCount,
		0, masterWidth, masterHeight
	);

	int clientCount = count - masterCount;
	if (clientCount == 0) return;

	int clientWidth = width - masterWidth;
	int clientHeight = height / clientCount;

	mBarPos = masterWidth;

	organizeHalf(
		managedClients.begin() + masterCount,
		managedClients.begin() + masterCount + clientCount,
		masterWidth, clientWidth, clientHeight
	);
}

void Tab::monocleOrganize(void) {
	mBarPos = -1;
	std::vector<ClientFrame*> managedClients = getManagedClientFrames();
	if (managedClients.size() == 0) return;
	for (ClientFrame* c : managedClients) {
		c->setPos(Vec2(0, getYOffset()));
		c->setSize(Vec2(mSize.x, getHeight()));
	}

	requireRedraw();
}

void Tab::reorganize(void) {
	switch (mCurrentLayout) {
	case LayoutType::Tile: {
		tileOrganize();
	} break;
	case LayoutType::Monocle: {
		monocleOrganize();
	} break;
	default: break;
	}
}

void Tab::drawSplitBar(void) {
//    Draw::style(Style(Color(0, 0, 0), Color(255, 255, 255)));
//    drawFilledBox(Vec2(mBarPos, getYOffset()), Vec2(1, getHeight()), "|");
}

void Tab::onLeftMouseButtonDrag(Vec2 pos) {
	mSplitPos = (float)pos.x / (float)mSize.x;
	mSplitPos = std::clamp(mSplitPos, 0.05f, 0.95f);
	reorganize();
}

void Tab::onResize(void) {
	reorganize();
}

void Tab::onPrerender(void) {
	requireSelfRedraw();

	if (getDisplay()->getSelected() == nullptr) {
		if (mLastSelected) {
			setSelected(mLastSelected);
			mLastSelected = nullptr;
		} else {
			setSelected(0);
		}
	}

	std::vector<ClientFrame*> managedClients = getManagedClientFrames();
	if (managedClients.size() == 0) return;
	for (ClientFrame* c : managedClients) {
		c->setLayer(0);
	}
	getSelectedFrame()->setLayer(1);
}

void Tab::render(void) {
	const Theme* theme = Vidd::getTheme();
	Draw::style(theme->text);
	if (mBarPos >= 0) {
		drawSplitBar();
	}

	if (mShowTabBar) {
		Style activeStyle = theme->text;
		Style inactiveStyle = activeStyle;
		inactiveStyle.bg = inactiveStyle.bg.lighten(1.5);
		Draw::style(inactiveStyle);
		drawFilledBox(Vec2(0, 0), Vec2(mSize.x, 1), " ");
	
		drawText(Vec2(0, 0), ""_ws);
		for (auto& frame : mClientFrames) {
			Client* client = frame->getClient();
			if (client == getSelected()) {
				Draw::style(activeStyle);
			} else {
				Draw::style(inactiveStyle);
			}
			drawText(" "_ws);
			drawText(client->getTitle());
			drawText(" "_ws);
		}
	}
}
