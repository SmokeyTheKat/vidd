#ifndef __VIDD_LOG_CLIENT_HPP__
#define __VIDD_LOG_CLIENT_HPP__

#include <vidd/client.hpp>

class LogClient : public Client {
	std::size_t mLastSize = 0;

public:
	LogClient(Tab* tab)
	: Client(tab) {};

	void onKeyDown(Key key) override;
	void onPrerender(void) override;
	void render(void) override;
};

#endif
