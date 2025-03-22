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
// ------------------------------------
// FETCH SETTINGS AND STUFF
// ------------------------------------

auto energyGaugeOpacity = Mod::get()->getSettingValue<int64_t>("ringenergy-opacity");

$on_mod(Loaded) {
    listenForSettingChanges("ringenergy-opacity", [](int value) {
        energyGaugeOpacity = value;
    });
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

        // do the uhh thing
        auto playLayer = PlayLayer::get();

        if (playLayer) {
            int jumps = playLayer->m_jumps;
            auto lifeSprite = playLayer->getChildByID("life-up-sprite"_spr);
            if (jumps % 100 == 0) {
                auto winSize = CCDirector::sharedDirector()->getWinSize();
                auto winSizeWidth = winSize.width;
                auto winSizeHeight = winSize.height;

                auto midWidth = winSizeWidth / 2;
                auto midHeight = winSizeHeight / 2;

                lifeSprite->setPosition({midWidth, midHeight - 70.0f});
                lifeSprite->setScaleY(0.2f);
                lifeSprite->setScaleX(0.f);

                auto immediateFadeIn = CCFadeIn::create(0.f);
                auto moveToPos = CCEaseIn::create(CCMoveTo::create(0.9f, {winSizeWidth / 2, 260.0f}), 2.5f);

                fmod->playEffect("lifeAdded.ogg"_spr);
                lifeSprite->runAction(immediateFadeIn);
                lifeSprite->runAction(moveToPos);

                auto helpme = CCSequence::create(
                    CCEaseOut::create(CCScaleTo::create(0.25f, -0.3f, 0.3f), 2.0f),
                    CCDelayTime::create(0.05f),
                    CCEaseIn::create(CCScaleTo::create(0.45f, 0.6f, 0.6f), 2.0f),
                    nullptr
                );

                auto fadeOut = CCSequence::create(
                    CCDelayTime::create(2.5f),
                    CCFadeOut::create(0.0f),
                    nullptr
                );

                lifeSprite->runAction(helpme);
                lifeSprite->runAction(fadeOut);
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
        CCLabelBMFont* timeLabel = CCLabelBMFont::create("00:00:00", "unleashedHUDFont.fnt"_spr);
        CCLabelBMFont* jumpsLabel = CCLabelBMFont::create("0000000", "unleashedHUDFont.fnt"_spr);
        CCLabelBMFont* attemptsLabel = nullptr;
        // --------------------
        // RING ENERGY
        // --------------------
        // gauge node
        CCNodeRGBA* ringEnergyGaugeNode = CCNodeRGBA::create();
        // energy gauge
        CCSprite* ringEnergyMeter = CCSprite::createWithSpriteFrameName("energy_meter.png"_spr);
        // speed fills
        CCSprite* energySlow = CCSprite::createWithSpriteFrameName("energy_slow.png"_spr);
        CCSprite* energyNrml = CCSprite::createWithSpriteFrameName("energy_normal.png"_spr);
        CCSprite* energyFast = CCSprite::createWithSpriteFrameName("energy_fast.png"_spr);
        CCSprite* energyVFast = CCSprite::createWithSpriteFrameName("energy_vfast.png"_spr);
        CCSprite* energyVVFast = CCSprite::createWithSpriteFrameName("energy_vvfast.png"_spr);
        // progress fills
        CCSprite* energyProgress10 = CCSprite::createWithSpriteFrameName("energy_10.png"_spr);
        CCSprite* energyProgress20 = CCSprite::createWithSpriteFrameName("energy_20.png"_spr);
        CCSprite* energyProgress30 = CCSprite::createWithSpriteFrameName("energy_30.png"_spr);
        CCSprite* energyProgress40 = CCSprite::createWithSpriteFrameName("energy_40.png"_spr);
        CCSprite* energyProgress50 = CCSprite::createWithSpriteFrameName("energy_50.png"_spr);
        CCSprite* energyProgress60 = CCSprite::createWithSpriteFrameName("energy_60.png"_spr);
        CCSprite* energyProgress70 = CCSprite::createWithSpriteFrameName("energy_70.png"_spr);
        CCSprite* energyProgress80 = CCSprite::createWithSpriteFrameName("energy_80.png"_spr);
        CCSprite* energyProgress90 = CCSprite::createWithSpriteFrameName("energy_90.png"_spr);
        CCSprite* energyProgress100 = CCSprite::createWithSpriteFrameName("energy_100.png"_spr);
        // REWRITING THE HUD ELEMENTS TO BE IN FIELDS INSTEAD FOR EASIER ACCESS
        CCNodeRGBA* UnleashedHUD = CCNodeRGBA::create();
        CCSprite* timeBar = CCSprite::createWithSpriteFrameName("hudBar_big.png"_spr);
        CCSprite* timeBarOverlay = CCSprite::createWithSpriteFrameName("hudBar_bigOverlay.png"_spr);
        CCNodeRGBA* timeBarNode = CCNodeRGBA::create();
        CCSprite* jumpsBar = CCSprite::createWithSpriteFrameName("hudBar_big.png"_spr);
        CCSprite* jumpsBarOverlay = CCSprite::createWithSpriteFrameName("hudBar_bigOverlay.png"_spr);
        CCNodeRGBA* jumpsBarNode = CCNodeRGBA::create();
        CCSprite* timeBarSmall = CCSprite::createWithSpriteFrameName("hudBar_small.png"_spr);
        CCSprite* timeBarSmallOverlay = CCSprite::createWithSpriteFrameName("hudBar_smallOverlay.png"_spr);
        CCSprite* timeTitle = CCSprite::createWithSpriteFrameName("timeTitle.png"_spr);
        CCNodeRGBA* timeBarSmallNode = CCNodeRGBA::create();
        CCSprite* jumpsBarSmall = CCSprite::createWithSpriteFrameName("hudBar_small.png"_spr);
        CCSprite* jumpsBarSmallOverlay = CCSprite::createWithSpriteFrameName("hudBar_smallOverlay.png"_spr);
        CCSprite* jumpsTitle = CCSprite::createWithSpriteFrameName("jumpsTitle.png"_spr);
        CCNodeRGBA* jumpsBarSmallNode = CCNodeRGBA::create();
        CCLabelBMFont* livesLabel = CCLabelBMFont::create("00", "unleashedHUDFont.fnt"_spr);
        CCSprite* livesIcon = CCSprite::createWithSpriteFrameName("lifeIcon.png"_spr);
    };

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        auto fmod = FMODAudioEngine::sharedEngine();
        auto fields = m_fields.self();
        fmod->playEffect("herewe.ogg"_spr);

        auto winSize = CCDirector::sharedDirector()->getWinSize();
        auto goBackSprite = CCSprite::createWithSpriteFrameName("go_backsprite.png"_spr);
        auto goMainSprite = CCSprite::createWithSpriteFrameName("go_mainsprite.png"_spr);

        auto lifeUpSprite = CCSprite::createWithSpriteFrameName("1upIcon.png"_spr);
        lifeUpSprite->setOpacity(0);
        lifeUpSprite->setID("life-up-sprite"_spr);
        this->addChild(lifeUpSprite);

        // hide the sprites
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
        fields->UnleashedHUD->setID("UnleashedHUD"_spr);
        fields->UnleashedHUD->setScale(0.65f);
        fields->UnleashedHUD->setPosition({4, 257});
        fields->UnleashedHUD->setZOrder(100);
        this->addChild(fields->UnleashedHUD);

        // cocos2d::ccColor3b to tint the bars with, use #3C4E86
        auto hudBarColor = ccc3(0x3C, 0x4E, 0x86);

        // time bar
        fields->timeBar->setID("time-bar-main"_spr);
        fields->timeBarOverlay->setID("time-bar-overlay"_spr);
        fields->timeBarNode->setID("time-bar"_spr);
        fields->timeBar->setColor(hudBarColor);
        fields->timeBarOverlay->setOpacity(25);
        fields->timeBarOverlay->setZOrder(1);
        
        fields->timeBarNode->addChild(fields->timeBar);
        fields->timeBarNode->addChild(fields->timeBarOverlay);
        fields->timeBarNode->setCascadeOpacityEnabled(true);

        // attempts bar
        fields->jumpsBar->setID("jumps-bar-main"_spr);
        fields->jumpsBarOverlay->setID("jumps-bar-overlay"_spr);
        fields->jumpsBarNode->setID("jumps-bar"_spr);
        fields->jumpsBarNode->setPosition({0, -35});
        fields->jumpsBar->setColor(hudBarColor);
        fields->jumpsBarOverlay->setOpacity(25);
        fields->jumpsBarOverlay->setZOrder(1);

        fields->jumpsBarNode->addChild(fields->jumpsBar);
        fields->jumpsBarNode->addChild(fields->jumpsBarOverlay);
        fields->jumpsBarNode->setCascadeOpacityEnabled(true);

        // time bar smaller
        fields->timeBarSmall->setID("time-bar-small-main"_spr);
        fields->timeBarSmallOverlay->setID("time-bar-small-overlay"_spr);
        fields->timeBarSmallNode->setID("time-bar-small"_spr);
        fields->timeTitle->setID("time-title"_spr);
        fields->timeBarSmallNode->setPosition({-27, 14});
        fields->timeBarSmallNode->setZOrder(2);
        fields->timeBarSmallNode->setScale(0.9f);
        fields->timeBarSmall->setColor(hudBarColor);
        fields->timeBarSmallOverlay->setOpacity(80);
        fields->timeBarSmallOverlay->setZOrder(1);
        fields->timeTitle->setZOrder(3);
        fields->timeTitle->setPosition({137, 1});
        fields->timeTitle->setScale(3.0f);

        fields->timeBarSmallNode->addChild(fields->timeBarSmall);
        fields->timeBarSmallNode->addChild(fields->timeBarSmallOverlay);
        fields->timeBarSmallNode->addChild(fields->timeTitle);
        fields->timeBarSmallNode->setCascadeOpacityEnabled(true);

        // attempts bar smaller
        fields->jumpsBarSmall->setID("jumps-bar-small-main"_spr);
        fields->jumpsBarSmallOverlay->setID("jumps-bar-small-overlay"_spr);
        fields->jumpsBarSmallNode->setID("jumps-bar-small"_spr);
        fields->jumpsTitle->setID("jumps-title"_spr);
        fields->jumpsBarSmallNode->setPosition({-27, -21});
        fields->jumpsBarSmallNode->setZOrder(2);
        fields->jumpsBarSmallNode->setScale(0.9f);
        fields->jumpsBarSmall->setColor(hudBarColor);
        fields->jumpsBarSmallOverlay->setOpacity(80);
        fields->jumpsBarSmallOverlay->setZOrder(1);
        fields->jumpsTitle->setZOrder(3);
        fields->jumpsTitle->setPosition({142, 1});
        fields->jumpsTitle->setScale(3.0f);

        fields->jumpsBarSmallNode->addChild(fields->jumpsBarSmall);
        fields->jumpsBarSmallNode->addChild(fields->jumpsBarSmallOverlay);
        fields->jumpsBarSmallNode->addChild(fields->jumpsTitle);
        fields->jumpsBarSmallNode->setCascadeOpacityEnabled(true);

        // lives icon
        fields->livesIcon->setID("lives-icon"_spr);
        fields->livesIcon->setPosition({92.5f, 39.5f});
        fields->livesIcon->setScale(1.1f);

        // lives label
        int stars = level->m_stars;
        std::string livesText = fmt::format("{:02}", stars);
        fields->livesLabel->setString(livesText.c_str(), true);
        fields->livesLabel->setID("lives-label"_spr);
        fields->livesLabel->setPosition({123.5f, 38.5f});
        fields->livesLabel->setScaleX(1.1f);
        fields->livesLabel->setScaleY(0.9f);

        // time label
        fields->timeLabel->setID("time-label"_spr);
        fields->timeLabel->setScaleX(1.1f);
        fields->timeLabel->setScaleY(0.9f);
        fields->timeLabel->scheduleUpdate();
        fields->timeLabel->setPosition({90.0f, 1.5f});
        fields->timeLabel->setScale(0.65f);
        fields->timeLabel->setAlignment(kCCTextAlignmentRight);
        fields->timeLabel->setAnchorPoint({0.f, 0.5f});

        // jumps label
        fields->jumpsLabel->setID("jumps-label"_spr);
        fields->jumpsLabel->setScaleX(1.1f);
        fields->jumpsLabel->setScaleY(0.9f);
        fields->jumpsLabel->scheduleUpdate();
        fields->jumpsLabel->setPosition({90.0f, -33.0f});
        fields->jumpsLabel->setScale(0.65f);
        fields->jumpsLabel->setAlignment(kCCTextAlignmentRight);
        fields->jumpsLabel->setAnchorPoint({0.f, 0.5f});

        // --- RING ENERGY STUFFFFF ---
        fields->ringEnergyMeter->setZOrder(2);
        fields->attemptsLabel = CCLabelBMFont::create(fmt::format("{}", m_attempts).c_str(), "unleashedHUDFont.fnt"_spr);
        fields->attemptsLabel->setZOrder(2);
        fields->attemptsLabel->setScale(0.2f);
        fields->attemptsLabel->setAnchorPoint({0.f, 0.5f});
        fields->attemptsLabel->setPosition({-50.0f, -1.0f});

        fields->ringEnergyGaugeNode->addChild(fields->ringEnergyMeter);
        fields->ringEnergyGaugeNode->addChild(fields->energySlow);
        fields->ringEnergyGaugeNode->addChild(fields->energyNrml);
        fields->ringEnergyGaugeNode->addChild(fields->energyFast);
        fields->ringEnergyGaugeNode->addChild(fields->energyVFast);
        fields->ringEnergyGaugeNode->addChild(fields->energyVVFast);
        fields->ringEnergyGaugeNode->addChild(fields->energyProgress10);
        fields->ringEnergyGaugeNode->addChild(fields->energyProgress20);
        fields->ringEnergyGaugeNode->addChild(fields->energyProgress30);
        fields->ringEnergyGaugeNode->addChild(fields->energyProgress40);
        fields->ringEnergyGaugeNode->addChild(fields->energyProgress50);
        fields->ringEnergyGaugeNode->addChild(fields->energyProgress60);
        fields->ringEnergyGaugeNode->addChild(fields->energyProgress70);
        fields->ringEnergyGaugeNode->addChild(fields->energyProgress80);
        fields->ringEnergyGaugeNode->addChild(fields->energyProgress90);
        fields->ringEnergyGaugeNode->addChild(fields->energyProgress100);
        fields->ringEnergyGaugeNode->addChild(fields->attemptsLabel);
        fields->ringEnergyGaugeNode->setCascadeOpacityEnabled(true);
        fields->ringEnergyGaugeNode->setOpacity(energyGaugeOpacity);

        fields->ringEnergyGaugeNode->setScale(1.77f);
        fields->ringEnergyGaugeNode->setPosition({163.0f, 37.5f});
        fields->ringEnergyGaugeNode->setID("ring-energy-gauge"_spr);

        // add everything to the HUD node
        fields->UnleashedHUD->setCascadeOpacityEnabled(true);
        fields->UnleashedHUD->addChild(fields->timeBarNode);
        fields->UnleashedHUD->addChild(fields->jumpsBarNode);
        fields->UnleashedHUD->addChild(fields->timeBarSmallNode);
        fields->UnleashedHUD->addChild(fields->jumpsBarSmallNode);
        fields->UnleashedHUD->addChild(fields->livesIcon);
        fields->UnleashedHUD->addChild(fields->livesLabel);
        fields->UnleashedHUD->addChild(fields->timeLabel);
        fields->UnleashedHUD->addChild(fields->jumpsLabel);
        this->addChild(fields->ringEnergyGaugeNode);

        return true;
    }

    void resetLevel() {
        PlayLayer::resetLevel();

        auto fields = m_fields.self();

        fields->UnleashedHUD->setOpacity(255);
        fields->ringEnergyGaugeNode->setOpacity(energyGaugeOpacity);
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
        auto fields = m_fields.self();

        fmod->playEffect("goal_ring.ogg"_spr);

        fmod->fadeOutMusic(2.5f, 0);
        fmod->fadeOutMusic(2.5f, 1);
        fmod->fadeOutMusic(2.5f, 2);
        fmod->fadeOutMusic(2.5f, 3);
        fmod->fadeOutMusic(2.5f, 4);

        auto fadeOutHud = CCFadeOut::create(0.5f);
        auto fadeOutRingGauge = CCFadeOut::create(0.5f);
        fields->UnleashedHUD->runAction(fadeOutHud);
        fields->ringEnergyGaugeNode->runAction(fadeOutRingGauge);

    }
};