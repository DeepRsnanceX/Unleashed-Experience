#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/EndLevelLayer.hpp>
#include <Geode/binding/GJGameLevel.hpp>
#include <Geode/binding/GJBaseGameLayer.hpp>
#include <Geode/loader/SettingV3.hpp>
#include "helper.hpp"
#include <random>

using namespace geode::prelude;

int getRandomInt(int min, int max) {
    std::random_device rd;  // Obtain a random number from hardware
    std::mt19937 gen(rd()); // Seed the generator
    std::uniform_int_distribution<> distr(min, max); // Define the range

    return distr(gen);
}

class $modify(PlayLayer) {
    struct Fields {
        CCSprite* whiteFlashOverlay = CCSprite::createWithSpriteFrameName("white_overlay.png"_spr);
    };
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        auto f = m_fields.self();
        
        f->whiteFlashOverlay->setZOrder(1000);
        f->whiteFlashOverlay->setScale(10.0f);
        f->whiteFlashOverlay->setOpacity(0);

        this->addChild(f->whiteFlashOverlay);

        return true;
    }

    void levelComplete() {
        PlayLayer::levelComplete();

        auto f = m_fields.self();

        auto flashbang = CCSequence::create(
            CCFadeIn::create(0.23f),
            CCDelayTime::create(0.52f),
            CCFadeOut::create(0.23f),
            nullptr
        );
        f->whiteFlashOverlay->runAction(flashbang);
    }
};

class $modify(EndLevelLayer) {
    struct Fields {
        CCNode* rankingScreenNode = CCNode::create();
        // -----------------------
        // RANKING SCREEN ELEMENTS
        // -----------------------
        // --- big results bar
        CCNode* resultsNode = CCNode::create();
        CCSprite* resultsBar = CCSprite::createWithSpriteFrameName("hudBar_huge.png"_spr);
        CCSprite* resultsBarOverlay = CCSprite::createWithSpriteFrameName("hudBar_hugeOverlay.png"_spr);
        // --- data bars: TIME, ATTEMPTS, JUMPS, SPEED
        CCSprite* timerBar = CCSprite::createWithSpriteFrameName("resultsBar.png"_spr);
        CCSprite* timerBarOverlay = CCSprite::createWithSpriteFrameName("resultsBarOverlay.png"_spr);
        CCNode* timerBarNode = CCNode::create();
        CCSprite* attemptsBar = CCSprite::createWithSpriteFrameName("resultsBar.png"_spr);
        CCSprite* attemptsBarOverlay = CCSprite::createWithSpriteFrameName("resultsBarOverlay.png"_spr);
        CCNode* attemptsBarNode = CCNode::create();
        CCSprite* jumpsBar = CCSprite::createWithSpriteFrameName("resultsBar.png"_spr);
        CCSprite* jumpsBarOverlay = CCSprite::createWithSpriteFrameName("resultsBarOverlay.png"_spr);
        CCNode* jumpsBarNode = CCNode::create();
        CCSprite* speedBar = CCSprite::createWithSpriteFrameName("resultsBar.png"_spr);
        CCSprite* speedBarOverlay = CCSprite::createWithSpriteFrameName("resultsBarOverlay.png"_spr);
        CCNode* speedBarNode = CCNode::create();
        // --- title bars for all the last ones
        CCNode* timeTitleNode = CCNode::create();
        CCSprite* timeTitle = CCSprite::createWithSpriteFrameName("ranking_titleBar.png"_spr);
        CCSprite* timeTitleOverlay = CCSprite::createWithSpriteFrameName("ranking_titleBarOverlay.png"_spr);
        CCNode* attemptsTitleNode = CCNode::create();
        CCSprite* attemptsTitle = CCSprite::createWithSpriteFrameName("ranking_titleBar.png"_spr);
        CCSprite* attemptsTitleOverlay = CCSprite::createWithSpriteFrameName("ranking_titleBarOverlay.png"_spr);
        CCNode* jumpsTitleNode = CCNode::create();
        CCSprite* jumpsTitle = CCSprite::createWithSpriteFrameName("ranking_titleBar.png"_spr);
        CCSprite* jumpsTitleOverlay = CCSprite::createWithSpriteFrameName("ranking_titleBarOverlay.png"_spr);
        CCNode* speedTitleNode = CCNode::create();
        CCSprite* speedTitle = CCSprite::createWithSpriteFrameName("ranking_titleBar.png"_spr);
        CCSprite* speedTitleOverlay = CCSprite::createWithSpriteFrameName("ranking_titleBarOverlay.png"_spr);
    };

