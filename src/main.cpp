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

    	// 1. Check if enabled
    	if (!Mod::get()->getSettingValue<bool>("enabled")) return;

    	// 2. Check practice mode requirement
    	bool practiceOnly = Mod::get()->getSettingValue<bool>("practice-mode-only");
    	if (practiceOnly && !m_isPracticeMode) return;

    	// 3. Safety checks: Ensure player exists, isn't dead, and game isn't paused
    	// We removed m_hasStartedLevel because it doesn't exist.
    	if (!m_player1 || m_player1->m_isDead || m_isPaused) return;

    	// 4. Logic
    	m_fields->m_timeSinceLastCheckpoint += dt;

    	// Use double to match your mod.json float/double type
    	double interval = Mod::get()->getSettingValue<double>("interval");

    	if (m_fields->m_timeSinceLastCheckpoint >= static_cast<float>(interval)) {
        	if (m_isPracticeMode) {
            	this->createCheckpoint();
            	m_fields->m_timeSinceLastCheckpoint = 0.0f;
            
        	    // Using fmt-style logging which is standard in Geode
        	    log::debug("Auto-placed checkpoint at {}", m_levelTime);
        	} else {
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
