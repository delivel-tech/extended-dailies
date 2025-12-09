#pragma once
#include <vector>
#include <functional>

namespace ExtendedDaily {
    void loadLevels(std::function<void(const std::vector<int>&)> callback);
    std::vector<int>& getCachedLevels();
}