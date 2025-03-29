#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <unordered_map>
#include "helper.hpp"

using namespace geode::prelude;

auto playMeterSFX = Mod::get()->getSettingValue<bool>("speedmeter-sfx");
auto fakeCheckpoint = Mod::get()->getSettingValue<bool>("fake-checkpoint");

$on_mod(Loaded) {
    listenForSettingChanges("speedmeter-sfx", [](bool value) {
        playMeterSFX = value;
    });
    listenForSettingChanges("fake-checkpoint", [](bool value) {
        fakeCheckpoint = value;
    });
}

float playerSpeed = 0.f;

void SonicUnleashed::timerCheck(float dt){
    auto fmod = FMODAudioEngine::sharedEngine();
    fmod->playEffect("speedmeter_check.ogg"_spr);
}
void SonicUnleashed::timerCheck2(float dt){
    auto fmod = FMODAudioEngine::sharedEngine();
    fmod->playEffect("speedmeter_check.ogg"_spr);
}
void SonicUnleashed::timerCheck3(float dt){
    auto fmod = FMODAudioEngine::sharedEngine();
    fmod->playEffect("speedmeter_check.ogg"_spr);
}
void SonicUnleashed::timerCheck4(float dt){
    auto fmod = FMODAudioEngine::sharedEngine();
    fmod->playEffect("speedmeter_check.ogg"_spr);
}

void SonicUnleashed::timerEnd(float dt){
    auto fmod = FMODAudioEngine::sharedEngine();
    fmod->playEffect("speedmeter_finished.ogg"_spr);
}

class $modify(PlayerObject){
    void updateTimeMod(float p0, bool p1) {
        PlayerObject::updateTimeMod(p0, p1);

        if (PlayLayer::get()){
            playerSpeed = p0;
        }
    }
};

class $modify(PlayLayer) {
    struct Fields {
        std::unordered_map<int, bool> progressLogged;
        float meterOffDelay = 3.23f;
    };

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        float meterNodeStartX = winSize.width - 269.0f;
        float meterNodeEndX = winSize.width - 569.0f;

        auto blinkAnim = CCSequence::create(
            CCFadeTo::create(0.1f, 30),
            CCFadeTo::create(0.1f, 90),
            nullptr
        );
        auto blinkAnim2 = CCSequence::create(
            CCFadeTo::create(0.1f, 30),
            CCFadeTo::create(0.1f, 90),
            nullptr
        );
        auto blinkAnim3 = CCSequence::create(
            CCFadeTo::create(0.1f, 30),
            CCFadeTo::create(0.1f, 90),
            nullptr
        );
        auto blinkAnim4 = CCSequence::create(
            CCFadeTo::create(0.1f, 30),
            CCFadeTo::create(0.1f, 90),
            nullptr
        );

        // speedmeter sprites
        auto meterUnder = CCSprite::createWithSpriteFrameName("speed_meter_alt.png"_spr);
        auto meterSpd = CCSprite::createWithSpriteFrameName("speed_spd-hq.png"_spr);
        auto meterCounter0 = CCSprite::createWithSpriteFrameName("speed_counter-hq.png"_spr);
        auto meterCounter1 = CCSprite::createWithSpriteFrameName("speed_counter-hq.png"_spr);
        auto meterCounter2 = CCSprite::createWithSpriteFrameName("speed_counter-hq.png"_spr);
        auto meterCounter3 = CCSprite::createWithSpriteFrameName("speed_counter-hq.png"_spr);

        // tint the meter under, color is #B7AFF7
        auto meterUnderColor = ccc3(183, 175, 247);
        meterUnder->setColor(meterUnderColor);

        auto meterGlow0 = CCSprite::createWithSpriteFrameName("counter-glow.png"_spr);
        auto meterGlow1 = CCSprite::createWithSpriteFrameName("counter-glow.png"_spr);
        auto meterGlow2 = CCSprite::createWithSpriteFrameName("counter-glow.png"_spr);
        auto meterGlow3 = CCSprite::createWithSpriteFrameName("counter-glow.png"_spr);

        auto meterBack0 = CCSprite::createWithSpriteFrameName("speed_counter-bg.png"_spr);
        auto meterBack1 = CCSprite::createWithSpriteFrameName("speed_counter-bg.png"_spr);
        auto meterBack2 = CCSprite::createWithSpriteFrameName("speed_counter-bg.png"_spr);
        auto meterBack3 = CCSprite::createWithSpriteFrameName("speed_counter-bg.png"_spr);

