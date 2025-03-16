#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/EndLevelLayer.hpp>
#include <Geode/binding/GJGameLevel.hpp>
#include <Geode/binding/GJBaseGameLayer.hpp>
#include <Geode/loader/SettingV3.hpp>
#include "helper.hpp"
#include <random>

using namespace geode::prelude;

bool unleashedSounds = true;
bool unleashedRankings = true;

int genRandomInt(int min, int max) {
    std::random_device rd;  // Obtain a random number from hardware
    std::mt19937 gen(rd()); // Seed the generator
    std::uniform_int_distribution<> distr(min, max); // Define the range

    return distr(gen);
}

class $modify(PlayerObject){
    void startDashing(DashRingObject* p0) {
        PlayerObject::startDashing(p0);

        auto fmod = FMODAudioEngine::sharedEngine();

        int randomBoost = genRandomInt(1, 7);
        auto sfxToPlayBoost = fmt::format("boost_{}.ogg"_spr, randomBoost);

        // Generate another random int to decide whether to play the sound
        int playSound = genRandomInt(1, 10);

        if (playSound > 7) {
            fmod->playEffect(sfxToPlayBoost);
        }

    }

    void incrementJumps() {
        PlayerObject::incrementJumps();

        auto fmod = FMODAudioEngine::sharedEngine();
        bool m_isCube = !m_isShip && !m_isBird && !m_isBall && !m_isDart && !m_isRobot && !m_isSpider && !m_isSwing;

        // Check for unleashedSounds and play a random jump sound
        int randomJump = genRandomInt(1, 5);
        auto sfxToPlayRandomJump = fmt::format("jump_{}.ogg"_spr, randomJump);

        // Generate another random int to decide whether to play the sound
        int playSound = genRandomInt(1, 10);

        if (playSound > 5) {
            if (m_isRobot || m_isCube && !m_ringJumpRelated && PlayLayer::get()) {
                fmod->playEffect(sfxToPlayRandomJump);
            }
        }

    }

    void updateTimeMod(float p0, bool p1) {
        
        PlayerObject::updateTimeMod(p0, p1);

        if (PlayLayer::get()) {
            auto fmod = FMODAudioEngine::sharedEngine();
            int randomBoost = genRandomInt(1, 7);
            auto sfxToPlayBoost = fmt::format("boost_{}.ogg"_spr, randomBoost);

            int doPlaySound = genRandomInt(1, 10);

            if (p0 >= 1.5f) {
                fmod->playEffect("boost_fullsfx.ogg"_spr);
                if (doPlaySound >= 3) {
                    fmod->playEffect(sfxToPlayBoost);
                }
            }
        }
    }
};

class $modify(PlayLayer) {
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        auto fmod = FMODAudioEngine::sharedEngine();
        fmod->playEffect("herewe.ogg"_spr);

        auto winSize = CCDirector::sharedDirector()->getWinSize();
        auto goBackSprite = CCSprite::createWithSpriteFrameName("go_backsprite.png"_spr);
        auto goMainSprite = CCSprite::createWithSpriteFrameName("go_mainsprite.png"_spr);

        goBackSprite->setOpacity(0);
        goMainSprite->setOpacity(0);

        // center both sprites
        goBackSprite->setPosition({winSize.width / 2, winSize.height / 2});
        goMainSprite->setPosition({winSize.width / 2, winSize.height / 2});

        goBackSprite->setScale(1.75f);
        goMainSprite->setScale(5.0f);

        // IDs for fetching
        goBackSprite->setID("go-back-sprite"_spr);
        goMainSprite->setID("go-main-sprite"_spr);

        goBackSprite->setZOrder(100);
        goMainSprite->setZOrder(100);

        this->addChild(goBackSprite);
        this->addChild(goMainSprite);

        // ---------------------------------
        // UNLEASHED HUD
        // ---------------------------------

        // unleashed HUD node 
        auto UnleashedHUD = CCNode::create();
        UnleashedHUD->setID("UnleashedHUD");
        UnleashedHUD->setScale(0.65f);
        UnleashedHUD->setPosition({4, 257});
        UnleashedHUD->setZOrder(100);
        this->addChild(UnleashedHUD);

