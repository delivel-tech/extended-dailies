#include "ExtendedLevelsUtils.hpp"
#include "Geode/cocos/cocoa/CCObject.h"
#include <Geode/Geode.hpp>

#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/binding/FLAlertLayer.hpp>
#include <Geode/binding/GJAccountManager.hpp>
#include <Geode/binding/GJGameLevel.hpp>
#include <Geode/binding/LevelInfoLayer.hpp>

#include <Geode/utils/web.hpp>
#include <Geode/loader/Event.hpp>
#include <fmt/format.h>
#include <matjson.hpp>
#include <Geode/binding/UploadActionPopup.hpp>
#include <vector>
#include "Geode/cocos/label_nodes/CCLabelBMFont.h"
#include "Geode/cocos/sprite_nodes/CCSprite.h"
#include "Geode/loader/Log.hpp"
#include "Geode/loader/ModEvent.hpp"
#include "ManageDailyPopup.hpp"

using namespace geode::prelude;

#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/ProfilePage.hpp>
#include <Geode/modify/CommentCell.hpp>

static std::vector<int> g_moderators;
bool loading = false;
bool loaded = false;

void loadModerators() {
    if (loading) return;

    loading = true;

    web::WebRequest req;

    auto task = req.get("https://delivel.tech/extapi/get_moderators");

    task.listen([](web::WebResponse* res) {
        loading = false;
        loaded = false;

        if (!res || !res->ok()) {
            log::error("Failed to fetch mods");
            return;
        }

        auto jsonResult = res->json();
        if (!jsonResult) {
            log::error("Failed to fetch mods");
            return;
        }

        auto json = jsonResult.unwrap();
        auto arrRes = json["moderators"].asArray();
        if (!arrRes) {
            log::error("Failed to fetch mods");
            return;
        }

        auto& arr = arrRes.unwrap();
        g_moderators.clear();

        for (auto& v : arr) {
            int id = v.asInt().unwrapOr(0);
            if (id > 0) g_moderators.push_back(id);
        }

        loaded = true;
    });
}

$on_mod(Loaded) {
    loadModerators();
}

class $modify(ExtProfilePage, ProfilePage) {
    void loadPageFromUserInfo(GJUserScore* a2) {
        ProfilePage::loadPageFromUserInfo(a2);

        int accountID = a2->m_accountID;

        bool isMod = std::find(
            g_moderators.begin(), g_moderators.end(), accountID
        ) != g_moderators.end();

        if (!isMod) {
            return;
        }

        auto layer = m_mainLayer;
        
        CCMenu* username_menu = static_cast<CCMenu*>(layer->getChildByIDRecursive("username-menu"));

        auto spr = CCSprite::createWithSpriteFrameName("modBadge_03_001.png");

        auto modBadge = CCMenuItemSpriteExtra::create(spr, this, menu_selector(ExtProfilePage::onBadge));
        
        modBadge->setID("extdailymod-badge");
        username_menu->addChild(modBadge);
        username_menu->updateLayout();
    }
    void onBadge(CCObject* sender) {
        FLAlertLayer::create("Daily Moderator", "This user is <cy>responsible for management of daily levels</c> in <cp>Extended Dailies</c> mod.", "OK")->show();
    }

};

class $modify(ExtCommentCell, CommentCell) {
    void loadFromComment(GJComment* p0) {
        CommentCell::loadFromComment(p0);

        int accountID = p0->m_accountID;

        bool isMod = std::find(
            g_moderators.begin(), g_moderators.end(), accountID
        ) != g_moderators.end();

        if (!isMod) {
            return;
        }

        auto layer = m_mainLayer;
        
        CCMenu* username_menu = static_cast<CCMenu*>(layer->getChildByIDRecursive("username-menu"));
        if (!username_menu) return;

        auto spr = CCSprite::createWithSpriteFrameName("modBadge_03_001.png");

        auto modBadge = CCMenuItemSpriteExtra::create(spr, this, menu_selector(ExtCommentCell::onModBadge));

        modBadge->setID("extdailymod-badge");
        username_menu->addChild(modBadge);
        username_menu->updateLayout();
    }
    void onModBadge(CCObject* sender) {
        FLAlertLayer::create("Daily Moderator", "This user is <cy>responsible for management of daily levels</c> in <cp>Extended Dailies</c> mod.", "OK")->show();
    }
};

