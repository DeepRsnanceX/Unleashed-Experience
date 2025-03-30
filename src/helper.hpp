#pragma once
#include "cocos2d.h"
#include <Geode/binding/GJBaseGameLayer.hpp>

class SonicUnleashed {
public:
    // speedmeter sfx
    void meterCount(float dt);
    void meterCheck1(float dt);
    void meterCheck2(float dt);
    void meterCheck3(float dt);
    void meterCheck4(float dt);
    void timerEnd(float dt);
    // sfx for the ranking screen
    void allSlides(float dt);
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