        // cocos2d::ccColor3b to tint the bars with, use #3C4E86
        auto hudBarColor = ccc3(0x3C, 0x4E, 0x86);

        // time bar setup
        auto timeBar = CCSprite::createWithSpriteFrameName("hudBar_big.png"_spr);
        auto timeBarOverlay = CCSprite::createWithSpriteFrameName("hudBar_bigOverlay.png"_spr);
        auto timeBarNode = CCNode::create();

        timeBar->setID("time-bar-main"_spr);
        timeBarOverlay->setID("time-bar-overlay"_spr);
        timeBarNode->setID("time-bar"_spr);
        timeBar->setColor(hudBarColor);
        timeBarOverlay->setOpacity(25);
        timeBarOverlay->setZOrder(1);

        timeBarNode->addChild(timeBar);
        timeBarNode->addChild(timeBarOverlay);

        // attempts bar setup
        auto jumpsBar = CCSprite::createWithSpriteFrameName("hudBar_big.png"_spr);
        auto jumpsBarOverlay = CCSprite::createWithSpriteFrameName("hudBar_bigOverlay.png"_spr);
        auto jumpsBarNode = CCNode::create();

        jumpsBar->setID("jumps-bar-main"_spr);
        jumpsBarOverlay->setID("jumps-bar-overlay"_spr);
        jumpsBarNode->setID("jumps-bar"_spr);
        jumpsBarNode->setPosition({0, -35});
        jumpsBar->setColor(hudBarColor);
        jumpsBarOverlay->setOpacity(25);
        jumpsBarOverlay->setZOrder(1);

        jumpsBarNode->addChild(jumpsBar);
        jumpsBarNode->addChild(jumpsBarOverlay);

        // time bar smaller setup
        auto timeBarSmall = CCSprite::createWithSpriteFrameName("hudBar_small.png"_spr);
        auto timeBarSmallOverlay = CCSprite::createWithSpriteFrameName("hudBar_smallOverlay.png"_spr);
        auto timeTitle = CCSprite::createWithSpriteFrameName("timeTitle.png"_spr);
        auto timeBarSmallNode = CCNode::create();

        timeBarSmall->setID("time-bar-small-main"_spr);
        timeBarSmallOverlay->setID("time-bar-small-overlay"_spr);
        timeBarSmallNode->setID("time-bar-small"_spr);
        timeTitle->setID("time-title"_spr);
        timeBarSmallNode->setPosition({-27, 14});
        timeBarSmallNode->setZOrder(2);
        timeBarSmallNode->setScale(0.9f);
        timeBarSmall->setColor(hudBarColor);
        timeBarSmallOverlay->setOpacity(80);
        timeBarSmallOverlay->setZOrder(1);
        timeTitle->setZOrder(3);
        timeTitle->setPosition({137, 1});
        timeTitle->setScale(3.0f);

        timeBarSmallNode->addChild(timeBarSmall);
        timeBarSmallNode->addChild(timeBarSmallOverlay);
        timeBarSmallNode->addChild(timeTitle);

        // attempts bar smaller setup
        auto jumpsBarSmall = CCSprite::createWithSpriteFrameName("hudBar_small.png"_spr);
        auto jumpsBarSmallOverlay = CCSprite::createWithSpriteFrameName("hudBar_smallOverlay.png"_spr);
        auto jumpsTitle = CCSprite::createWithSpriteFrameName("jumpsTitle.png"_spr);
        auto jumpsBarSmallNode = CCNode::create();

        jumpsBarSmall->setID("jumps-bar-small-main"_spr);
        jumpsBarSmallOverlay->setID("jumps-bar-small-overlay"_spr);
        jumpsBarSmallNode->setID("jumps-bar-small"_spr);
        jumpsTitle->setID("jumps-title"_spr);
        jumpsBarSmallNode->setPosition({-27, -21});
        jumpsBarSmallNode->setZOrder(2);
        jumpsBarSmallNode->setScale(0.9f);
        jumpsBarSmall->setColor(hudBarColor);
        jumpsBarSmallOverlay->setOpacity(80);
        jumpsBarSmallOverlay->setZOrder(1);
        jumpsTitle->setZOrder(3);
        jumpsTitle->setPosition({142, 1});
        jumpsTitle->setScale(3.0f);

