#pragma once
#include "Geode/cocos/cocoa/CCObject.h"
#include <Geode/Geode.hpp>
#include <Geode/ui/Popup.hpp>

using namespace geode::prelude;

class ManageDailyPopup : public Popup<> {
    protected:
    bool setup() override;
    void onDelete(CCObject*);
    void onSuggest(CCObject*);

    std::function<void()> m_onDelete;
    std::function<void()> m_onSuggest;

    public:
    static ManageDailyPopup* create(
        std::function<void()> m_onDelete,
        std::function<void()> m_onSuggest
    );
};