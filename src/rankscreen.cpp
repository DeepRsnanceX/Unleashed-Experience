#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/EndLevelLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/binding/GJGameLevel.hpp>
#include <Geode/binding/GJBaseGameLayer.hpp>
#include <Geode/loader/SettingV3.hpp>
#include "helper.hpp"
#include <random>

using namespace geode::prelude;

int maxCoins = 0;
int collectedCoinsManual = 0;

// ------------------------------------
// FETCH SETTINGS AND STUFF
// ------------------------------------

auto userCoinSFX = Mod::get()->getSettingValue<std::string>("usercoin-sfx");
auto secretCoinSFX = Mod::get()->getSettingValue<std::string>("secretcoin-sfx");
auto disableRankInCreated = Mod::get()->getSettingValue<bool>("disable-increated");
auto stageClearDo = Mod::get()->getSettingValue<bool>("stage-clear");
auto doResultsMusic = Mod::get()->getSettingValue<bool>("results-music");
auto doWhiteFlash = Mod::get()->getSettingValue<bool>("enable-flashbang");

$on_mod(Loaded) {
    listenForSettingChanges("usercoin-sfx", [](std::string value) {
        userCoinSFX = value;
    });
    listenForSettingChanges("secretcoin-sfx", [](std::string value) {
        secretCoinSFX = value;
    });
    listenForSettingChanges("disable-increated", [](bool value) {
        disableRankInCreated = value;
    });
    listenForSettingChanges("stage-clear", [](bool value) {
        stageClearDo = value;
    });
    listenForSettingChanges("results-music", [](bool value) {
        doResultsMusic = value;
    });
    listenForSettingChanges("enable-flashbang", [](bool value) {
        doWhiteFlash = value;
    });
}

int getRandomInt(int min, int max) {
    std::random_device rd;  // Obtain a random number from hardware
    std::mt19937 gen(rd()); // Seed the generator
    std::uniform_int_distribution<> distr(min, max); // Define the range

    return distr(gen);
}

void SonicUnleashed::rankReaction(float dt) {
    auto fmod = FMODAudioEngine::sharedEngine();
    auto playLayer = PlayLayer::get();

    int attempts = playLayer->m_attempts;

    int randomBest = getRandomInt(1, 6);
    int randomGood = getRandomInt(1, 5);
    int randomBad = getRandomInt(1, 5);

    if (attempts <= 3) {
        fmod->playEffect(fmt::format("complete_best_{}.ogg"_spr, randomBest));
    } else if (attempts > 3 && attempts <= 10) {
        fmod->playEffect(fmt::format("complete_good_{}.ogg"_spr, randomGood));
    } else {
        fmod->playEffect(fmt::format("complete_bad_{}.ogg"_spr, randomBad));
    }
}

void SonicUnleashed::rankPlacement(float dt) {
    auto fmod = FMODAudioEngine::sharedEngine();
    auto playLayer = PlayLayer::get();
    int attempts = playLayer->m_attempts;

    bool rankS = attempts == 1;
    bool rankA = attempts == 2 || attempts == 3;
    bool rankB = attempts >= 4 && attempts <= 6;
    bool rankC = attempts >= 7 && attempts <= 10;
    bool rankD = attempts >= 11 && attempts <= 15;
    bool rankE = attempts > 16;

    if (rankS) {
        fmod->playEffect("rankC.ogg"_spr);
        fmod->playEffect("rankS.ogg"_spr);
    } else if (rankA) {
        fmod->playEffect("rankC.ogg"_spr);
        fmod->playEffect("rankA.ogg"_spr);
    } else if (rankB) {
        fmod->playEffect("rankC.ogg"_spr);
    } else if (rankC) {
        fmod->playEffect("rankC.ogg"_spr);
    } else if (rankD) {
        fmod->playEffect("rankC.ogg"_spr);
    } else if (rankE) {
        fmod->playEffect("rankC.ogg"_spr);
        fmod->playEffect("rankE.ogg"_spr);
    }
}