        jumpsBarSmallNode->addChild(jumpsBarSmall);
        jumpsBarSmallNode->addChild(jumpsBarSmallOverlay);
        jumpsBarSmallNode->addChild(jumpsTitle);

        // lives icon
        auto livesIcon = CCSprite::createWithSpriteFrameName("lifeIcon.png"_spr);
        livesIcon->setID("lives-icon"_spr);
        livesIcon->setPosition({92.5f, 39.5f});
        livesIcon->setScale(1.1f);

        // lives label
        auto lives = level->m_stars;
        auto livesLabel = CCLabelBMFont::create(std::to_string(lives).c_str(), "unleashedHUDFont.fnt"_spr);
        livesLabel->setID("lives-label"_spr);
        livesLabel->setPosition({123.5f, 38.5f});
        livesLabel->setScaleX(1.1f);
        livesLabel->setScaleY(0.9f);

        // time label
        auto levelTimeText = level->m_attemptTime.value();
        auto timeLabel = CCLabelBMFont::create(std::to_string(levelTimeText).c_str(), "unleashedHUDFont.fnt"_spr);
        timeLabel->setID("time-label"_spr);
        timeLabel->setScaleX(1.1f);
        timeLabel->setScaleY(0.9f);
        timeLabel->scheduleUpdate();
        timeLabel->setPosition({121.0f, 1.5f});
        timeLabel->setScale(0.65f);

        // jumps label
        auto jumpsLabel = CCLabelBMFont::create(std::to_string(m_jumps).c_str(), "unleashedHUDFont.fnt"_spr);
        jumpsLabel->setID("jumps-label"_spr);
        jumpsLabel->setScaleX(1.1f);
        jumpsLabel->setScaleY(0.9f);
        jumpsLabel->scheduleUpdate();
        jumpsLabel->setPosition({121.0f, -33.0f});
        jumpsLabel->setScale(0.65f);

        // add everything to the HUD node
        UnleashedHUD->addChild(timeBarNode);
        UnleashedHUD->addChild(jumpsBarNode);
        UnleashedHUD->addChild(timeBarSmallNode);
        UnleashedHUD->addChild(jumpsBarSmallNode);
        UnleashedHUD->addChild(livesIcon);
        UnleashedHUD->addChild(livesLabel);
        UnleashedHUD->addChild(timeLabel);
        UnleashedHUD->addChild(jumpsLabel);

        return true;
    }

    void startGame() {
        PlayLayer::startGame();

        auto fmod = FMODAudioEngine::sharedEngine();
        fmod->playEffect("go.ogg"_spr);

        auto goBackSprite = this->getChildByID("go-back-sprite"_spr);
        auto goMainSprite = this->getChildByID("go-main-sprite"_spr);

        // create actions in order to animate the sprites
        auto showBack = CCFadeTo::create(0.05f, 100);
        auto showMain = CCSequence::create(
            CCDelayTime::create(0.1f),
            CCFadeIn::create(0.15f),
            nullptr
        );
        auto scaleMain = CCSequence::create(
            CCDelayTime::create(0.1f),
            CCEaseIn::create(CCScaleTo::create(0.15f, 1.75f), 2.0f),
            nullptr
        );
        auto scaleBack = CCSequence::create(
            CCDelayTime::create(0.26f),
            CCEaseExponentialOut::create(CCScaleTo::create(0.15f, 12.0f)),
            nullptr
        );
        auto fadeOutBack = CCSequence::create(
            CCDelayTime::create(0.26f),
            CCEaseExponentialOut::create(CCFadeOut::create(0.15f)),
            nullptr
        );
        auto squishMain = CCSequence::create(
            CCDelayTime::create(1.28f),
            CCEaseBackIn::create(CCScaleBy::create(0.08f, 2.5f, 0.1f)),
            nullptr
        );
        auto fadeOutMain = CCSequence::create(
            CCDelayTime::create(1.36f),
            CCFadeOut::create(0.05f),
            nullptr
        );

        // run actions
        goBackSprite->runAction(showBack);
        goMainSprite->runAction(showMain);
        goMainSprite->runAction(scaleMain);
        goBackSprite->runAction(scaleBack);
        goBackSprite->runAction(fadeOutBack);
        goMainSprite->runAction(squishMain);
        goMainSprite->runAction(fadeOutMain);

    }

    void levelComplete() {
        PlayLayer::levelComplete();
        auto fmod = FMODAudioEngine::sharedEngine();

        if (unleashedSounds){
            fmod->playEffect("goal_ring.ogg"_spr);
        }

        if(unleashedRankings){

            int attempts = this->m_attempts;

            int randomBest = genRandomInt(1, 6);
            int randomGood = genRandomInt(1, 5);
            int randomBad = genRandomInt(1, 5);

            if (attempts <= 3) {
                fmod->playEffect(fmt::format("complete_best_{}.ogg"_spr, randomBest));
            } else if (attempts > 3 && attempts <= 10) {
                fmod->playEffect(fmt::format("complete_good_{}.ogg"_spr, randomGood));
            } else {
                fmod->playEffect(fmt::format("complete_bad_{}.ogg"_spr, randomBad));
            }

        }
    }
};

