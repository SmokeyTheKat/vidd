#ifndef __VIDD_TABAREA_HPP__
#define __VIDD_TABAREA_HPP__

#include <vidd/component.hpp>
#include <vidd/tab.hpp>

#include <memory>
#include <vector>

class Tab;
class Client;

class TabArea : public Component {
	std::vector<std::unique_ptr<Tab>> mTabs;
	int mLastTab = 0;
	int mActiveTab = 0;
	int mTabWidth = 3;
	bool mShowTabBar = true;

public:
	TabArea(void);

	Tab* getActive(void) const {
		return mTabs[mActiveTab].get();
	};

	Tab* getTab(int tab) const {
		return mTabs[tab].get();
	};

	int tabCount(void) const {
		return mTabs.size();
	};

	void showTabBar(bool state) { mShowTabBar = state; resizeTabs(); requireRedraw(); };
	void toggleBar(void) { showTabBar(!mShowTabBar); };

	void setNextActive(void);
	void setPrevActive(void);
	void setActive(int idx);
	void setActive(Tab* tab);
	void setLastActive(void);
	void setChild(void);

	std::vector<Client*> getAllClients(void);

	template<class T>
	std::vector<T*> getAllClientsOf(void) {
		std::vector<T*> clients;
		for (auto& tab : mTabs) {
			std::vector<Client*> tabClients = tab->getClients();
			for (Client* c : tabClients) {
				if (T* t = dynamic_cast<T*>(c)) {
					clients.push_back(t);
				}
			}
		}
		return clients;
	}

	void openNextTabWithClients(void);

	void resizeTabs(void) const;

	void onLeftMouseButtonDown(Vec2 pos) override;
	void onResize(void) override;
	void onAttach(void) override;
	void onPrerender(void) override;
	void render(void) override;

private:
	int getTabId(Tab* tab);
};

#endif