void SonicUnleashed::slide1(float dt) {
    auto fmod = FMODAudioEngine::sharedEngine();
    fmod->playEffect("barSlide.ogg"_spr);
}
void SonicUnleashed::slide2(float dt) {
    auto fmod = FMODAudioEngine::sharedEngine();
    fmod->playEffect("barSlide.ogg"_spr);
}
void SonicUnleashed::slide3(float dt) {
    auto fmod = FMODAudioEngine::sharedEngine();
    fmod->playEffect("barSlide.ogg"_spr);
}

void SonicUnleashed::totalScoreSound(float dt) {
    auto fmod = FMODAudioEngine::sharedEngine();
    fmod->playEffect("totalScore.ogg"_spr);
}

void SonicUnleashed::stageClear(float dt) {
    auto fmod = FMODAudioEngine::sharedEngine();
    fmod->playEffect("stageClear.ogg"_spr);
}

void SonicUnleashed::rankingMusic(float dt) {
    auto fmod = FMODAudioEngine::sharedEngine();
    auto playLayer = PlayLayer::get();

    int attempts = playLayer->m_attempts;

    if (attempts >= 16){
        fmod->playMusic("badRanking.mp3"_spr, true, 0, 1);
    } else {
        fmod->playMusic("goodRanking.mp3"_spr, true, 0, 1);
    }
}

class $modify(GJBaseGameLayer) {
	void pickupItem(EffectGameObject *p0) {

        auto fmod = FMODAudioEngine::sharedEngine();
        std::string userCoinSound = fmt::format("{}.ogg"_spr, userCoinSFX);
        std::string secretCoinSound = fmt::format("{}.ogg"_spr, secretCoinSFX);

        if (p0->m_objectID == 1329) {
            collectedCoinsManual++;
            fmod->playEffect(userCoinSound.c_str());

        }
        if (p0->m_objectID == 142) {
            collectedCoinsManual = collectedCoinsManual + 1;
            fmod->playEffect(secretCoinSound.c_str());
        }

        GJBaseGameLayer::pickupItem(p0);
    }
};

class $modify(PlayLayer) {
    struct Fields {
        CCSprite* whiteFlashOverlay = CCSprite::createWithSpriteFrameName("white_overlay.png"_spr);
    };
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        auto f = m_fields.self();
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        
        f->whiteFlashOverlay->setZOrder(1000);
        f->whiteFlashOverlay->setScale(3.0f);
        f->whiteFlashOverlay->setOpacity(0);
        f->whiteFlashOverlay->setPosition({winSize.width / 2, winSize.height / 2});

        this->addChild(f->whiteFlashOverlay);

        maxCoins = level->m_coins;

        return true;
    }

    void levelComplete() {
        PlayLayer::levelComplete();

        auto f = m_fields.self();

        auto flashbang = CCSequence::create(
            CCFadeIn::create(0.75f),
            CCDelayTime::create(1.2f),
            CCFadeOut::create(0.35f),
            nullptr
        );

        if (doWhiteFlash) {
            f->whiteFlashOverlay->runAction(flashbang);
        }
    }

    void resetLevel() {
        PlayLayer::resetLevel();

        auto f = m_fields.self();

        if (doWhiteFlash) {
            f->whiteFlashOverlay->setOpacity(0);
        }

        collectedCoinsManual = 0;
    }
};

class $modify(EndLevelLayer) {
    struct Fields {
        CCNodeRGBA* rankingScreenNode = CCNodeRGBA::create();
        // -----------------------
        // RANKING SCREEN ELEMENTS
        // -----------------------
        // --- ranking sprite
        CCSprite* rankingSprite = CCSprite::createWithSpriteFrameName("rank_placeholder.png"_spr);
        CCSprite* afterimageRankingSprite = CCSprite::createWithSpriteFrameName("rank_placeholder.png"_spr);
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
        CCLabelBMFont* totalLabel = CCLabelBMFont::create("000000", "unleashedHUDFont.fnt"_spr);
        // --- top deco arrows
        CCSprite* topArrow = CCSprite::createWithSpriteFrameName("decoArrow.png"_spr);
        CCSprite* topArrow2 = CCSprite::createWithSpriteFrameName("decoArrow.png"_spr);
        CCSprite* topArrow3 = CCSprite::createWithSpriteFrameName("decoArrow.png"_spr);
        // -----------------------
        // OTHER STUFF
        // -----------------------
        CCMenu* menu = CCMenu::create();
        float extraDelay = 0.9f;
        float happenFaster = 0.1f;
        int totalScore = 0;
        bool isHidden = false;
    };

