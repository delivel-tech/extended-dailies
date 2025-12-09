#pragma once

#include <Geode/Geode.hpp>
#include <Geode/binding/GJSearchObject.hpp>
#include <Geode/binding/LevelBrowserLayer.hpp>

using namespace geode::prelude;

class ExtendedBrowserLayer : public LevelBrowserLayer {
    public:
    static ExtendedBrowserLayer* create(GJSearchObject* search);

    bool init(GJSearchObject* search);

    void updateTitleLabel();
};