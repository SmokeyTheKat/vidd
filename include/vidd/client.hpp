#ifndef __VIDD_CLIENT_HPP__
#define __VIDD_CLIENT_HPP__

#include <vidd/pos.hpp>
#include <vidd/keybinds.hpp>
#include <vidd/component.hpp>

#include <memory>
#include <vector>

class Tab;
class TabArea;
class ClientFrame;

class Client : public Component {
	Tab* mTab = nullptr;
protected:
	std::vector<Key> mKeyBuffer;
	const AliasBinds* mAliases = nullptr;
	const KeyBinds* mKeyBinds = nullptr;
	bool mIsFloating = false;

public:
	Client(Tab* tab)
	: Component(Vec2::zero(), Vec2::ones()), mTab(tab) {};

	virtual ~Client(void);

	bool isFloating(void) const { return mIsFloating; };
	void setFloating(void);
	void setNotFloating(void);
	void toggleFloating(void);

	void windowMoveX(int dx);
	void windowMoveY(int dy);
	void windowResizeX(int dx);
	void windowResizeY(int dy);

	bool interpretBindSet(const KeyBinds& bindSet);
	bool interpretAliasSet(const AliasBinds& aliasSet);
	bool interpret(Key key);

	bool keysBuffered(void) const {
		return mKeyBuffer.size() > 0;
	};

	virtual void unhandledKey(Key key) {};
	virtual WString getTitle(void) { return "client"_ws; };

	TabArea* getTabArea(void) const;
	Tab* getTab(void) const;
	void setTab(Tab* tab) { mTab = tab; };

	void close(void);

	void quit(void);
};

class ClientFrame : public Component {
	std::unique_ptr<Client> mClient;
	bool mLateClientHasBorder = false;

public:
	ClientFrame(std::unique_ptr<Client> client)
	: Component(Vec2::zero(), Vec2::ones()), mClient(std::move(client)) {};

	Client* getClient(void) const { return mClient.get(); };

	void replace(std::unique_ptr<Client> newClient) {
		if (mClient->isFloating()) newClient->setFloating();
		else newClient->setNotFloating();
		mClient.reset(newClient.release());
		onAttach();
		onResize();
	};

	void onAttach(void) override;
	void onResize(void) override;
	void onPrerender(void) override;
	void render(void) override;
};

#endif
