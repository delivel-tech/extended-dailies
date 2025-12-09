#include "ManageDailyPopup.hpp"
#include "Geode/cocos/menu_nodes/CCMenu.h"
#include "Geode/cocos/platform/CCPlatformMacros.h"
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/binding/FLAlertLayer.hpp>
#include <Geode/cocos/label_nodes/CCLabelBMFont.h>

using namespace geode::prelude;

ManageDailyPopup* ManageDailyPopup::create(std::function<void()> onDelete, std::function<void()> onSuggest) {
    auto ret = new ManageDailyPopup;
    if (ret && ret->initAnchored(320.f, 120.f, "GJ_square01.png")) {
        ret->autorelease();
        ret->m_onSuggest = std::move(onSuggest);
        ret->m_onDelete = std::move(onDelete);
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool ManageDailyPopup::setup() {
    this->setTitle("Mod: Manage Daily", "bigFont.fnt", 1.2f);

    auto content = this->m_mainLayer;

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto menu = CCMenu::create();
    menu->setPosition(this->getContentSize().width / 2, 40.f);
    content->addChild(menu);

    auto deleteSpr = ButtonSprite::create("Delete");
    auto deleteBtn = CCMenuItemSpriteExtra::create(deleteSpr, this, menu_selector(ManageDailyPopup::onDelete));
    
    auto suggestSpr = ButtonSprite::create("Submit");
    auto suggestBtn = CCMenuItemSpriteExtra::create(suggestSpr, this, menu_selector(ManageDailyPopup::onSuggest));

    deleteBtn->setPosition(-203.f, -12.f);
    suggestBtn->setPosition(-54.f, -12.f);

    menu->addChild(deleteBtn);
    menu->addChild(suggestBtn);

    return true;
}

void ManageDailyPopup::onDelete(CCObject* sender) {
    if (m_onDelete) m_onDelete();
}

void ManageDailyPopup::onSuggest(CCObject* sender) {
    if (m_onSuggest) m_onSuggest();
}