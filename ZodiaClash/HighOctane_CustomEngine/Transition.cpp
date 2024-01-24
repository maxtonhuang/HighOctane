#include "Transition.h"
#include "Global.h"
#include "Events.h"
#include "EntityFactory.h"

bool transitionActive{ false };
bool transitionType{};
const float TRANSITION_TIME{ 1.2f };
std::string transitionNextScene{};

const std::string TRANSITION_FADEIN_PREFAB{ "transition_fadein.prefab" };
const std::string TRANSITION_FADEOUT_PREFAB{ "transition_fadeout.prefab" };

void TransitionSystem::Update() {
	if (transitionActive) {
		if (timer > 0.f) {
			timer -= FIXED_DT;
			if (timer < 0.f) {
				if (transitionType) {
					events.Call("Change Scene", transitionNextScene);
					transitionType = false;
				}
				else {
					transitionActive = false;
				}
			}
		}
		else {
			timer = TRANSITION_TIME;
			if (transitionType) {
				EntityFactory::entityFactory().ClonePrefab(TRANSITION_FADEOUT_PREFAB);
			}
			else {
				EntityFactory::entityFactory().ClonePrefab(TRANSITION_FADEIN_PREFAB);
			}
		}
	}
}