        auto meterNode = CCNode::create();
        meterNode->setID("unleashed-speedmeter"_spr);
        meterNode->setZOrder(100);
        meterNode->setPosition({meterNodeStartX, 0.f});

        if (!meterUnder || !meterSpd || !meterCounter0 || !meterCounter1 || !meterCounter2 || !meterCounter3) {
            geode::log::error("Failed to create one or more speedmeter sprites");
            return false;
        }

        // set their IDs for fetching later
        meterUnder->setID("speed-meter-under"_spr);
        meterSpd->setID("speed-meter-spd"_spr);
        meterCounter0->setID("speed-meter-counter0"_spr);
        meterCounter1->setID("speed-meter-counter1"_spr);
        meterCounter2->setID("speed-meter-counter2"_spr);
        meterCounter3->setID("speed-meter-counter3"_spr);
        meterGlow0->setID("speed-meter-glow0"_spr);
        meterGlow1->setID("speed-meter-glow1"_spr);
        meterGlow2->setID("speed-meter-glow2"_spr);
        meterGlow3->setID("speed-meter-glow3"_spr);
        meterBack0->setID("speed-meter-back0"_spr);
        meterBack1->setID("speed-meter-back1"_spr);
        meterBack2->setID("speed-meter-back2"_spr);
        meterBack3->setID("speed-meter-back3"_spr);

        // positions and scales
        meterUnder->setPosition({585, 153});
        meterUnder->setScale(0.8f);
        meterSpd->setPosition({495 + 5, 161});
        meterSpd->setScale(1.1f);
        meterCounter0->setPosition({450 + 5, 167});
        meterCounter1->setPosition({458 + 5, 167});
        meterCounter2->setPosition({466 + 5, 167});
        meterCounter3->setPosition({474 + 5, 167});
        meterCounter0->setScale(1.35f);
        meterCounter1->setScale(1.35f);
        meterCounter2->setScale(1.35f);
        meterCounter3->setScale(1.35f);

        meterGlow0->setPosition({450 + 5, 167});
        meterGlow1->setPosition({458 + 5, 167});
        meterGlow2->setPosition({466 + 5, 167});
        meterGlow3->setPosition({474 + 5, 167});

        meterBack0->setPosition({450 + 5, 167});
        meterBack1->setPosition({458 + 5, 167});
        meterBack2->setPosition({466 + 5, 167});
        meterBack3->setPosition({474 + 5, 167});
        meterBack0->setScale(1.35f);
        meterBack1->setScale(1.35f);
        meterBack2->setScale(1.35f);
        meterBack3->setScale(1.35f);

        // set some opacities to 0
        meterCounter0->setOpacity(0);
        meterCounter1->setOpacity(0);
        meterCounter2->setOpacity(0);
        meterCounter3->setOpacity(0);
        meterGlow0->setOpacity(0);
        meterGlow1->setOpacity(0);
        meterGlow2->setOpacity(0);
        meterGlow3->setOpacity(0);

        // make glows small as shit
        meterGlow0->setScale(0.1f);
        meterGlow1->setScale(0.1f);
        meterGlow2->setScale(0.1f);
        meterGlow3->setScale(0.1f);

        // make background blink forever
        meterBack0->runAction(CCRepeatForever::create(blinkAnim));
        meterBack1->runAction(CCRepeatForever::create(blinkAnim2));
        meterBack2->runAction(CCRepeatForever::create(blinkAnim3));
        meterBack3->runAction(CCRepeatForever::create(blinkAnim4));

        // add all to PlayLayer
        this->addChild(meterNode);
        meterNode->addChild(meterUnder, 1);
        meterNode->addChild(meterSpd, 1);
        meterNode->addChild(meterCounter0, 1);
        meterNode->addChild(meterCounter1, 1);
        meterNode->addChild(meterCounter2, 1);
        meterNode->addChild(meterCounter3, 1);
        meterNode->addChild(meterGlow0, 2);
        meterNode->addChild(meterGlow1, 2);
        meterNode->addChild(meterGlow2, 2);
        meterNode->addChild(meterGlow3, 2);
        meterNode->addChild(meterBack0, 0);
        meterNode->addChild(meterBack1, 0);
        meterNode->addChild(meterBack2, 0);
        meterNode->addChild(meterBack3, 0);