    void customSetup() {
        EndLevelLayer::customSetup();

        auto f = m_fields.self();
        auto fmod = FMODAudioEngine::sharedEngine();
        auto baselayer = GJBaseGameLayer::get();
        auto thisLevel = baselayer->m_level;

        if (thisLevel->m_isEditable && disableRankInCreated) return;

        // --------------------------------------------------------
        // CLICKABLE KEYBIND HINTS SETUP
        // --------------------------------------------------------

        // menu for the keybinds
        f->menu = CCMenu::create();
        f->menu->setLayout(
            RowLayout::create()
                ->setGap(27.f)
                ->setAxisAlignment(AxisAlignment::End)
                ->setAxisReverse(true)
                ->setCrossAxisOverflow(false)
        );
        f->menu->setPosition({260.f, 35.f});
        f->menu->setContentSize({550.f, 40.f});
        f->menu->setScale(0.65f);
        f->menu->setZOrder(10);
        this->addChild(f->menu);

        // sprites and buttons
        auto replayHint = CCSprite::createWithSpriteFrameName("hint_replay.png"_spr);
        auto replayBtn = CCMenuItemSpriteExtra::create(
            replayHint,
            this,
            menu_selector(EndLevelLayer::onReplay)
        );

        auto exitHint = CCSprite::createWithSpriteFrameName("hint_exit.png"_spr);
        auto exitBtn = CCMenuItemSpriteExtra::create(
            exitHint,
            this,
            menu_selector(EndLevelLayer::onMenu)
        );

        auto editHint = CCSprite::createWithSpriteFrameName("hint_edit.png"_spr);
        auto editBtn = CCMenuItemSpriteExtra::create(
            editHint,
            this,
            menu_selector(EndLevelLayer::onEdit)
        );

        auto lastCheckpointHint = CCSprite::createWithSpriteFrameName("hint_lastCheckpoint.png"_spr);
        auto lastCheckpointBtn = CCMenuItemSpriteExtra::create(
            lastCheckpointHint,
            this,
            menu_selector(EndLevelLayer::onRestartCheckpoint)
        );

        f->menu->addChild(replayBtn);
        f->menu->addChild(exitBtn);

        if (baselayer->m_isPracticeMode) {
            f->menu->addChild(lastCheckpointBtn);
        }
        if (thisLevel->m_isEditable) {
            f->menu->addChild(editBtn);
        }

        f->menu->updateLayout();
        f->menu->setOpacity(0);

        // --------------------------------------------------------
        // RANKING SPRITE SETUP
        // --------------------------------------------------------

        int attempts = m_playLayer->m_attempts;

        f->rankingSprite->setOpacity(0);
        f->rankingSprite->setZOrder(10);
        f->rankingSprite->setPosition({188, 83});
        f->rankingSprite->setID("rank-sprite"_spr);
        f->rankingSprite->setScale(9.0f);

        f->afterimageRankingSprite->setOpacity(0);
        f->afterimageRankingSprite->setZOrder(11);
        f->afterimageRankingSprite->setPosition({188, 83});
        f->afterimageRankingSprite->setID("rank-sprite-afterimage"_spr);
        f->afterimageRankingSprite->setScale(15.0f);

        bool rankS = attempts == 1;
        bool rankA = attempts == 2 || attempts == 3;
        bool rankB = attempts >= 4 && attempts <= 6;
        bool rankC = attempts >= 7 && attempts <= 10;
        bool rankD = attempts >= 11 && attempts <= 15;
        bool rankE = attempts > 16;

        if (rankS) {
            f->rankingSprite->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("rank_s.png"_spr));
            f->afterimageRankingSprite->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("rank_s.png"_spr));
            f->totalScore = f->totalScore + 100000;
            f->totalScore = f->totalScore + getRandomInt(0, 1000);
        } else if (rankA) {
            f->rankingSprite->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("rank_a.png"_spr));
            f->afterimageRankingSprite->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("rank_a.png"_spr));
            f->totalScore = f->totalScore + 50000;
            f->totalScore = f->totalScore + getRandomInt(0, 1000);
        } else if (rankB) {
            f->rankingSprite->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("rank_b.png"_spr));
            f->afterimageRankingSprite->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("rank_b.png"_spr));
            f->totalScore = f->totalScore + 25000;
            f->totalScore = f->totalScore + getRandomInt(0, 500);
        } else if (rankC) {
            f->rankingSprite->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("rank_c.png"_spr));
            f->afterimageRankingSprite->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("rank_c.png"_spr));
            f->totalScore = f->totalScore + 10000;
            f->totalScore = f->totalScore + getRandomInt(0, 500);
        } else if (rankD) {
            f->rankingSprite->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("rank_d.png"_spr));
            f->afterimageRankingSprite->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("rank_d.png"_spr));
            f->totalScore = f->totalScore + 5000;
            f->totalScore = f->totalScore + getRandomInt(0, 250);
        } else if (rankE) {
            f->rankingSprite->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("rank_e.png"_spr));
            f->afterimageRankingSprite->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("rank_e.png"_spr));
            f->totalScore = f->totalScore + 1000;
            f->totalScore = f->totalScore + getRandomInt(0, 250);
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
        // DECO ARROWS FOR RESULTS BAR
        // ----------------
        f->topArrow->setScale(0.7f);
        f->topArrow->setPosition({-65.0f, 281.0f});
        f->topArrow->setID("top-arrow"_spr);

        f->topArrow2->setScale(0.7f);
        f->topArrow2->setPosition({-65.0f, 281.0f});
        f->topArrow2->setID("top-arrow2"_spr);

        f->topArrow3->setScale(0.7f);
        f->topArrow3->setPosition({-65.0f, 281.0f});
        f->topArrow3->setID("top-arrow3"_spr);

        // ----------------
        // TITLE BARS
        // ----------------
        f->timeTitle->setScale(1.775f);
        f->timeTitle->setID("time-title"_spr);
        f->timeTitle->setPosition({322.0f + 6, 233.0f + 6});
        // starting pos = 328.0f, 239f
        // move by - X: -6, Y: -6
        f->timeTitle->setOpacity(0);

        f->attemptsTitle->setScale(1.775f);
        f->attemptsTitle->setID("atts-title"_spr);
        f->attemptsTitle->setPosition({322.0f + 6, 203.0f + 6});
        f->attemptsTitle->setOpacity(0);

        f->jumpsTitle->setScale(1.775f);
        f->jumpsTitle->setID("jumps-title"_spr);
        f->jumpsTitle->setPosition({322.0f + 6, 173.0f + 6});
        f->jumpsTitle->setOpacity(0);

        f->coinsTitle->setScale(1.775f);
        f->coinsTitle->setID("coins-title"_spr);
        f->coinsTitle->setPosition({322.0f + 6, 143.0f + 6});
        f->coinsTitle->setOpacity(0);

        // ----------------
        // LOWER TOTAL SECTION
        // ----------------
        f->totalTitle->setScale(1.775f);
        f->totalTitle->setID("total-title"_spr);
        f->totalTitle->setPosition({321.0f + 6, 84.5f + 6});
        f->totalTitle->setOpacity(0);

        f->totalBar->setScale(1.775f);
        f->totalBar->setPosition({36.0f - 550, 77.0f});
        f->totalBar->setID("total-bar"_spr);

        // ----------------
        // DATA LABELS
        // ----------------
        float attTime = baselayer->m_gameState.m_totalTime;
        int mins = static_cast<int>(attTime) / 60;
        int secs = static_cast<int>(attTime) % 60;
        int millis = static_cast<int>((attTime - static_cast<int>(attTime)) * 100);
        std::string levelTimeText = fmt::format("{:02}:{:02}:{:02}", mins, secs, millis);
        std::string attemptsText = fmt::format("{:06}", m_playLayer->m_attempts);
        std::string jumpsText = fmt::format("{:06}", m_playLayer->m_jumps);

        f->timeLabel->setString(levelTimeText.c_str(), true);
        f->timeLabel->setID("time-label"_spr);
        f->timeLabel->setAnchorPoint({1.0f, 0.5f});
        f->timeLabel->setScaleX(0.77f);
        f->timeLabel->setScaleY(0.65f);
        f->timeLabel->setPosition({453.0f, 227.5f});
        f->timeLabel->setAlignment(kCCTextAlignmentRight);
        f->timeLabel->setOpacity(0);

        f->attemptsLabel->setString(attemptsText.c_str(), true);
        f->attemptsLabel->setID("attempts-label"_spr);
        f->attemptsLabel->setAnchorPoint({1.0f, 0.5f});
        f->attemptsLabel->setPosition({453.0f, 197.5f});
        f->attemptsLabel->setScaleX(0.77f);
        f->attemptsLabel->setScaleY(0.65f);
        f->attemptsLabel->setAlignment(kCCTextAlignmentRight);
        f->attemptsLabel->setOpacity(0);
        
        f->jumpsLabel->setString(jumpsText.c_str(), true);
        f->jumpsLabel->setID("jumps-label"_spr);
        f->jumpsLabel->setAnchorPoint({1.0f, 0.5f});
        f->jumpsLabel->setPosition({453.0f, 167.5f});
        f->jumpsLabel->setScaleX(0.77f);
        f->jumpsLabel->setScaleY(0.65f);
        f->jumpsLabel->setAlignment(kCCTextAlignmentRight);
        f->jumpsLabel->setOpacity(0);

        std::string coinsText = fmt::format("{}/{}", collectedCoinsManual, maxCoins);
        f->coinsLabel->setString(coinsText.c_str(), true);
        f->coinsLabel->setID("coins-label"_spr);
        f->coinsLabel->setAnchorPoint({1.0f, 0.5f});
        f->coinsLabel->setPosition({453.0f, 137.5f});
        f->coinsLabel->setScaleX(0.77f);
        f->coinsLabel->setScaleY(0.65f);
        f->coinsLabel->setAlignment(kCCTextAlignmentRight);
        f->coinsLabel->setOpacity(0);

        // total score
        int totalJumpScore = m_playLayer->m_jumps * 10 + getRandomInt(0, 100);
        int finalScore = f->totalScore + totalJumpScore;
        std::string totalScoreText = fmt::format("{:06}", finalScore);

        f->totalLabel->setString(totalScoreText.c_str(), true);
        f->totalLabel->setID("total-label"_spr);
        f->totalLabel->setAnchorPoint({1.0f, 0.5f});
        f->totalLabel->setPosition({453.0f, 78.5f});
        f->totalLabel->setScaleX(0.77f);
        f->totalLabel->setScaleY(0.65f);
        f->totalLabel->setAlignment(kCCTextAlignmentRight);
        f->totalLabel->setOpacity(0);

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
        f->rankingScreenNode->addChild(f->coinsLabel);
        f->rankingScreenNode->addChild(f->totalLabel);
        f->rankingScreenNode->addChild(f->rankingSprite);
        f->rankingScreenNode->addChild(f->topArrow);
        f->rankingScreenNode->addChild(f->topArrow2);
        f->rankingScreenNode->addChild(f->topArrow3);
        f->rankingScreenNode->addChild(f->rankingSprite);
        f->rankingScreenNode->addChild(f->afterimageRankingSprite);
        f->rankingScreenNode->setCascadeOpacityEnabled(true);
        f->rankingScreenNode->setZOrder(100);
        this->addChild(f->rankingScreenNode);

    }

    void playCoinEffect(float p0) {
        // fuck you
        // *unplays your CoinEffect*
    }

    void onHideLayer(cocos2d::CCObject* sender) {
        EndLevelLayer::onHideLayer(sender);

        auto fields = m_fields.self();

        auto hideRankingStuff = CCFadeOut::create(0.5f);
        auto showRankingStuff = CCFadeIn::create(0.5f);
        auto baselayer = GJBaseGameLayer::get();
        auto thisLevel = baselayer->m_level;

        auto hideKeybindHints = CCFadeOut::create(0.5f);
        auto showKeybindHints = CCFadeIn::create(0.5f);

        if (thisLevel->m_isEditable && disableRankInCreated) return;

        if (!fields->isHidden) {
            fields->rankingScreenNode->runAction(hideRankingStuff);
            fields->menu->runAction(hideKeybindHints);
        } else {
            fields->rankingScreenNode->runAction(showRankingStuff);
            fields->menu->runAction(showKeybindHints);
        }

        fields->isHidden = !fields->isHidden;
    }

    void showLayer(bool p0) {
        EndLevelLayer::showLayer(p0);

        auto baselayer = GJBaseGameLayer::get();
        auto thisLevel = baselayer->m_level;

        if (thisLevel->m_isEditable && disableRankInCreated) return;

        auto literallyTheEndscreen = this->getChildByID("main-layer");
        auto f = m_fields.self();
        literallyTheEndscreen->stopAllActions();
        literallyTheEndscreen->setVisible(false);

        auto fadeInToWhite = CCFadeIn::create(0.75f);
        auto hold = CCDelayTime::create(0.35f);
        auto fadeOut = CCFadeOut::create(0.6f);
        auto fadeInToWhiteSeq = CCSequence::create(fadeInToWhite, hold, fadeOut, nullptr);

        auto showKeybinds = CCSequence::create(
            CCDelayTime::create(f->extraDelay),
            CCFadeIn::create(0.0f),
            nullptr
        );

        auto resultsBarMove = CCSequence::create(
            CCDelayTime::create(0.07f + f->extraDelay),
            CCEaseIn::create(CCMoveBy::create(0.28f, {300.0f, 0.f}), 2.0f),
            nullptr
        );
        auto decoArrow1Move = CCSequence::create(
            CCDelayTime::create(0.34f + f->extraDelay),
            CCEaseIn::create(CCMoveBy::create(0.25f, {300.0f, 0.0f}), 2.0f),
            nullptr
        );
        auto decoArrow1FadeOut = CCSequence::create(
            CCDelayTime::create(0.52f + f->extraDelay),
            CCFadeOut::create(0.07f),
            nullptr
        );
        auto decoArrow2Move = CCSequence::create(
            CCDelayTime::create(0.37f + f->extraDelay),
            CCEaseIn::create(CCMoveBy::create(0.30f, {300.0f, 0.0f}), 2.0f),
            nullptr
        );
        auto decoArrow2FadeOut = CCSequence::create(
            CCDelayTime::create(0.65f + f->extraDelay),
            CCFadeOut::create(0.02f),
            nullptr
        );
        auto decoArrow3Move = CCSequence::create(
            CCDelayTime::create(0.55f + f->extraDelay),
            CCEaseIn::create(CCMoveBy::create(0.37f, {300.0f, 0.0f}), 2.0f),
            nullptr
        );
        auto decoArrow3FadeOut = CCSequence::create(
            CCDelayTime::create(0.85f + f->extraDelay),
            CCFadeOut::create(0.07f),
            nullptr
        );

        auto timeTitleFadeIn = CCSequence::create(
            CCDelayTime::create(0.34f + f->extraDelay),
            CCFadeIn::create(0.07f),
            nullptr
        );
        auto timeTitleMove = CCSequence::create(
            CCDelayTime::create(0.34f + f->extraDelay),
            CCEaseIn::create(CCMoveBy::create(0.07f, {-6.0f, -6.0f}), 2.0f),
            nullptr
        );
        auto timeBarMove = CCSequence::create(
            CCDelayTime::create(0.37f + f->extraDelay),
            CCEaseIn::create(CCMoveBy::create(0.20f, {-350.0f, 0.0f}), 2.0f),
            nullptr
        );
        auto timeLabelAppear = CCSequence::create(
            CCDelayTime::create(0.67f + f->extraDelay),
            CCFadeIn::create(0.0f),
            nullptr
        );
        auto attsTitleFadeIn = CCSequence::create(
            CCDelayTime::create(0.56f + f->extraDelay),
            CCFadeIn::create(0.06f),
            nullptr
        );
        auto attsTitleMove = CCSequence::create(
            CCDelayTime::create(0.56f + f->extraDelay),
            CCEaseIn::create(CCMoveBy::create(0.06f, {-6.0f, -6.0f}), 2.0f),
            nullptr
        );
        auto attsBarMove = CCSequence::create(
            CCDelayTime::create(0.59f + f->extraDelay),
            CCEaseIn::create(CCMoveBy::create(0.20f, {-350.0f, 0.0f}), 2.0f),
            nullptr
        );
        auto attsLabelAppear = CCSequence::create(
            CCDelayTime::create(0.82f + f->extraDelay),
            CCFadeIn::create(0.0f),
            nullptr
        );
        auto jumpsTitleFadeIn = CCSequence::create(
            CCDelayTime::create(0.74f + f->extraDelay),
            CCFadeIn::create(0.08f),
            nullptr
        );
        auto jumpsTitleMove = CCSequence::create(
            CCDelayTime::create(0.74f + f->extraDelay),
            CCEaseIn::create(CCMoveBy::create(0.08f, {-6.0f, -6.0f}), 2.0f),
            nullptr
        );
        auto jumpsBarMove = CCSequence::create(
            CCDelayTime::create(0.77f + f->extraDelay),
            CCEaseIn::create(CCMoveBy::create(0.22f, {-350.0f, 0.0f}), 2.0f),
            nullptr
        );
        auto jumpsLabelAppear = CCSequence::create(
            CCDelayTime::create(0.99f + f->extraDelay),
            CCFadeIn::create(0.0f),
            nullptr
        );
        auto coinsTitleFadeIn = CCSequence::create(
            CCDelayTime::create(0.89f + f->extraDelay),
            CCFadeIn::create(0.08f),
            nullptr
        );
        auto coinsTitleMove = CCSequence::create(
            CCDelayTime::create(0.89f + f->extraDelay),
            CCEaseIn::create(CCMoveBy::create(0.08f, {-6.0f, -6.0f}), 2.0f),
            nullptr
        );
        auto coinsBarMove = CCSequence::create(
            CCDelayTime::create(0.92f + f->extraDelay),
            CCEaseIn::create(CCMoveBy::create(0.22f, {-350.0f, 0.0f}), 2.0f),
            nullptr
        );
        auto coinsLabelAppear = CCSequence::create(
            CCDelayTime::create(1.14f + f->extraDelay),
            CCFadeIn::create(0.0f),
            nullptr
        );
        auto totalTitleMove = CCSequence::create(
            CCDelayTime::create(1.09f + f->extraDelay),
            CCFadeIn::create(0.08f),
            nullptr
        );
        auto totalTitleFadeIn = CCSequence::create(
            CCDelayTime::create(1.09f + f->extraDelay),
            CCEaseIn::create(CCMoveBy::create(0.08f, {-6.0f, -6.0f}), 2.0f),
            nullptr
        );
        auto totalBarMove = CCSequence::create(
            CCDelayTime::create(1.34f + f->extraDelay),
            CCEaseIn::create(CCMoveBy::create(0.20f, {550.0f, 0.0f}), 2.0f),
            nullptr
        );
        auto totalLabelAppear = CCSequence::create(
            CCDelayTime::create(1.55f + f->extraDelay),
            CCFadeIn::create(0.0f),
            nullptr
        );
        auto rankTextFadeIn = CCSequence::create(
            CCDelayTime::create(2.60f + f->extraDelay),
            CCFadeIn::create(0.19f),
            nullptr
        );
        auto rankTextScaleUp = CCSequence::create(
            CCDelayTime::create(2.60f + f->extraDelay),
            CCScaleTo::create(0.19f, 1.9f),
            nullptr
        );
        auto rankSpriteScaleDown = CCSequence::create(
            CCDelayTime::create(2.79f + f->extraDelay),
            CCScaleTo::create(0.15f, 1.75f),
            nullptr
        );
        auto afterimageRankSpriteScaleDown = CCSequence::create(
            CCDelayTime::create(2.79f + f->extraDelay),
            CCScaleTo::create(0.15f, 1.75f),
            nullptr
        );
        auto afterimageRankSpriteFadeIn = CCSequence::create(
            CCDelayTime::create(2.79f + f->extraDelay),
            CCFadeIn::create(0.1f),
            nullptr
        );
        auto rankSpriteFadeOut = CCSequence::create(
            CCDelayTime::create(2.94f + f->extraDelay),
            CCFadeOut::create(0.f),
            nullptr
        );


        f->resultsBar->runAction(resultsBarMove);
        f->topArrow->runAction(decoArrow1Move);
        f->topArrow->runAction(decoArrow1FadeOut);
        f->topArrow2->runAction(decoArrow2Move);
        f->topArrow2->runAction(decoArrow2FadeOut);
        f->topArrow3->runAction(decoArrow3Move);
        f->topArrow3->runAction(decoArrow3FadeOut);
        f->timeTitle->runAction(timeTitleFadeIn);
        f->timeTitle->runAction(timeTitleMove);
        f->timerBar->runAction(timeBarMove);
        f->timeLabel->runAction(timeLabelAppear);
        f->attemptsTitle->runAction(attsTitleFadeIn);
        f->attemptsTitle->runAction(attsTitleMove);
        f->attemptsBar->runAction(attsBarMove);
        f->attemptsLabel->runAction(attsLabelAppear);
        f->jumpsTitle->runAction(jumpsTitleFadeIn);
        f->jumpsTitle->runAction(jumpsTitleMove);
        f->jumpsBar->runAction(jumpsBarMove);
        f->jumpsLabel->runAction(jumpsLabelAppear);
        f->coinsTitle->runAction(coinsTitleFadeIn);
        f->coinsTitle->runAction(coinsTitleMove);
        f->coinsBar->runAction(coinsBarMove);
        f->coinsLabel->runAction(coinsLabelAppear);
        f->totalTitle->runAction(totalTitleMove);
        f->totalTitle->runAction(totalTitleFadeIn);
        f->totalBar->runAction(totalBarMove);
        f->totalLabel->runAction(totalLabelAppear);
        f->rankText->runAction(rankTextFadeIn);
        f->rankText->runAction(rankTextScaleUp);
        f->rankingSprite->runAction(rankSpriteScaleDown);
        f->afterimageRankingSprite->runAction(afterimageRankSpriteScaleDown);
        f->afterimageRankingSprite->runAction(afterimageRankSpriteFadeIn);
        f->rankingSprite->runAction(rankSpriteFadeOut);

        f->menu->runAction(showKeybinds);

        int attempts = m_playLayer->m_attempts;
        if (attempts > 16) {
            auto shitRankLmao = CCSequence::create(
                CCDelayTime::create(3.45f + f->extraDelay),
                CCEaseBounceOut::create(CCRotateBy::create(1.0f, 35.0f)),
                nullptr
            );
            auto lowKeyFellOffBro = CCSequence::create(
                CCDelayTime::create(3.5f + f->extraDelay),
                CCEaseBounceOut::create(CCMoveBy::create(0.75f, {0.0f, -15.0f})),
                nullptr
            );
            f->afterimageRankingSprite->runAction(shitRankLmao);
            f->afterimageRankingSprite->runAction(lowKeyFellOffBro);
        }

        this->scheduleOnce(schedule_selector(SonicUnleashed::rankReaction), 3.3f + f->extraDelay);
        this->scheduleOnce(schedule_selector(SonicUnleashed::rankPlacement), 2.94f + f->extraDelay);
        this->scheduleOnce(schedule_selector(SonicUnleashed::totalScoreSound), 1.4f + f->extraDelay);

        this->scheduleOnce(schedule_selector(SonicUnleashed::slide1), 0.59f + f->extraDelay);
        this->scheduleOnce(schedule_selector(SonicUnleashed::slide2), 0.77f + f->extraDelay);
        this->scheduleOnce(schedule_selector(SonicUnleashed::slide3), 0.92f + f->extraDelay);

        if (stageClearDo) {
            this->scheduleOnce(schedule_selector(SonicUnleashed::stageClear), f->extraDelay / 2);
            if (doResultsMusic) {
                this->scheduleOnce(schedule_selector(SonicUnleashed::rankingMusic), 5.65f + f->extraDelay);
            }
        }
        
    }
};