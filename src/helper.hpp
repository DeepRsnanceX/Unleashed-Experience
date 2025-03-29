#pragma once
#include "cocos2d.h"
#include <Geode/binding/GJBaseGameLayer.hpp>

class SonicUnleashed {
public:
    // speedmeter sfx
    void meterCount(float dt);
    void timerCheck(float dt);
    void timerCheck2(float dt);
    void timerCheck3(float dt);
    void timerCheck4(float dt);
    void timerEnd(float dt);
    // sfx for the ranking screen
    void slide1(float dt);
    void slide2(float dt);
    void slide3(float dt);
    void rankReaction(float dt);
    void rankPlacement(float dt);
    void totalScoreSound(float dt);
    // music
    void stageClear(float dt);
    void rankingMusic(float dt);
    // misc
    void cooldownLifeUp(float dt);
};