        return true;
    }

    void updateProgressbar() {
        PlayLayer::updateProgressbar();

        auto baseLayer = GJBaseGameLayer::get();
        if (baseLayer->m_isPlatformer) return;
        auto winSize = CCDirector::sharedDirector()->getWinSize();

        float meterNodeEndX = winSize.width - 569.0f;

        int currentProgress = this->getCurrentPercentInt();
        bool validPercent = currentProgress == 20 || currentProgress == 50 || currentProgress == 80;
        auto fields = m_fields.self();
        auto fmod = FMODAudioEngine::sharedEngine();

        // get sprites for use
        auto meterMainNode = this->getChildByID("unleashed-speedmeter"_spr);

        if (meterMainNode) {

            auto speedMeterUnder = meterMainNode->getChildByID("speed-meter-under"_spr);
            auto speedMeterSpd = meterMainNode->getChildByID("speed-meter-spd"_spr);
            auto speedMeterCounter0 = meterMainNode->getChildByID("speed-meter-counter0"_spr);
            auto speedMeterCounter1 = meterMainNode->getChildByID("speed-meter-counter1"_spr);
            auto speedMeterCounter2 = meterMainNode->getChildByID("speed-meter-counter2"_spr);
            auto speedMeterCounter3 = meterMainNode->getChildByID("speed-meter-counter3"_spr);

            auto meterCounterBack0 = meterMainNode->getChildByID("speed-meter-back0"_spr);
            auto meterCounterBack1 = meterMainNode->getChildByID("speed-meter-back1"_spr);
            auto meterCounterBack2 = meterMainNode->getChildByID("speed-meter-back2"_spr);
            auto meterCounterBack3 = meterMainNode->getChildByID("speed-meter-back3"_spr);

            auto meterCounterGlow0 = meterMainNode->getChildByID("speed-meter-glow0"_spr);
            auto meterCounterGlow1 = meterMainNode->getChildByID("speed-meter-glow1"_spr);
            auto meterCounterGlow2 = meterMainNode->getChildByID("speed-meter-glow2"_spr);
            auto meterCounterGlow3 = meterMainNode->getChildByID("speed-meter-glow3"_spr);

            if (!speedMeterUnder || !speedMeterSpd || !speedMeterCounter0 || !speedMeterCounter1 || !speedMeterCounter2 || !speedMeterCounter3) {
                geode::log::error("Failed to fetch one or more speedmeter sprites");
                return;
            }

            // slide in anim for everything
            auto meterInAnim1 = CCSequence::create(
            CCMoveTo::create(0.15f, {meterNodeEndX, 0.f}), // Slide to dynamic end position
            nullptr
            );

            auto meterOffAnim1 = CCSequence::create(
                CCDelayTime::create(fields->meterOffDelay),
                CCMoveTo::create(0.15f, {winSize.width - 269.0f, 0.f}), // Slide back to dynamic start position
                nullptr
            );

            // reset the stuff that needs to be reset
            auto resetGlow1 = CCSequence::create(
                CCDelayTime::create(fields->meterOffDelay + 0.15f),
                CCScaleTo::create(0.f, 0.1f),
                nullptr
            );
            auto resetGlow2 = CCSequence::create(
                CCDelayTime::create(fields->meterOffDelay + 0.15f),
                CCScaleTo::create(0.f, 0.1f),
                nullptr
            );
            auto resetGlow3 = CCSequence::create(
                CCDelayTime::create(fields->meterOffDelay + 0.15f),
                CCScaleTo::create(0.f, 0.1f),
                nullptr
            );
            auto resetGlow4 = CCSequence::create(
                CCDelayTime::create(fields->meterOffDelay + 0.15f),
                CCScaleTo::create(0.f, 0.1f),
                nullptr
            );
            auto resetCounter1 = CCSequence::create(
                CCDelayTime::create(fields->meterOffDelay + 0.15f),
                CCFadeTo::create(0.f, 0),
                nullptr
            );
            auto resetCounter2 = CCSequence::create(
                CCDelayTime::create(fields->meterOffDelay + 0.15f),
                CCFadeTo::create(0.f, 0),
                nullptr
            );
            auto resetCounter3 = CCSequence::create(
                CCDelayTime::create(fields->meterOffDelay + 0.15f),
                CCFadeTo::create(0.f, 0),
                nullptr
            );
            auto resetCounter4 = CCSequence::create(
                CCDelayTime::create(fields->meterOffDelay + 0.15f),
                CCFadeTo::create(0.f, 0),
                nullptr
            );


            if (validPercent) {
                if (fields->progressLogged.find(currentProgress) == fields->progressLogged.end() || !fields->progressLogged[currentProgress]) {

                    fmod->playEffect("speedmeter_1.ogg"_spr);

                    // run slide in anim
                    meterMainNode->runAction(meterInAnim1);

                    // make actions per speed value

                    // FIRST COUNTER (Group 1)
                    auto scaleGlowAnim1 = CCSequence::create(
                        CCDelayTime::create(1.23f),             // Group 1 start at 1.23s
                        CCFadeTo::create(0.f, 255),
                        CCEaseOut::create(CCScaleTo::create(0.2f, 2.0f), 3.0f),          // Scale lasts 0.2s (1.23s -> 1.43s)
                        nullptr
                    );
                    auto fadeOutGlow1 = CCSequence::create(
                        CCDelayTime::create(1.38f),              // Fade starts at 1.23 + 0.15 = 1.38s
                        CCFadeTo::create(0.05f, 0),          // Fade lasts 0.05s (ends at 1.43s)
                        nullptr
                    );
                    auto showMeter1 = CCSequence::create(
                        CCDelayTime::create(1.38f),              // Sprite shown at 1.38s (SFX played too)
                        CCFadeTo::create(0.03f, 255),
                        nullptr
                    );

                    // SECOND COUNTER (Group 2)
                    auto scaleGlowAnim2 = CCSequence::create(
                        CCDelayTime::create(1.53f),             // Group 2 starts at 1.53s
                        CCFadeTo::create(0.f, 255),
                        CCEaseOut::create(CCScaleTo::create(0.2f, 2.0f), 3.0f),          // Scale lasts 0.2s (1.53s -> 1.73s)
                        nullptr
                    );
                    auto fadeOutGlow2 = CCSequence::create(
                        CCDelayTime::create(1.68f),              // 1.53 + 0.15 = 1.68s
                        CCFadeTo::create(0.05f, 0),
                        nullptr
                    );
                    auto showMeter2 = CCSequence::create(
                        CCDelayTime::create(1.68f),
                        CCFadeTo::create(0.03f, 255),
                        nullptr
                    );

                    // THIRD COUNTER (Group 3)
                    auto scaleGlowAnim3 = CCSequence::create(
                        CCDelayTime::create(1.83f),             // Group 3 starts at 1.83s
                        CCFadeTo::create(0.f, 255),
                        CCEaseOut::create(CCScaleTo::create(0.2f, 2.0f), 3.0f),          // Scale lasts 0.2s (1.83s -> 2.03s)
                        nullptr
                    );
                    auto fadeOutGlow3 = CCSequence::create(
                        CCDelayTime::create(1.98f),              // 1.83 + 0.15 = 1.98s
                        CCFadeTo::create(0.05f, 0),
                        nullptr
                    );
                    auto showMeter3 = CCSequence::create(
                        CCDelayTime::create(1.98f),
                        CCFadeTo::create(0.03f, 255),
                        nullptr
                    );

                    // FOURTH COUNTER (Group 4)
                    auto scaleGlowAnim4 = CCSequence::create(
                        CCDelayTime::create(2.13f),             // Group 4 starts at 2.13s
                        CCFadeTo::create(0.f, 255),
                        CCEaseOut::create(CCScaleTo::create(0.2f, 2.0f), 3.0f),          // Scale lasts 0.2s (2.13s -> 2.33s)
                        nullptr
                    );
                    auto fadeOutGlow4 = CCSequence::create(
                        CCDelayTime::create(2.28f),              // 2.13 + 0.15 = 2.28s
                        CCFadeTo::create(0.05f, 0),
                        nullptr
                    );
                    auto showMeter4 = CCSequence::create(
                        CCDelayTime::create(2.28f),
                        CCFadeTo::create(0.03f, 255),
                        nullptr
                    );

                    // run the visuals according to player speeds, 0.5x to 4x
                    if (playerSpeed == 0.7f || playerSpeed == 0.9f){
                        // 0.5x & 1x speed, show only 1 counter
                        // fields->meterOffDelay = 2.33f;

                        if (fakeCheckpoint) {
                            fmod->playEffect("triggerCheckpoint.ogg"_spr);
                        }

                        meterCounterGlow3->runAction(scaleGlowAnim1);
                        meterCounterGlow3->runAction(fadeOutGlow1);
                        speedMeterCounter3->runAction(showMeter1);
                        if (playMeterSFX) {
                            this->scheduleOnce(schedule_selector(SonicUnleashed::timerCheck), 1.38f);
                            this->scheduleOnce(schedule_selector(SonicUnleashed::timerEnd), 1.53f);
                        }

                    } else if (playerSpeed == 1.1f) {
                        // 2x speed, show only 2 counters
                        // fields->meterOffDelay = 2.63f;

                        if (fakeCheckpoint) {
                            fmod->playEffect("triggerCheckpoint.ogg"_spr);
                        }

                        meterCounterGlow3->runAction(scaleGlowAnim1);
                        meterCounterGlow3->runAction(fadeOutGlow1);
                        speedMeterCounter3->runAction(showMeter1);

                        meterCounterGlow2->runAction(scaleGlowAnim2);
                        meterCounterGlow2->runAction(fadeOutGlow2);
                        speedMeterCounter2->runAction(showMeter2);

                        if (playMeterSFX) {
                            this->scheduleOnce(schedule_selector(SonicUnleashed::timerCheck), 1.38f);
                            this->scheduleOnce(schedule_selector(SonicUnleashed::timerCheck2), 1.68f);
                            this->scheduleOnce(schedule_selector(SonicUnleashed::timerEnd), 1.83f);
                        }

                    } else if (playerSpeed == 1.3f) {
                        // 3x speed, show only 3 counters
                        // fields->meterOffDelay = 2.93f;

                        if (fakeCheckpoint) {
                            fmod->playEffect("triggerCheckpoint.ogg"_spr);
                        }

                        meterCounterGlow3->runAction(scaleGlowAnim1);
                        meterCounterGlow3->runAction(fadeOutGlow1);
                        speedMeterCounter3->runAction(showMeter1);

                        meterCounterGlow2->runAction(scaleGlowAnim2);
                        meterCounterGlow2->runAction(fadeOutGlow2);
                        speedMeterCounter2->runAction(showMeter2);

                        meterCounterGlow1->runAction(scaleGlowAnim3);
                        meterCounterGlow1->runAction(fadeOutGlow3);
                        speedMeterCounter1->runAction(showMeter3);

                        if (playMeterSFX) {
                            this->scheduleOnce(schedule_selector(SonicUnleashed::timerCheck), 1.38f);
                            this->scheduleOnce(schedule_selector(SonicUnleashed::timerCheck2), 1.68f);
                            this->scheduleOnce(schedule_selector(SonicUnleashed::timerCheck3), 1.98f);
                            this->scheduleOnce(schedule_selector(SonicUnleashed::timerEnd), 2.13f);
                        }

                    } else if (playerSpeed == 1.6f) {
                        // 4x speed, show all 4 counters
                        // fields->meterOffDelay = 3.23f;

                        if (fakeCheckpoint) {
                            fmod->playEffect("triggerCheckpoint.ogg"_spr);
                        }

                        meterCounterGlow3->runAction(scaleGlowAnim1);
                        meterCounterGlow3->runAction(fadeOutGlow1);
                        speedMeterCounter3->runAction(showMeter1);

                        meterCounterGlow2->runAction(scaleGlowAnim2);
                        meterCounterGlow2->runAction(fadeOutGlow2);
                        speedMeterCounter2->runAction(showMeter2);

                        meterCounterGlow1->runAction(scaleGlowAnim3);
                        meterCounterGlow1->runAction(fadeOutGlow3);
                        speedMeterCounter1->runAction(showMeter3);

                        meterCounterGlow0->runAction(scaleGlowAnim4);
                        meterCounterGlow0->runAction(fadeOutGlow4);
                        speedMeterCounter0->runAction(showMeter4);

                        if (playMeterSFX) {
                            this->scheduleOnce(schedule_selector(SonicUnleashed::timerCheck), 1.38f);
                            this->scheduleOnce(schedule_selector(SonicUnleashed::timerCheck2), 1.68f);
                            this->scheduleOnce(schedule_selector(SonicUnleashed::timerCheck3), 1.98f);
                            this->scheduleOnce(schedule_selector(SonicUnleashed::timerCheck4), 2.28f);
                            this->scheduleOnce(schedule_selector(SonicUnleashed::timerEnd), 2.43f);
                        }
                    }

                    // run slide off anim
                    meterMainNode->runAction(meterOffAnim1);

                    // reset the glow and counter sprites
                    meterCounterGlow0->runAction(resetGlow1);
                    meterCounterGlow1->runAction(resetGlow2);
                    meterCounterGlow2->runAction(resetGlow3);
                    meterCounterGlow3->runAction(resetGlow4);
                    speedMeterCounter0->runAction(resetCounter1);
                    speedMeterCounter1->runAction(resetCounter2);
                    speedMeterCounter2->runAction(resetCounter3);
                    speedMeterCounter3->runAction(resetCounter4);
                    

                    fields->progressLogged[currentProgress] = true;
                }
            } else {
                // Reset the flags for all tracked percentages if the progress is not at the specific percentages
                fields->progressLogged[20] = false;
                fields->progressLogged[50] = false;
                fields->progressLogged[80] = false;
            }
        }
    }
};