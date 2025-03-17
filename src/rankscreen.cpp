#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/EndLevelLayer.hpp>
#include <Geode/binding/GJGameLevel.hpp>
#include <Geode/binding/GJBaseGameLayer.hpp>
#include <Geode/loader/SettingV3.hpp>
#include "helper.hpp"
#include <random>

using namespace geode::prelude;

int genRandomInt(int min, int max) {
    std::random_device rd;  // Obtain a random number from hardware
    std::mt19937 gen(rd()); // Seed the generator
    std::uniform_int_distribution<> distr(min, max); // Define the range

    return distr(gen);
}