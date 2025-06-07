#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/EffectGameObject.hpp>
#include <Geode/modify/EnhancedGameObject.hpp>
#include <Geode/modify/GameObject.hpp>
#include <unordered_set>
#include <unordered_map>

using namespace geode::prelude;

auto homingReticleScale = Mod::get()->getSettingValue<double>("reticle-scale");

$on_mod(Loaded) {
    listenForSettingChanges("reticle-scale", [](double value) {
        homingReticleScale = value;
    });
}

float sensitivity = 120.0f;
const float multiplier = 2.0f;

const std::unordered_set<GameObjectType> validObjectTypes = {
    GameObjectType::YellowJumpRing,
    GameObjectType::PinkJumpRing,
    GameObjectType::GravityRing,
    GameObjectType::GreenRing,
    GameObjectType::RedJumpRing,
    GameObjectType::DashRing,
    GameObjectType::GravityDashRing,
    GameObjectType::SpiderOrb,
    GameObjectType::DropRing
};

const std::unordered_set<GameObjectType> validSimplePortalTypes = {
    GameObjectType::InverseGravityPortal,
    GameObjectType::NormalGravityPortal,
    GameObjectType::InverseMirrorPortal,
    GameObjectType::NormalMirrorPortal,
    GameObjectType::DualPortal,
    GameObjectType::SoloPortal,
    GameObjectType::GravityTogglePortal
};

const std::unordered_set<GameObjectType> validRainbowPortalTypes = {
    GameObjectType::ShipPortal,
    GameObjectType::CubePortal,
    GameObjectType::BallPortal,
    GameObjectType::UfoPortal,
    GameObjectType::WavePortal,
    GameObjectType::RobotPortal,
    GameObjectType::SpiderPortal,
    GameObjectType::SwingPortal
};

std::unordered_set<GameObject*> triggeredObjects;
std::unordered_map<GameObject*, CCNodeRGBA*> objectNodes;

class $modify(UnleashedBaseLayer, GJBaseGameLayer) {

    void forceScale(float dt) {
        setScale(homingReticleScale);
    }

    void collisionCheckObjects(PlayerObject* player, gd::vector<GameObject*>* objs, int uhh, float v1) {
        GJBaseGameLayer::collisionCheckObjects(player, objs, uhh, v1);

        for (int i = 0; i < uhh; i++) {
            GameObject* obj = objs->at(i);
            if (!obj || obj->m_isGroupDisabled) continue;
            if (validObjectTypes.find(obj->m_objectType) == validObjectTypes.end()) continue;
            if ((obj->m_isHide || obj->getOpacity() < 1)) continue;

            CCRect sensitivityRect = CCRect(obj->getObjectRect().origin - CCPoint(sensitivity, sensitivity), obj->getObjectRect().size + CCPoint(sensitivity * multiplier, sensitivity * multiplier));
            bool isInRange = player->getObjectRect().intersectsRect(sensitivityRect);

            if (isInRange) {
                // Check if the object has already triggered the sound
                if (triggeredObjects.find(obj) == triggeredObjects.end()) {
                    auto fmod = FMODAudioEngine::sharedEngine();
                    fmod->playEffect("targetLock.ogg"_spr);
                    triggeredObjects.insert(obj); // Mark the object as triggered

                    auto orbContentSize = obj->getContentSize();
                    auto color = ccc3(0, 255, 0);

                    // fix?
                    auto objParent = obj->getParent();
                    if (objParent) {
                        int parentZOrder = obj->getParent()->getZOrder() + 1;
                        objParent->setZOrder(parentZOrder + 20);
                    }

                    // node that holds the sprites
                    auto node = CCNodeRGBA::create();

                    // all 4 homing reticle sprites
                    auto middleSprite = CCSprite::createWithSpriteFrameName("targetCenter.png"_spr);
                    auto topSprite = CCSprite::createWithSpriteFrameName("targetTop.png"_spr);
                    auto bottomSprite = CCSprite::createWithSpriteFrameName("targetBottom.png"_spr);
                    auto leftSprite = CCSprite::createWithSpriteFrameName("targetLeft.png"_spr);
                    auto rightSprite = CCSprite::createWithSpriteFrameName("targetRight.png"_spr);

                    topSprite->setPosition({0, 25});
                    bottomSprite->setPosition({0, -25});
                    leftSprite->setPosition({-25, 0});
                    rightSprite->setPosition({25, 0});

                    node->addChild(topSprite);
                    node->addChild(bottomSprite);
                    node->addChild(leftSprite);
                    node->addChild(rightSprite);
                    node->addChild(middleSprite);

                    // node shit
                    node->setPosition({orbContentSize.width / 2, orbContentSize.height / 2});
                    //node->setScale(homingReticleScale);
                    node->schedule(schedule_selector(UnleashedBaseLayer::forceScale));
                    node->setCascadeColorEnabled(true);
                    node->setColor(color);
                    node->setID("target-lock"_spr);
                    obj->addChild(node, 1);

                    // animations woo hoo
                    topSprite->runAction( CCMoveBy::create(0.1f, {0, -25}) );
                    bottomSprite->runAction( CCMoveBy::create(0.1f, {0, 25}) );
                    leftSprite->runAction( CCMoveBy::create(0.1f, {25, 0}) );
                    rightSprite->runAction( CCMoveBy::create(0.1f, {-25, 0}) );

                    objectNodes[obj] = node;
                }
            }
        }
    }
};

class $modify(UnleashedEffectGameObject, EffectGameObject) {
    void customSetup() {
        EffectGameObject::customSetup();

        if (validObjectTypes.find(m_objectType) != validObjectTypes.end()) {
            m_hasSpecialChild = true;
        }
    }
};

class $modify(UnleashedGameObject, GameObject) {
    void resetObject() {
        GameObject::resetObject();

        if (validObjectTypes.find(m_objectType) != validObjectTypes.end() && m_hasSpecialChild) {
            auto node = getChildByID("target-lock"_spr);
            if (triggeredObjects.find(this) != triggeredObjects.end()) {
                triggeredObjects.erase(this);
            }
            if (node) {
                this->removeChild(node, true);
            }
        }
    }
};

class $modify(UnleashedEnhancedGameObject, EnhancedGameObject) {
    void activatedByPlayer(PlayerObject* p0) {
        EnhancedGameObject::activatedByPlayer(p0);

        if (validObjectTypes.find(m_objectType) != validObjectTypes.end() && m_hasSpecialChild) {
            auto node = getChildByID("target-lock"_spr);
            if (node) {
                this->removeChild(node, true);
            }
        }
    }
};