class $modify(MyLIL, LevelInfoLayer) {
public:
    struct Fields {
        EventListener<web::WebTask> m_listener;
        UploadActionPopup* popup = nullptr;
    };

    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge))
            return false;

        auto level1 = this->m_level;
        auto& ids = ExtendedDaily::getCachedLevels();
        int levelID1 = level1->m_levelID;
        
        bool isDaily = std::ranges::find(ids, levelID1) != ids.end();

        if (isDaily) {
            auto titleMenu = getChildByIDRecursive("garage-menu");

            auto dailyTitle = CCLabelBMFont::create("(Daily)", "goldFont.fnt");
            dailyTitle->setPosition(-407.f, 7.f);
            dailyTitle->setScale(0.7f);

            titleMenu->addChild(dailyTitle);
        }

        int accountID = GJAccountManager::get()->m_accountID;

        bool isMod = std::find(
            g_moderators.begin(), g_moderators.end(), accountID
        ) != g_moderators.end();

        if (!isMod) {
            return true;
        }

        auto menu = this->getChildByIDRecursive("left-side-menu");
        if (!menu)
            return true;

        auto spr = CCSprite::createWithSpriteFrameName("GJ_starBtnMod_001.png");

        auto btn = CCMenuItemSpriteExtra::create(
            spr,
            this,
            menu_selector(MyLIL::onButtonClick)
        );

        menu->addChild(btn);
        menu->updateLayout();

        return true;
    }

    void onButtonClick(CCObject* sender) {
        auto popup = ManageDailyPopup::create([this]() {
            auto level = this->m_level;
            if (!level) return;

            int levelID = level->m_levelID;
            std::string password = Mod::get()->getSettingValue<std::string>("mod-password");

            auto popup1 = UploadActionPopup::create(nullptr, "Deleting daily...");
            popup1->show();
            m_fields->popup = popup1;

            matjson::Value body;
            body["level_id"] = levelID;
            body["mod_password"] = password;

            web::WebRequest req;
            req.bodyJSON(body);

            auto task = req.post("https://delivel.tech/extapi/delete_daily");

            m_fields->m_listener.bind([this](web::WebTask::Event* e) {
                auto popup = m_fields->popup;

                if (auto* res = e->getValue()) {
                    if (res->ok()) {
                        popup->showSuccessMessage("Daily deleted!");
                    } else {
                        popup->showFailMessage("Failed! Try again later.");
                    }
                }
            });

            m_fields->m_listener.setFilter(task);
        },
    [this]() {
        auto level = this->m_level;
        if (!level) return;

        int levelID = level->m_levelID;
        auto acc = GJAccountManager::get();
        std::string password = Mod::get()->getSettingValue<std::string>("mod-password");

                auto popup1 = UploadActionPopup::create(nullptr, "Submitting daily...");
                popup1->show();
                m_fields->popup = popup1;

                matjson::Value body;
                body["level_id"]     = levelID;
                body["set_by_admin"] = acc->m_username;
                body["mod_password"] = password;

                web::WebRequest req;
                req.bodyJSON(body);

                auto task = req.post("https://delivel.tech/extapi/set_daily");

                m_fields->m_listener.bind([this](web::WebTask::Event* e) {
                    auto popup = m_fields->popup;

                    if (auto* res = e->getValue()) {
                        if (res->ok()) {
                            popup->showSuccessMessage("Submitted successfully!");
                        } else {
                            popup->showFailMessage("Failed! Try again later.");
                        }
                    }
                });

                m_fields->m_listener.setFilter(task);
    });
    popup->show();
            };
        
    };