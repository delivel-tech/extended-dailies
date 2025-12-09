#include "ExtendedLevelsUtils.hpp"
#include <Geode/utils/web.hpp>
#include <matjson.hpp>

using namespace geode::prelude;

static std::vector<int> g_cachedLevels;

namespace ExtendedDaily {

std::vector<int>& getCachedLevels() {
    return g_cachedLevels;
}

void loadLevels(std::function<void(const std::vector<int>&)> callback) {
    web::WebRequest req;

    auto task = req.get("https://delivel.tech/extapi/get_daily");

    task.listen([callback](web::WebResponse* res) {

        if (!res || !res->ok()) {
            callback(g_cachedLevels);
            return;
        }

        auto jsonResult = res->json();
        if (!jsonResult) {
            callback(g_cachedLevels);
            return;
        }

        auto json = jsonResult.unwrap();
        auto arrRes = json["levels"].asArray();
        if (!arrRes) {
            callback(g_cachedLevels);
            return;
        }

        auto& arr = arrRes.unwrap();

        g_cachedLevels.clear();

        for (auto& v : arr) {
            int id = v.asInt().unwrapOr(0);
            if (id > 0)
                g_cachedLevels.push_back(id);
        }

        callback(g_cachedLevels);
    });
}

}