    void customSetup() {
        EndLevelLayer::customSetup();

        auto f = m_fields.self();
        auto rightBarsColor = ccc3(0x3c, 0x4e, 0x86);
        auto titleBarsColor = ccc3(0x3E, 0x44, 0x62);
        auto resultsBarColor = ccc3(0x3B, 0x56, 0x8E);
        auto totalBarColor = ccc3(0x31, 0x95, 0x80);
        auto totalTitleColor = ccc3(0x2F, 0xA3, 0x6A);

        // general setup for all elements
        f->timerBar->setColor(rightBarsColor);
        f->timerBarOverlay->setOpacity(50);
        f->timerBarNode->setScale(1.15f);
        f->timerBarNode->setPosition({515.0f + 350, 226.0f});
        f->timerBarNode->setID("time-bar"_spr);

        f->attemptsBar->setColor(rightBarsColor);
        f->attemptsBarOverlay->setOpacity(50);
        f->attemptsBarNode->setScale(1.15f);
        f->attemptsBarNode->setPosition({515.0f + 350, 196.0f});
        f->attemptsBarNode->setID("attempts-bar"_spr);

        f->jumpsBar->setColor(rightBarsColor);
        f->jumpsBarOverlay->setOpacity(50);
        f->jumpsBarNode->setScale(1.15f);
        f->jumpsBarNode->setPosition({515.0f + 350, 166.0f});
        f->jumpsBarNode->setID("jumps-bar"_spr);

        f->speedBar->setColor(rightBarsColor);
        f->speedBarOverlay->setOpacity(50);
        f->speedBarNode->setScale(1.15f);
        f->speedBarNode->setPosition({515.0f + 350, 136.0f});
        f->speedBarNode->setID("speed-bar"_spr);

        f->resultsBar->setColor(resultsBarColor);
        f->resultsBarOverlay->setOpacity(90);
        f->resultsNode->setScale(1.05f);
        f->resultsNode->setPosition({-235.0f - 300, 278.0f});
        f->resultsNode->setID("results-bar"_spr);

        // title bars setup
        f->timeTitle->setColor(titleBarsColor);
        f->timeTitleOverlay->setOpacity(50);
        f->timeTitleNode->setScale(0.825f);
        f->timeTitleNode->addChild(f->timeTitle);
        f->timeTitleNode->addChild(f->timeTitleOverlay);
        f->timeTitleNode->setID("time-title"_spr);

        f->attemptsTitle->setColor(titleBarsColor);
        f->attemptsTitleOverlay->setOpacity(50);
        f->attemptsTitleNode->setScale(0.825f);
        f->attemptsTitleNode->addChild(f->attemptsTitle);
        f->attemptsTitleNode->addChild(f->attemptsTitleOverlay);
        f->attemptsTitleNode->setID("atts-title"_spr);

        f->jumpsTitle->setColor(titleBarsColor);
        f->jumpsTitleOverlay->setOpacity(50);
        f->jumpsTitleNode->setScale(0.825f);
        f->jumpsTitleNode->addChild(f->jumpsTitle);
        f->jumpsTitleNode->addChild(f->jumpsTitleOverlay);
        f->jumpsTitleNode->setID("jumps-title"_spr);

        f->speedTitle->setColor(titleBarsColor);
        f->speedTitleOverlay->setOpacity(50);
        f->speedTitleNode->setScale(0.825f);
        f->speedTitleNode->addChild(f->speedTitle);
        f->speedTitleNode->addChild(f->speedTitleOverlay);
        f->speedTitleNode->setID("speed-title"_spr);

        // ID setting and add them to their nodes
        f->timerBarNode->addChild(f->timerBar);
        f->timerBarNode->addChild(f->timerBarOverlay);
        f->attemptsBarNode->addChild(f->attemptsBar);
        f->attemptsBarNode->addChild(f->attemptsBarOverlay);
        f->jumpsBarNode->addChild(f->jumpsBar);
        f->jumpsBarNode->addChild(f->jumpsBarOverlay);
        f->speedBarNode->addChild(f->speedBar);
        f->speedBarNode->addChild(f->speedBarOverlay);
        f->resultsNode->addChild(f->resultsBar);
        f->resultsNode->addChild(f->resultsBarOverlay);
        
        // add elements to main node, add main node to layer
        f->rankingScreenNode->addChild(f->timerBarNode);
        f->rankingScreenNode->addChild(f->attemptsBarNode);
        f->rankingScreenNode->addChild(f->jumpsBarNode);
        f->rankingScreenNode->addChild(f->speedBarNode);
        f->rankingScreenNode->addChild(f->resultsNode);
        f->rankingScreenNode->addChild(f->timeTitleNode);
        f->rankingScreenNode->addChild(f->attemptsTitleNode);
        f->rankingScreenNode->addChild(f->jumpsTitleNode);
        f->rankingScreenNode->addChild(f->speedTitleNode);
        f->rankingScreenNode->setZOrder(100);
        this->addChild(f->rankingScreenNode);

        // guides
        auto imageGuide = CCSprite::createWithSpriteFrameName("imageGuide.png"_spr);
        auto guideTitleStart = CCSprite::createWithSpriteFrameName("guideTitlestart.png"_spr);
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        imageGuide->setPosition({winSize.width / 2, winSize.height / 2});
        imageGuide->setZOrder(-10);
        imageGuide->setScale(1.78);
        imageGuide->setID("guide-main"_spr);
        guideTitleStart->setPosition({winSize.width / 2, winSize.height / 2});
        guideTitleStart->setZOrder(-11);
        guideTitleStart->setID("guide-tstart"_spr);
        guideTitleStart->setScale(1.78);
        this->addChild(imageGuide);
        this->addChild(guideTitleStart);

    }
};