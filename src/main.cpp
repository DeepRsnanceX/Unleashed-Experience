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

float playerSpeedMain = 0.f;

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

        playerSpeedMain = p0;

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
    struct Fields{
        // labels
        CCLabelBMFont* timeLabel = nullptr;
        CCLabelBMFont* jumpsLabel = nullptr;
        CCLabelBMFont* attemptsLabel = nullptr;
        // --------------------
        // RING ENERGY
        // --------------------
        // energy gauge
        CCSprite* ringEnergyMeter = nullptr;
        // speed fills
        CCSprite* energySlow = nullptr;
        CCSprite* energyNrml = nullptr;
        CCSprite* energyFast = nullptr;
        CCSprite* energyVFast = nullptr;
        CCSprite* energyVVFast = nullptr;
        // progress fills
        CCSprite* energyProgress10 = nullptr;
        CCSprite* energyProgress20 = nullptr;
        CCSprite* energyProgress30 = nullptr;
        CCSprite* energyProgress40 = nullptr;
        CCSprite* energyProgress50 = nullptr;
        CCSprite* energyProgress60 = nullptr;
        CCSprite* energyProgress70 = nullptr;
        CCSprite* energyProgress80 = nullptr;
        CCSprite* energyProgress90 = nullptr;
        CCSprite* energyProgress100 = nullptr;
    };

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        auto fmod = FMODAudioEngine::sharedEngine();
        auto fields = m_fields.self();
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
        UnleashedHUD->setID("UnleashedHUD"_spr);
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
        std::string timePlaceholder = fmt::format("00:00:00");
        fields->timeLabel = CCLabelBMFont::create(timePlaceholder.c_str(), "unleashedHUDFont.fnt"_spr);
        fields->timeLabel->setID("time-label"_spr);
        fields->timeLabel->setScaleX(1.1f);
        fields->timeLabel->setScaleY(0.9f);
        fields->timeLabel->scheduleUpdate();
        fields->timeLabel->setPosition({90.0f, 1.5f});
        fields->timeLabel->setScale(0.65f);
        fields->timeLabel->setAlignment(kCCTextAlignmentRight);
        fields->timeLabel->setAnchorPoint({0.f, 0.5f});

        // jumps label
        std::string jumpsPlaceholder = fmt::format("0000000");
        fields->jumpsLabel = CCLabelBMFont::create(jumpsPlaceholder.c_str(), "unleashedHUDFont.fnt"_spr);
        fields->jumpsLabel->setID("jumps-label"_spr);
        fields->jumpsLabel->setScaleX(1.1f);
        fields->jumpsLabel->setScaleY(0.9f);
        fields->jumpsLabel->scheduleUpdate();
        fields->jumpsLabel->setPosition({90.0f, -33.0f});
        fields->jumpsLabel->setScale(0.65f);
        fields->jumpsLabel->setAlignment(kCCTextAlignmentRight);
        fields->jumpsLabel->setAnchorPoint({0.f, 0.5f});

        // --- RING ENERGY STUFFFFF ---
        auto ringEnergyGaugeNode = CCNode::create();
        fields->ringEnergyMeter = CCSprite::createWithSpriteFrameName("energy_meter.png"_spr);
        fields->ringEnergyMeter->setZOrder(2);
        fields->energySlow = CCSprite::createWithSpriteFrameName("energy_slow.png"_spr);
        fields->energyNrml = CCSprite::createWithSpriteFrameName("energy_normal.png"_spr);
        fields->energyFast = CCSprite::createWithSpriteFrameName("energy_fast.png"_spr);
        fields->energyVFast = CCSprite::createWithSpriteFrameName("energy_vfast.png"_spr);
        fields->energyVVFast = CCSprite::createWithSpriteFrameName("energy_vvfast.png"_spr);
        fields->energyProgress10 = CCSprite::createWithSpriteFrameName("energy_10.png"_spr);
        fields->energyProgress20 = CCSprite::createWithSpriteFrameName("energy_20.png"_spr);
        fields->energyProgress30 = CCSprite::createWithSpriteFrameName("energy_30.png"_spr);
        fields->energyProgress40 = CCSprite::createWithSpriteFrameName("energy_40.png"_spr);
        fields->energyProgress50 = CCSprite::createWithSpriteFrameName("energy_50.png"_spr);
        fields->energyProgress60 = CCSprite::createWithSpriteFrameName("energy_60.png"_spr);
        fields->energyProgress70 = CCSprite::createWithSpriteFrameName("energy_70.png"_spr);
        fields->energyProgress80 = CCSprite::createWithSpriteFrameName("energy_80.png"_spr);
        fields->energyProgress90 = CCSprite::createWithSpriteFrameName("energy_90.png"_spr);
        fields->energyProgress100 = CCSprite::createWithSpriteFrameName("energy_100.png"_spr);

        fields->attemptsLabel = CCLabelBMFont::create(fmt::format("{}",m_attempts).c_str(), "unleashedHUDFont.fnt"_spr);
        fields->attemptsLabel->setZOrder(2);
        fields->attemptsLabel->setScale(0.2f);
        fields->attemptsLabel->setAnchorPoint({0.f, 0.5f});
        fields->attemptsLabel->setPosition({-50.0f, -1.0f});

        ringEnergyGaugeNode->addChild(fields->ringEnergyMeter);
        ringEnergyGaugeNode->addChild(fields->energySlow);
        ringEnergyGaugeNode->addChild(fields->energyNrml);
        ringEnergyGaugeNode->addChild(fields->energyFast);
        ringEnergyGaugeNode->addChild(fields->energyVFast);
        ringEnergyGaugeNode->addChild(fields->energyVVFast);
        ringEnergyGaugeNode->addChild(fields->energyProgress10);
        ringEnergyGaugeNode->addChild(fields->energyProgress20);
        ringEnergyGaugeNode->addChild(fields->energyProgress30);
        ringEnergyGaugeNode->addChild(fields->energyProgress40);
        ringEnergyGaugeNode->addChild(fields->energyProgress50);
        ringEnergyGaugeNode->addChild(fields->energyProgress60);
        ringEnergyGaugeNode->addChild(fields->energyProgress70);
        ringEnergyGaugeNode->addChild(fields->energyProgress80);
        ringEnergyGaugeNode->addChild(fields->energyProgress90);
        ringEnergyGaugeNode->addChild(fields->energyProgress100);
        ringEnergyGaugeNode->addChild(fields->attemptsLabel);

        ringEnergyGaugeNode->setScale(2.73f);
        ringEnergyGaugeNode->setPosition({245.0f, -338.0f});

        // add everything to the HUD node
        UnleashedHUD->addChild(timeBarNode);
        UnleashedHUD->addChild(jumpsBarNode);
        UnleashedHUD->addChild(timeBarSmallNode);
        UnleashedHUD->addChild(jumpsBarSmallNode);
        UnleashedHUD->addChild(livesIcon);
        UnleashedHUD->addChild(livesLabel);
        UnleashedHUD->addChild(fields->timeLabel);
        UnleashedHUD->addChild(fields->jumpsLabel);
        UnleashedHUD->addChild(ringEnergyGaugeNode);

        return true;
    }

    void updateProgressbar() {
        PlayLayer::updateProgressbar();
    
        auto baseLayer = GJBaseGameLayer::get();
        auto fields = m_fields.self();
        float currentTime = baseLayer->m_gameState.m_levelTime;
    
        int minutes = static_cast<int>(currentTime) / 60;
        int seconds = static_cast<int>(currentTime) % 60;
        int milliseconds = static_cast<int>((currentTime - static_cast<int>(currentTime)) * 100);
        std::string levelTimeText = fmt::format("{:02}:{:02}:{:02}", minutes, seconds, milliseconds);

        auto jumpAmount = m_jumps;
        std::string jumpsText = fmt::format("{:06}", jumpAmount);

        std::string attemptsText = fmt::format("{:03}", m_attempts);
    
        if (fields->timeLabel) {
            fields->timeLabel->setString(levelTimeText.c_str(), true);
        }
        if (fields->jumpsLabel) {
            fields->jumpsLabel->setString(jumpsText.c_str(), true);
        }
        if (fields->attemptsLabel) {
            fields->attemptsLabel->setString(attemptsText.c_str(), true);
        }

        // ring energy gauge
        auto currentPercent = getCurrentPercentInt();
        bool progressBarsCreated = fields->energyProgress10 && fields->energyProgress20 && fields->energyProgress30 && fields->energyProgress40 && fields->energyProgress50 && fields->energyProgress60 && fields->energyProgress70 && fields->energyProgress80 && fields->energyProgress90 && fields->energyProgress100;
        bool energyBarsCreated = fields->energySlow && fields->energyNrml && fields->energyFast && fields->energyVFast && fields->energyVVFast;

        if (progressBarsCreated) {
            fields->energyProgress10->setVisible(currentPercent >= 10);
            fields->energyProgress20->setVisible(currentPercent >= 20);
            fields->energyProgress30->setVisible(currentPercent >= 30);
            fields->energyProgress40->setVisible(currentPercent >= 40);
            fields->energyProgress50->setVisible(currentPercent >= 50);
            fields->energyProgress60->setVisible(currentPercent >= 60);
            fields->energyProgress70->setVisible(currentPercent >= 70);
            fields->energyProgress80->setVisible(currentPercent >= 80);
            fields->energyProgress90->setVisible(currentPercent >= 90);
            fields->energyProgress100->setVisible(currentPercent >= 100);
        }

        if (energyBarsCreated) {
            fields->energySlow->setVisible(playerSpeedMain >= 0.7f);
            fields->energyNrml->setVisible(playerSpeedMain >= 0.9f);
            fields->energyFast->setVisible(playerSpeedMain >= 1.1f);
            fields->energyVFast->setVisible(playerSpeedMain >= 1.3f);
            fields->energyVVFast->setVisible(playerSpeedMain >= 1.6f);
        }

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

        fmod->playEffect("goal_ring.ogg"_spr);

        fmod->fadeOutMusic(1.5f, 0);
        fmod->fadeOutMusic(1.5f, 1);
        fmod->fadeOutMusic(1.5f, 2);
        fmod->fadeOutMusic(1.5f, 3);
        fmod->fadeOutMusic(1.5f, 4);

        auto hud = this->getChildByID("UnleashedHUD"_spr);
        auto movehudoff = CCSequence::create(
            CCDelayTime::create(0.76f),
            CCMoveBy::create(0.f, {0, 1000}),
            nullptr
        );
        hud->runAction(movehudoff);

    }
};