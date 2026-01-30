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

		if (!Mod::get()->getSettingValue<bool>("enabled")) return;

		// Correct practice mode check
		bool practiceOnly = Mod::get()->getSettingValue<bool>("practice-mode-only");
		if (practiceOnly && !m_isPracticeMode) return;

		if (!m_hasStartedLevel || m_player1->m_isDead) return;

		m_fields->m_timeSinceLastCheckpoint += dt;

		double interval = Mod::get()->getSettingValue<double>("interval");

		if (m_fields->m_timeSinceLastCheckpoint >= static_cast<float>(interval)) {
			// createCheckpoint() only works if m_isPracticeMode is true
			if (m_isPracticeMode) {
				this->createCheckpoint();
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
