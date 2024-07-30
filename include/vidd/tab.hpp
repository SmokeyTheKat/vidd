#ifndef __VIDD_TAB_HPP__
#define __VIDD_TAB_HPP__

#include <vidd/component.hpp>
#include <vidd/client.hpp>
#include <vidd/display.hpp>

#include <vector>
#include <functional>
#include <memory>

class TabArea;

class Tab : public Component {
	enum LayoutType {
		Tile=0,
		Monocle,
		EnumLength,
	};

	TabArea* mTabArea = nullptr;
	std::vector<std::unique_ptr<ClientFrame>> mClientFrames;
	LayoutType mCurrentLayout = LayoutType::Monocle;
	bool mShowTabBar = false;
	int mSelected = 0;
	Client* mLastSelected = nullptr;
	int mMasterCount = 1;
	float mSplitPos = 0.5;
	int mBarPos = -1;

public:
	Tab(TabArea* tabArea);

	TabArea* getTabArea(void) const;

	Client* getSelected(void) const;
	ClientFrame* getSelectedFrame(void) const;
	void setSelected(int idx);
	void setSelectedQuiet(int idx);
	void setSelected(Client* client);
	void setSelectedQuiet(Client* client);

	int getClientIndex(Client* find);

	void setLastSelected(void);

	void activate(void);

	std::vector<Client*> getClients(void);

	void addClientFrame(std::unique_ptr<ClientFrame> frame);
	void addAndSelectClientFrame(std::unique_ptr<ClientFrame> frame);

	template <class T, class... Args>
	T* addClient(Args&&... args) {
		T* ptr = new T(this, std::forward<Args>(args)...);
		addClientFrame(std::make_unique<ClientFrame>(std::unique_ptr<Client>(ptr)));
		return ptr;
	};

	template <class T, class... Args>
	T* addAndSelectClient(Args&&... args) {
		T* ptr = new T(this, std::forward<Args>(args)...);
		addAndSelectClientFrame(std::make_unique<ClientFrame>(std::unique_ptr<Client>(ptr)));
		return ptr;
	};

	template <class T, class... Args>
	T* replaceClientWithNew(Client* client, Args&&... args) {
		ClientFrame* frame = static_cast<ClientFrame*>(client->getParent());
		T* ptr = new T(this, std::forward<Args>(args)...);
		frame->replace(std::unique_ptr<Client>(ptr));
		getDisplay()->setSelected(ptr);
		return ptr;
	};

	void closeClient(Client* client);

	void selectNextClient(void);
	void selectPrevClient(void);

	void moveClientToTab(Client* client, int tab);

	int clientCount(void) const {
		return mClientFrames.size();
	};

	void toggleTabBar(void) { mShowTabBar = !mShowTabBar; reorganize(); };

	void incrementSplitPos(float inc);
	void incrementMasterCount(int inc);
	void swapSelectedWithMaster(void);

	void nextLayout(void);

	void tileOrganize(void);
	void monocleOrganize(void);
	void reorganize(void);

	void onLeftMouseButtonDrag(Vec2 pos) override;
	void onResize(void) override;
	void onPrerender(void) override;
	void render(void) override;

private:
	int getHeight(void) const;
	int getYOffset(void) const;
	std::vector<ClientFrame*> getManagedClientFrames(void);

	void drawSplitBar(void);
};

#endif
