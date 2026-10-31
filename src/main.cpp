#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

using namespace geode::prelude;

class $modify(MyLevelInfoLayer, LevelInfoLayer) {
    bool init(GJGameLevel* level, bool isGauntlet) {
        if (!LevelInfoLayer::init(level, isGauntlet)) return false;

        if (auto leftMenu = typeinfo_cast<CCMenu*>(this->getChildByID("left-menu"))) {
            auto btnSprite = CCSprite::createWithSpriteFrameName("GJ_duplicateBtn_001.png");
            if (btnSprite) btnSprite->setColor({0, 150, 255});

            auto copyBtn = CCMenuItemSpriteExtra::create(
                btnSprite ? static_cast<CCNode*>(btnSprite) : ButtonSprite::create("Clonar Exacto"),
                this, menu_selector(MyLevelInfoLayer::onCloneExactLevel)
            );
            copyBtn->setID("exact-copy-button");

            leftMenu->addChild(copyBtn);
            leftMenu->updateLayout();
        }
        return true;
    }

    void onCloneExactLevel(CCObject* sender) {
        if (!m_level) return;

        GJGameLevel* cloned = GJGameLevel::create();
        if (!cloned) {
            FLAlertLayer::create("Error", "The level could not be copied.", "OK")->show();
            return;
        }

        // Copia compacta de propiedades esenciales
        cloned->m_levelString = m_level->m_levelString;
        cloned->m_levelName = m_level->m_levelName;
        cloned->m_levelDesc = m_level->m_levelDesc;
        cloned->m_creatorName = m_level->m_creatorName;
        cloned->m_audioTrack = m_level->m_audioTrack;
        cloned->m_songID = m_level->m_songID;
        cloned->m_levelID = m_level->m_levelID;       
        cloned->m_levelVersion = m_level->m_levelVersion; 
        cloned->m_accountID = m_level->m_accountID;   
        cloned->m_userID = m_level->m_userID;         
        cloned->m_password = m_level->m_password;         
        cloned->m_levelType = GJLevelType::Editor;

        if (auto localLevels = LocalLevelManager::sharedState()->m_localLevels) {
            localLevels->insertObject(cloned, 0);
        }

        LocalLevelManager::sharedState()->updateLevelOrder();
        FLAlertLayer::create("Success", "The level has been successfully copied.", "OK")->show();
    }
};
