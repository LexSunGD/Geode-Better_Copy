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
        // Obtenemos el nivel actual de la capa original
        GJGameLevel* currentLevel = m_level;
        if (!currentLevel) return;

        auto levelManager = LocalLevelManager::sharedState();
        if (!levelManager) return;

        // 1. Creamos un nuevo objeto de nivel local en blanco
        GJGameLevel* clonedLevel = levelManager->createNewLevel();
        if (!clonedLevel) {
            FLAlertLayer::create("Error", "No se pudo crear el nivel local.", "OK")->show();
            return;
        }

        // 2. Copiamos las estructuras esenciales de datos (el mapa/diseño)
        clonedLevel->m_levelString = currentLevel->m_levelString;
        clonedLevel->m_levelName = currentLevel->m_levelName;
        clonedLevel->m_levelDesc = currentLevel->m_levelDesc;
        clonedLevel->m_creatorName = currentLevel->m_creatorName;
        clonedLevel->m_audioTrack = currentLevel->m_audioTrack;
        clonedLevel->m_songID = currentLevel->m_songID;

        // 3. Forzamos la preservación exacta de los metadatos solicitados
        clonedLevel->m_levelID = currentLevel->m_levelID;       // ID del nivel online original
        clonedLevel->m_levelVersion = currentLevel->m_levelVersion; // Versión del nivel
        clonedLevel->m_accountID = currentLevel->m_accountID;   // ID de cuenta del creador
        clonedLevel->m_userID = currentLevel->m_userID;         // ID de usuario del creador
        clonedLevel->m_password = currentLevel->m_password;     // Contraseña (si posee)
        clonedLevel->m_originalLevel = currentLevel->m_originalLevel; // ID original del nivel (si es un reupload)
        
        // Configuramos propiedades de guardado local para que el juego lo reconozca
        clonedLevel->m_levelType = GJLevelType::Local;

        // 4. Guardamos los cambios inmediatamente en el gestor local del juego
        levelManager->updateLevelOrder();
        
        // Notificación de éxito en pantalla
        FLAlertLayer::create(
            "Éxito", 
            "Nivel clonado con <cg>todos sus metadatos originales</c>. Revisa tus Saved Levels.", 
            "OK"
        )->show();
    }
};
