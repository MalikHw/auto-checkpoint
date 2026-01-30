#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

class $modify(AutoCheckpointPlayLayer, PlayLayer) {
    struct Fields {
        float m_timeSinceLastCheckpoint = 0.0f;
        bool m_hasPlacedInitialCheckpoint = false;
    };

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) {
            return false;
        }

        // Reset our timer when level starts
        m_fields->m_timeSinceLastCheckpoint = 0.0f;
        m_fields->m_hasPlacedInitialCheckpoint = false;

        return true;
    }

    void update(float dt) {
        PlayLayer::update(dt);

        // Check if auto-checkpoint is enabled
        if (!Mod::get()->getSettingValue<bool>("enabled")) {
            return;
        }

        // Check if we should only work in practice mode
        bool practiceOnly = Mod::get()->getSettingValue<bool>("practice-mode-only");
        if (practiceOnly && !m_isPracticeMode) {
            return;
        }

        // Don't place checkpoints if the level hasn't started yet
        if (!m_hasStartedLevel) {
            return;
        }

        // Don't place checkpoints if the player is dead
        if (m_player1->m_isDead || (m_player2 && m_player2->m_isDead)) {
            return;
        }

        // Accumulate time
        m_fields->m_timeSinceLastCheckpoint += dt;

        // Get the interval from settings
        float interval = Mod::get()->getSettingValue<double>("interval");

        // Check if it's time to place a checkpoint
        if (m_fields->m_timeSinceLastCheckpoint >= interval) {
            // Only place checkpoint if we're in practice mode (can't place checkpoints in normal mode)
            if (m_isPracticeMode) {
                // Place the checkpoint using the game's built-in function
                this->addToSection(m_player1->m_position.x);
                
                // Reset timer
                m_fields->m_timeSinceLastCheckpoint = 0.0f;
                
                log::debug("Auto-placed checkpoint at {:.2f}s", m_gameState.m_levelTime);
            } else {
                // If not in practice mode, just reset timer to avoid spam
                m_fields->m_timeSinceLastCheckpoint = 0.0f;
            }
        }
    }

    void resetLevel() {
        PlayLayer::resetLevel();
        
        // Reset our timer when level is reset
        m_fields->m_timeSinceLastCheckpoint = 0.0f;
    }

    void onQuit() {
        PlayLayer::onQuit();
        
        // Clean up when quitting
        m_fields->m_timeSinceLastCheckpoint = 0.0f;
    }
};
