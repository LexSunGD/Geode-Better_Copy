#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

using namespace geode::prelude;

class $modify(MyLevelInfoLayer, LevelInfoLayer) {
    bool init(GJGameLevel* level, bool p1) {
        if (!LevelInfoLayer::init(level, p1)) return false;

        // 1. Obtener el menú lateral izquierdo mediante ID seguro (Geode Node IDs)
        auto menu = this->getChildByID("left-side-menu");
        
        if (menu) {
            // Creamos el sprite usando la textura nativa de duplicar de GD
            auto sprite = CCSprite::createWithSpriteFrameName("GJ_duplicateBtn_001.png");
            sprite->setColor({ 255, 140, 0 }); // Tono naranja brillante para identificar que es la copia completa
            
            auto myBtn = CCMenuItemSpriteExtra::create(
                sprite,
                this,
                menu_selector(MyLevelInfoLayer::onFullMetadataCopy)
            );
            
            myBtn->setID("full-copy-button"_spr);
            menu->addChild(myBtn);
            menu->updateLayout();
        }

        return true;
    }

    void onFullMetadataCopy(CCObject* sender) {
        GJGameLevel* originalLevel = this->m_level;
        if (!originalLevel) return;

        auto localLevelManager = LocalLevelManager::sharedState();
        
        // 2. Crear la instancia de clonación nativa
        auto clonedLevel = localLevelManager->createNewLevel(originalLevel);

        if (clonedLevel) {
            // 3. Forzar e inyectar TODOS los metadatos de la 2.2081
            clonedLevel->m_levelID = originalLevel->m_levelID;
            clonedLevel->m_originalLevelID = originalLevel->m_originalLevelID;
            clonedLevel->m_copiedID = originalLevel->m_copiedID;
            
            // Tiempos exactos del editor y revisiones
            clonedLevel->m_workingTime = originalLevel->m_workingTime;
            clonedLevel->m_workingTime2 = originalLevel->m_workingTime2;
            clonedLevel->m_levelVersion = originalLevel->m_levelVersion;
            clonedLevel->m_revision = originalLevel->m_revision;
            clonedLevel->m_password = originalLevel->m_password;
            
            // Datos del Autor e Identificadores de red
            clonedLevel->m_userID = originalLevel->m_userID;
            clonedLevel->m_accountID = originalLevel->m_accountID;
            clonedLevel->m_creatorName = originalLevel->m_creatorName;
            
            // Monedas y estadísticas registradas (Por si deseas preservar las verificadas de la 2.2)
            clonedLevel->m_coins = originalLevel->m_coins;
            clonedLevel->m_coinsVerified = originalLevel->m_coinsVerified;
            
            // 4. Mover el nivel explícitamente al principio de la lista de creados ("Saved")
            auto localLevels = localLevelManager->m_localLevels;
            if (localLevels) {
                // Eliminarlo de donde se creó por defecto en el array
                localLevels->removeObject(clonedLevel, false);
                // Insertarlo en el índice 0 para que aparezca arriba de todo en tu lista de creación
                localLevels->insertObject(clonedLevel, 0);
            }

            // Forzar al juego a escribir los datos modificados en el save de tu PC/Dispositivo
            localLevelManager->updateLevelOrder();

            FLAlertLayer::create(
                "Copia Exitosa", 
                "El nivel se clonó conservando IDs, metadatos y tiempo. Revisa tu lista de niveles creados.", 
                "OK"
            )->show();
        } else {
            FLAlertLayer::create(
                "Error", 
                "No se pudo inicializar la clonación de datos del nivel.", 
                "OK"
            )->show();
        }
    }
};
