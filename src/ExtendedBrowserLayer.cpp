#include "ExtendedBrowserLayer.hpp"
#include "Geode/cocos/label_nodes/CCLabelBMFont.h"
#include "Geode/cocos/platform/CCPlatformMacros.h"
#include "Geode/platform/windows.hpp"
#include <Geode/binding/GJSearchObject.hpp>
#include <Geode/binding/LevelBrowserLayer.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>

using namespace geode::prelude;

ExtendedBrowserLayer* ExtendedBrowserLayer::create(GJSearchObject *search) {
    auto ret = new ExtendedBrowserLayer;
    if (ret && ret->init(search)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

void ExtendedBrowserLayer::updateTitleLabel() {
    if (auto node = this->getChildByIDRecursive("title")) {
        if (auto label = typeinfo_cast<CCLabelBMFont*>(node)) {
            label->setString("Extended Dailies");
        };
    };
}

bool ExtendedBrowserLayer::init(GJSearchObject* search) {
    if (!LevelBrowserLayer::init(search)) return false;

    this->updateTitleLabel();
    return true;
};

class $modify(ExtBrowserReloadHook, LevelBrowserLayer) {
    public:
        gd::string getSearchTitle() {
            if (auto ext = typeinfo_cast<ExtendedBrowserLayer*>(this)) {
            return "Extended Dailies";
        }
        return LevelBrowserLayer::getSearchTitle();
        }
};