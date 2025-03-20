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
        // --- ranking sprite
        CCSprite* rankingSprite = CCSprite::createWithSpriteFrameName("rank_placeholder.png"_spr);
        // --- big results bar
        CCSprite* resultsBar = CCSprite::createWithSpriteFrameName("topResultsBar.png"_spr);
        // --- data bars: TIME, ATTEMPTS, JUMPS, SPEED
        CCSprite* timerBar = CCSprite::createWithSpriteFrameName("resultsBar.png"_spr);
        CCSprite* attemptsBar = CCSprite::createWithSpriteFrameName("resultsBar.png"_spr);
        CCSprite* jumpsBar = CCSprite::createWithSpriteFrameName("resultsBar.png"_spr);
        CCSprite* coinsBar = CCSprite::createWithSpriteFrameName("resultsBar.png"_spr);
        // --- title bars for all the last ones
        CCSprite* timeTitle = CCSprite::createWithSpriteFrameName("endTimeTitle.png"_spr);
        CCSprite* attemptsTitle = CCSprite::createWithSpriteFrameName("endAttTitle.png"_spr);
        CCSprite* jumpsTitle = CCSprite::createWithSpriteFrameName("endJumpsTitle.png"_spr);
        CCSprite* coinsTitle = CCSprite::createWithSpriteFrameName("endCoinsTitle.png"_spr);
        // --- total bar and title
        CCSprite* totalBar = CCSprite::createWithSpriteFrameName("totalBar.png"_spr);
        CCSprite* totalTitle = CCSprite::createWithSpriteFrameName("totalTitle.png"_spr);
        // --- rank text
        CCSprite* rankText = CCSprite::createWithSpriteFrameName("rankText.png"_spr);
        // --- data labels
        CCLabelBMFont* timeLabel = CCLabelBMFont::create("00:00:00", "unleashedHUDFont.fnt"_spr);
        CCLabelBMFont* attemptsLabel = CCLabelBMFont::create("00000", "unleashedHUDFont.fnt"_spr);
        CCLabelBMFont* jumpsLabel = CCLabelBMFont::create("00000", "unleashedHUDFont.fnt"_spr);
        CCLabelBMFont* coinsLabel = CCLabelBMFont::create("0/0", "unleashedHUDFont.fnt"_spr);
    };

    void customSetup() {
        EndLevelLayer::customSetup();

        auto f = m_fields.self();
        auto fmod = FMODAudioEngine::sharedEngine();

        // --------------------------------------------------------
        // RANKING SPRITE SETUP
        // --------------------------------------------------------

        int attempts = m_playLayer->m_attempts;

        f->rankingSprite->setOpacity(0);
        f->rankingSprite->setZOrder(10);
        f->rankingSprite->setPosition({50, 50});
        f->rankingSprite->setID("rank-sprite"_spr);
        f->rankingSprite->setScale(9.0f);

        bool rankS = attempts == 1;
        bool rankA = attempts == 2 || attempts == 3;
        bool rankB = attempts >= 4 && attempts <= 6;
        bool rankC = attempts >= 7 && attempts <= 10;
        bool rankD = attempts >= 11 && attempts <= 15;
        bool rankE = attempts > 16;

        if (rankS) {
            f->rankingSprite->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("rank_s.png"_spr));
            fmod->playEffect("rankS.ogg"_spr);
        } else if (rankA) {
            f->rankingSprite->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("rank_a.png"_spr));
            fmod->playEffect("rankA.ogg"_spr);
        } else if (rankB) {
            f->rankingSprite->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("rank_b.png"_spr));
            fmod->playEffect("rankC.ogg"_spr);
        } else if (rankC) {
            f->rankingSprite->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("rank_c.png"_spr));
            fmod->playEffect("rankC.ogg"_spr);
        } else if (rankD) {
            f->rankingSprite->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("rank_d.png"_spr));
            fmod->playEffect("rankC.ogg"_spr);
        } else if (rankE) {
            f->rankingSprite->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("rank_e.png"_spr));
            fmod->playEffect("rankE.ogg"_spr);
        }

        // ----------------
        // RIGHT SIDE BARS
        // ----------------
        f->timerBar->setScale(1.775f);
        f->timerBar->setPosition({505.0f + 350, 226.0f});
        f->timerBar->setID("time-bar"_spr);

        f->attemptsBar->setScale(1.775f);
        f->attemptsBar->setPosition({505.0f + 350, 196.0f});
        f->attemptsBar->setID("attempts-bar"_spr);

        f->jumpsBar->setScale(1.775f);
        f->jumpsBar->setPosition({505.0f + 350, 166.0f});
        f->jumpsBar->setID("jumps-bar"_spr);

        f->coinsBar->setScale(1.775f);
        f->coinsBar->setPosition({505.0f + 350, 136.0f});
        f->coinsBar->setID("coins-bar"_spr);

        // ----------------
        // TOP RESULTS BAR
        // ----------------
        f->resultsBar->setScale(1.75f);
        f->resultsBar->setPosition({26.0f - 300, 279.0f});
        f->resultsBar->setID("results-bar"_spr);

        // ----------------
        // TITLE BARS
        // ----------------
        f->timeTitle->setScale(1.775f);
        f->timeTitle->setID("time-title"_spr);
        f->timeTitle->setPosition({322.0f, 233.0f});
        // starting pos = 328.0f, 239f
        // move by - X: -6, Y: -6

        f->attemptsTitle->setScale(1.775f);
        f->attemptsTitle->setID("atts-title"_spr);
        f->attemptsTitle->setPosition({322.0f, 203.0f});

        f->jumpsTitle->setScale(1.775f);
        f->jumpsTitle->setID("jumps-title"_spr);
        f->jumpsTitle->setPosition({322.0f, 173.0f});

        f->coinsTitle->setScale(1.775f);
        f->coinsTitle->setID("coins-title"_spr);
        f->coinsTitle->setPosition({322.0f, 143.0f});

        // ----------------
        // LOWER TOTAL SECTION
        // ----------------
        f->totalTitle->setScale(1.775f);
        f->totalTitle->setID("total-title"_spr);
        f->totalTitle->setPosition({321.0f, 84.5f});

        f->totalBar->setScale(1.775f);
        f->totalBar->setPosition({36.0f - 350, 77.0f});

        // ----------------
        // DATA LABELS
        // ----------------
        auto baselayer = GJBaseGameLayer::get();
        auto playlayer = PlayLayer::get();
        float attTime = baselayer->m_gameState.m_levelTime;
        int mins = static_cast<int>(attTime) / 60;
        int secs = static_cast<int>(attTime) % 60;
        int millis = static_cast<int>((attTime - static_cast<int>(attTime)) * 100);
        std::string levelTimeText = fmt::format("{:02}:{:02}:{:02}", mins, secs, millis);
        std::string attemptsText = fmt::format("{:03}", playlayer->m_attempts);
        std::string jumpsText = fmt::format("{:06}", playlayer->m_jumps);

        f->timeLabel->setString(levelTimeText.c_str(), true);
        f->timeLabel->setID("time-label"_spr);
        f->timeLabel->setAnchorPoint({1.0f, 0.5f});
        f->timeLabel->setScaleX(0.77f);
        f->timeLabel->setScaleY(0.65f);
        f->timeLabel->setPosition({453.0f, 227.5f});
        f->timeLabel->setAlignment(kCCTextAlignmentRight);

        f->attemptsLabel->setString(attemptsText.c_str(), true);
        f->attemptsLabel->setID("attempts-label"_spr);
        f->attemptsLabel->setAnchorPoint({1.0f, 0.5f});
        f->attemptsLabel->setPosition({453.0f, 197.5f});
        f->attemptsLabel->setScaleX(0.77f);
        f->attemptsLabel->setScaleY(0.65f);
        f->attemptsLabel->setAlignment(kCCTextAlignmentRight);
        
        f->jumpsLabel->setString(jumpsText.c_str(), true);
        f->jumpsLabel->setID("jumps-label"_spr);
        f->jumpsLabel->setAnchorPoint({1.0f, 0.5f});
        f->jumpsLabel->setPosition({453.0f, 167.5f});
        f->jumpsLabel->setScaleX(0.77f);
        f->jumpsLabel->setScaleY(0.65f);
        f->jumpsLabel->setAlignment(kCCTextAlignmentRight);

        // ----------------
        // LITERALLY JUST THE RANK TEXT
        // ----------------
        f->rankText->setPosition({152.0f, 78.5f});
        f->rankText->setID("rank-text"_spr);
        f->rankText->setOpacity(0);
        f->rankText->setAnchorPoint({1.0f, 0.5f});
        // will scale up to 1.9f
        
        // ----------------
        // ADD TO MAIN NODE
        // ----------------
        f->rankingScreenNode->addChild(f->timerBar);
        f->rankingScreenNode->addChild(f->attemptsBar);
        f->rankingScreenNode->addChild(f->jumpsBar);
        f->rankingScreenNode->addChild(f->coinsBar);
        f->rankingScreenNode->addChild(f->resultsBar);
        f->rankingScreenNode->addChild(f->timeTitle);
        f->rankingScreenNode->addChild(f->attemptsTitle);
        f->rankingScreenNode->addChild(f->jumpsTitle);
        f->rankingScreenNode->addChild(f->coinsTitle);
        f->rankingScreenNode->addChild(f->totalBar);
        f->rankingScreenNode->addChild(f->totalTitle);
        f->rankingScreenNode->addChild(f->rankText);
        f->rankingScreenNode->addChild(f->timeLabel);
        f->rankingScreenNode->addChild(f->attemptsLabel);
        f->rankingScreenNode->addChild(f->jumpsLabel);
        f->rankingScreenNode->addChild(f->rankingSprite);
        f->rankingScreenNode->setZOrder(100);
        this->addChild(f->rankingScreenNode);

        // ----------------
        // GUIDES (REMOVE LATER)
        // ----------------
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

    void showLayer(bool p0) {
        EndLevelLayer::showLayer(p0);

        auto literallyTheEndscreen = this->getChildByID("main-layer");
        literallyTheEndscreen->stopAllActions();
        literallyTheEndscreen->setVisible(false);

        // animations
        auto fadeIn = CCFadeIn::create(0.3f);
        auto scaleDown = CCScaleTo::create(0.3, 1.75f);

        ranking->runAction(fadeIn);
        ranking->runAction(scaleDown);

        // -----------------------------------------------
        // RANKING SCREEN ANIMATIONS
        // GOD SAVE ME
        // -----------------------------------------------
    }
};