class $modify(EndLevelLayer) {
    void customSetup() {
        EndLevelLayer::customSetup();

        int attempts = m_playLayer->m_attempts;
        auto fmod = FMODAudioEngine::sharedEngine();

        auto rankSprite = CCSprite::createWithSpriteFrameName("rank_placeholder.png"_spr);
        rankSprite->setOpacity(0);
        rankSprite->setZOrder(10);
        rankSprite->setPosition({50, 50});
        rankSprite->setID("rank-sprite"_spr);
        rankSprite->setScale(9.0f);
        this->addChild(rankSprite);

        geode::log::debug("attempts: {}", attempts);

        bool rankS = attempts == 1;
        bool rankA = attempts == 2 || attempts == 3;
        bool rankB = attempts >= 4 && attempts <= 6;
        bool rankC = attempts >= 7 && attempts <= 10;
        bool rankD = attempts >= 11 && attempts <= 15;
        bool rankE = attempts > 16;

        if (rankS) {
            rankSprite->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("rank_s.png"_spr));
            fmod->playEffect("rankS.ogg"_spr);
        } else if (rankA) {
            rankSprite->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("rank_a.png"_spr));
            fmod->playEffect("rankA.ogg"_spr);
        } else if (rankB) {
            rankSprite->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("rank_b.png"_spr));
            fmod->playEffect("rankC.ogg"_spr);
        } else if (rankC) {
            rankSprite->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("rank_c.png"_spr));
            fmod->playEffect("rankC.ogg"_spr);
        } else if (rankD) {
            rankSprite->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("rank_d.png"_spr));
            fmod->playEffect("rankC.ogg"_spr);
        } else if (rankE) {
            rankSprite->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("rank_e.png"_spr));
            fmod->playEffect("rankE.ogg"_spr);
        }

        // ---------------------------------
        // UNLEASHED RANKING SCREEN
        // MAKING THIS IS GONNA BE A PAIN
        // ---------------------------------

        
    }

    void onHideLayer(CCObject* sender) {
        EndLevelLayer::onHideLayer(sender);
        geode::log::debug("when escondes el endlevel layer");
    }

    void showLayer(bool p0) {
        EndLevelLayer::showLayer(p0);

        auto literallyTheEndscreen = this->getChildByID("main-layer");
        literallyTheEndscreen->stopAllActions();
        literallyTheEndscreen->setVisible(false);
        
        auto ranking = this->getChildByID("rank-sprite"_spr);

        // animations
        auto fadeIn = CCFadeIn::create(0.3f);
        auto scaleDown = CCScaleTo::create(0.3, 1.75f);

        ranking->runAction(fadeIn);
        ranking->runAction(scaleDown);
    }

};