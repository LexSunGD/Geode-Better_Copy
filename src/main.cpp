#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

using namespace geode::prelude;

class $modify(MyLevelInfoLayer, LevelInfoLayer) {
    bool init(GJGameLevel* level, bool isGauntlet) {
        if (!LevelInfoLayer::init(level, isGauntlet)) return false; //

        // Buscamos el menú lateral derecho usando Node IDs de forma segura
        auto rightMenu = this->getChildByID("right-side-menu");
        if (!rightMenu) return true; 

        // Creamos el sprite para nuestro botón (usaremos un icono nativo de copiar)
        auto btnSprite = CCSprite::createWithSpriteFrameName("GJ_duplicateBtn_001.png");
        if (!btnSprite) {
            btnSprite = ButtonSprite::create("Clonar Exacto"); // Fallback si no encuentra el sprite
        }

        // Creamos el botón vinculando la función callback
        auto copyBtn = CCMenuItemSpriteExtra::create(
            btnSprite,
            this,
            menu_selector(MyLevelInfoLayer::onCloneExactLevel)
        );
        copyBtn->setID("exact-copy-button");

        // Agregamos el botón al menú y actualizamos la disposición visual
        rightMenu->addChild(copyBtn);
        rightMenu->updateLayout();

        return true;
    }

        void onCloneExactLevel(CCObject* sender) {
        GJGameLevel* currentLevel = m_level;
        if (!currentLevel) return;

        auto levelManager = LocalLevelManager::sharedState();
        if (!levelManager) return;

        // CORRECCIÓN 1: Instanciar el nivel usando create() en lugar de un método de levelManager
        GJGameLevel* clonedLevel = GJGameLevel::create();
        if (!clonedLevel) {
            FLAlertLayer::create("Error", "No se pudo crear el nivel local.", "OK")->show();
            return;
        }

        // Copiamos las estructuras esenciales de datos
        clonedLevel->m_levelString = currentLevel->m_levelString;
        clonedLevel->m_levelName = currentLevel->m_levelName;
        clonedLevel->m_levelDesc = currentLevel->m_levelDesc;
        clonedLevel->m_creatorName = currentLevel->m_creatorName;
        clonedLevel->m_audioTrack = currentLevel->m_audioTrack;
        clonedLevel->m_songID = currentLevel->m_songID;

        // Forzamos la preservación exacta de los metadatos solicitados
        clonedLevel->m_levelID = currentLevel->m_levelID;       
        clonedLevel->m_levelVersion = currentLevel->m_levelVersion; 
        clonedLevel->m_accountID = currentLevel->m_accountID;   
        clonedLevel->m_userID = currentLevel->m_userID;         
        clonedLevel->m_password = currentLevel->m_password;     
        clonedLevel->m_originalLevel = currentLevel->m_originalLevel; 
        
        // CORRECCIÓN 2: El enum correcto en el código moderno es 'Editor' para tus niveles locales
        clonedLevel->m_levelType = GJLevelType::Editor;

        // Agregamos el nivel creado al arreglo de niveles locales
        auto localLevels = levelManager->m_localLevels;
        if (localLevels) {
            localLevels->insertObject(clonedLevel, 0); // Insertar al inicio de la lista
        }

        // Guardamos los cambios en el gestor local
        levelManager->updateLevelOrder();
        
        FLAlertLayer::create(
            "Éxito", 
            "Nivel clonado con <cg>todos sus metadatos originales</c>. Revisa tus Saved/Local Levels.", 
            "OK"
        )->show();
}
