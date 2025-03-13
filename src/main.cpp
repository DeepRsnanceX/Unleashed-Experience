#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/EndLevelLayer.hpp>
#include <Geode/binding/GJGameLevel.hpp>
#include <Geode/loader/SettingV3.hpp>
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
    void incrementJumps() {
        PlayerObject::incrementJumps();
    }
}

class $modify(PlayLayer) {
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        if (unleashedSounds) {
            auto fmod = FMODAudioEngine::sharedEngine();
            fmod->playEffect("herewe.ogg"_spr);
        }
        
        return true;
    }

    void startGame() {
        PlayLayer::startGame();

        if (unleashedSounds) {
            auto fmod = FMODAudioEngine::sharedEngine();
            fmod->playEffect("go.ogg"_spr);
        }
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

        if (unleashedRankings) {
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
        }
        
    }

    void onHideLayer(CCObject* sender) {
        EndLevelLayer::onHideLayer(sender);
        geode::log::debug("when escondes el endlevel layer");
    }

    void showLayer(bool p0) {
        EndLevelLayer::showLayer(p0);

        if (unleashedRankings) {
            auto ranking = this->getChildByID("rank-sprite"_spr);

            // animations
            auto fadeIn = CCFadeIn::create(0.3f);
            auto scaleDown = CCScaleTo::create(0.3, 1.75f);

            ranking->runAction(fadeIn);
            ranking->runAction(scaleDown);
        }
    }

};