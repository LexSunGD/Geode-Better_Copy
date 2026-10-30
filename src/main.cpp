#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

using namespace geode::prelude;

class $modify(MyLevelInfoLayer, LevelInfoLayer) {
    bool init(GJGameLevel* level, bool isGauntlet) {
        if (!LevelInfoLayer::init(level, isGauntlet)) return false;

        auto leftMenu = this->getChildByID("left-side-menu");
        if (!leftMenu) return true; 

        auto btnSprite = CCSprite::createWithSpriteFrameName("GJ_duplicateBtn_001.png");
        if (!btnSprite) {
            btnSprite = ButtonSprite::create("Clonar Exacto");
        } else {
            btnSprite->setColor(ccColor3B{0, 150, 255});
        }

        auto copyBtn = CCMenuItemSpriteExtra::create(
            btnSprite,
            this,
            menu_selector(MyLevelInfoLayer::onCloneExactLevel)
        );
        copyBtn->setID("exact-copy-button");

        leftMenu->addChild(copyBtn);
        leftMenu->updateLayout();

        return true;
    }

    void onCloneExactLevel(CCObject* sender) {
        GJGameLevel* currentLevel = m_level;
        if (!currentLevel) return;

        auto levelManager = LocalLevelManager::sharedState();
        if (!levelManager) return;

        GJGameLevel* clonedLevel = GJGameLevel::create();
        if (!clonedLevel) {
            FLAlertLayer::create("Error", "No se pudo crear el nivel local.", "OK")->show();
            return;
        }

        clonedLevel->m_levelString = currentLevel->m_levelString;
        clonedLevel->m_levelName = currentLevel->m_levelName;
        clonedLevel->m_levelDesc = currentLevel->m_levelDesc;
        clonedLevel->m_creatorName = currentLevel->m_creatorName;
        clonedLevel->m_audioTrack = currentLevel->m_audioTrack;
        clonedLevel->m_songID = currentLevel->m_songID;

        clonedLevel->m_levelID = currentLevel->m_levelID;       
        clonedLevel->m_levelVersion = currentLevel->m_levelVersion; 
        clonedLevel->m_accountID = currentLevel->m_accountID;   
        clonedLevel->m_userID = currentLevel->m_userID;         
        clonedLevel->m_password = currentLevel->m_password;     
        
        clonedLevel->m_levelType = GJLevelType::Editor;

        auto localLevels = levelManager->m_localLevels;
        if (localLevels) {
            localLevels->insertObject(clonedLevel, 0);
        }

        levelManager->updateLevelOrder();
        
        FLAlertLayer::create(
            "Level Cloned", 
            "A <c:0,150,255>copy</c> of this level has been created in your local <cg>levels</c>.", 
            "OK"
        )->show();
    }
};
