#include "ExtendedBrowserLayer.hpp"
#include "ExtendedLevelsUtils.hpp"
#include "Geode/cocos/CCDirector.h"
#include "Geode/cocos/layers_scenes_transitions_nodes/CCTransition.h"
#include <Geode/Enums.hpp>
#include <Geode/Geode.hpp>
#include <Geode/binding/DailyLevelPage.hpp>
#include <Geode/binding/GJSearchObject.hpp>
#include <Geode/binding/LevelBrowserLayer.hpp>
#include <Geode/modify/DailyLevelPage.hpp>
#include <vector>
#include <matjson.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>

using namespace geode::prelude;

class $modify(ExtDailyPage, DailyLevelPage) {
public:
    bool init(GJTimedLevelType type) {
        if (!DailyLevelPage::init(type))
            return false;

        auto extmenu = getChildByIDRecursive("main-menu");

        auto extspr = CCSprite::createWithSpriteFrameName("GJ_deSelBtn_001.png");
        extspr->setScale(0.8f);

        auto extbtn = CCMenuItemSpriteExtra::create(
            extspr, this, menu_selector(ExtDailyPage::onExtBtnClick)
        );

        extmenu->addChild(extbtn);
        extbtn->setPosition(386, -247);

        if (auto layoutNode = typeinfo_cast<CCNode*>(extmenu)) {
            layoutNode->updateLayout();
        }

		ExtendedDaily::loadLevels([this](auto const& ids) {
			log::info("Extended Dailies: loaded {} ids", ids.size());
		});

        return true;
    };

	void onExtBtnClick(CCObject*) {
        auto& ids = ExtendedDaily::getCachedLevels();

        if (ids.empty()) {
            FLAlertLayer::create(
                "Extended Dailies",
                "No levels found!",
                "OK"
            )->show();
            return;
        }

        std::string levelIDs;
        for (size_t i = 0; i < ids.size(); ++i) {
            if (i > 0)
                levelIDs += ",";
            levelIDs += std::to_string(ids[i]);
        }

        auto search = GJSearchObject::create(SearchType::Type19, levelIDs);
        auto browser = ExtendedBrowserLayer::create(search);

        auto scene = CCScene::create();
        scene->addChild(browser);

        auto transition = CCTransitionFade::create(0.5f, scene);
        CCDirector::sharedDirector()->pushScene(transition);
    }
};

class $modify(ReloadHook, LevelBrowserLayer) {
	void onRefresh(CCObject* sender) {
		LevelBrowserLayer::onRefresh(sender);
		ExtendedDaily::loadLevels([this](auto const& ids) {
			log::info("Extended Dailies: loaded {} ids", ids.size());
		});
	}
};