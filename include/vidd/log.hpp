#ifndef __VIDD_LOG_HPP__
#define __VIDD_LOG_HPP__

#include <string>
#include <vector>

namespace Log {
    void log(std::string_view msg);
    void clear(void);
    std::vector<std::string>& get(void);
};

#endif
