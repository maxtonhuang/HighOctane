/******************************************************************************
*
*	\copyright
*		All content(C) 2023/2024 DigiPen Institute of Technology Singapore.
*		All rights reserved. Reproduction or disclosure of this file or its
*		contents without the prior written consent of DigiPen Institute of
*		Technology is prohibited.
*
* *****************************************************************************
*
*	@file		Serialization.cpp
*
*	@author		Kai Alexander Van Adrichem Boogaert
*
*	@email		kaialexander.v\@digipen.edu
*
*   @co-author	Foong Pun Yuen Nigel (Prefab and parent-child serialisation as well as animation serialisation)
*
*	@email		p.foong\@digipen.edu
* 
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		10 September 2023
*
* *****************************************************************************
*
*	@brief		
*
*	This file contains all the definitions for serialize and dersiarialize.
*	It also has helper functions as well to serialize specific components.
* 
******************************************************************************/
#include "Serialization.h"
#include <rapidjson-master/include/rapidjson/document.h>
#include <rapidjson-master/include/rapidjson/writer.h>
#include <rapidjson-master/include/rapidjson/prettywriter.h>
#include <rapidjson-master/include/rapidjson/stringbuffer.h>
#include <rapidjson-master/include/rapidjson/istreamwrapper.h>
#include "EntityFactory.h"
#include "model.h"
#include "Global.h"
#include "AssetManager.h"
#include "CharacterStats.h"
#include "UIComponents.h"
#include <memory>
#include "Layering.h"
#include "Animation.h"
#include "Particles.h"

//extern std::unordered_map<std::string, Entity> masterEntitiesList;

//extern std::vector<VariableInfo> variablesTEST;

bool Serializer::Open(const std::string& file)
{
	stream.open(file.c_str());
	return stream.is_open();
}

bool Serializer::IsGood()
{
	return stream.good();
}

void Serializer::ReadInt(int& i)
{
	stream >> i;
}

void Serializer::ReadFloat(float& f)
{
	stream >> f;
}

void Serializer::ReadString(std::string& str)
{
	std::getline(stream, str);
}


void StreamRead(Serializer& stream, float& f)
{
	stream.ReadFloat(f);
}

void StreamRead(Serializer& stream, int& i)
{
	stream.ReadInt(i);
}

void StreamRead(Serializer& stream, std::string& str)
{
	stream.ReadString(str);
}

void Serializer::SerializeCSV(const std::string& file) {
	Serializer stream;
	ASSERT(!(stream.Open(file)), "Unable to open file");
	if (!stream.Open(file)) {
		ASSERT(true, "Unable to open file");
		return;
	}
	std::string line, header;
	PseudoCharacter characterStats;
	std::getline(stream.stream, header);  // Read the header line and discard it
	char c;
	std::string field;

	while (!stream.stream.eof()) {
		field.clear();
		// Read characters until a ',' or newline is encountered
		while (stream.stream.get(c) && c != ',' && c != '\n') {
			field += c;
		}

		// Process the field based on the context (e.g., assign to the appropriate member)
		if (!field.empty()) {
			if (characterStats.id == 0) {
				characterStats.id = std::stoi(field);
			}
			else if (characterStats.name.empty()) {
				characterStats.name = field;
			}
			else if (characterStats.health == 0.0f) {
				characterStats.health = std::stof(field);
			}
			else if (characterStats.attack == 0.0f) {
				characterStats.attack = std::stof(field);
			}
			else if (characterStats.defence == 0.0f) {
				characterStats.defence = std::stof(field);
			}
			else if (characterStats.speed == 0.0f) {
				characterStats.speed = std::stof(field);
			}
		}
		// Check for newline to indicate the end of a line
		if (c == '\n' || stream.stream.eof()) {
			// Reset characterStats for the next line
			characterStats = PseudoCharacter();
		}
	}
}

rapidjson::Value SerializeName(const Name& entityName, rapidjson::Document::AllocatorType& allocator) {
	
	rapidjson::Value nameObject(rapidjson::kObjectType);
	rapidjson::Value nameValue;
	nameValue.SetString(entityName.name.c_str(), static_cast<rapidjson::SizeType>(entityName.name.length()), allocator);
	nameObject.AddMember("Name", nameValue, allocator);
	//nameObject.AddMember("Selected", entityName.selected, allocator);
	nameObject.AddMember("Current Layer", static_cast<uint64_t>(entityName.serializationLayer), allocator);
	nameObject.AddMember("Order in Layer", static_cast<uint64_t>(entityName.serializationOrderInLayer), allocator);
	nameObject.AddMember("isLocked", entityName.lock, allocator);
	nameObject.AddMember("isSkipped", entityName.skip, allocator);

	return nameObject;

}

rapidjson::Value SerializeTransform(const Transform& transform, rapidjson::Document::AllocatorType& allocator) {
	rapidjson::Value transformObject(rapidjson::kObjectType);
	transformObject.AddMember("position_x", transform.position.x, allocator);
	transformObject.AddMember("position_y", transform.position.y, allocator);
	transformObject.AddMember("rotation", transform.rotation, allocator);
	transformObject.AddMember("scale", transform.scale, allocator);
	transformObject.AddMember("velocity_x", transform.velocity.x, allocator);
	transformObject.AddMember("velocity_y", transform.velocity.y, allocator);
	return transformObject;
}

rapidjson::Value SerializeSize(const Size& size, rapidjson::Document::AllocatorType& allocator) {
	rapidjson::Value sizeObject(rapidjson::kObjectType);
	sizeObject.AddMember("width", size.width, allocator);
	sizeObject.AddMember("height", size.height, allocator);
	return sizeObject;
}

rapidjson::Value SerializeColor(const Color& color, rapidjson::Document::AllocatorType& allocator) {
	rapidjson::Value colorObject(rapidjson::kObjectType);
	colorObject.AddMember("r", color.color.r, allocator);
	colorObject.AddMember("g", color.color.g, allocator);
	colorObject.AddMember("b", color.color.b, allocator);
	colorObject.AddMember("a", color.color.a, allocator);
	return colorObject;
}

rapidjson::Value SerializeParticle(const Emitter& emitter, rapidjson::Document::AllocatorType& allocator) {
	rapidjson::Value emitterObject(rapidjson::kObjectType);
	emitterObject.AddMember("Emitter Lifetime", emitter.emitterLifetime, allocator);
	emitterObject.AddMember("Emitter PositionX", emitter.position.x, allocator);
	emitterObject.AddMember("Emitter PositionY", emitter.position.y, allocator);
	emitterObject.AddMember("Particles SizeX", emitter.size.x, allocator);
	emitterObject.AddMember("Particles SizeY", emitter.size.y, allocator);
	emitterObject.AddMember("Particles ColorR", emitter.particleColor.color.r, allocator);
	emitterObject.AddMember("Particles ColorG", emitter.particleColor.color.g, allocator);
	emitterObject.AddMember("Particles ColorB", emitter.particleColor.color.b, allocator);
	emitterObject.AddMember("Particles ColorA", emitter.particleColor.color.a, allocator);
	emitterObject.AddMember("Frequency", emitter.frequency, allocator);
	emitterObject.AddMember("Particle Lifetime", emitter.particleLifetime, allocator);
	emitterObject.AddMember("Rotation", emitter.rotation, allocator);
	emitterObject.AddMember("Rotation Speed", emitter.rotationSpeed, allocator);
	emitterObject.AddMember("VelocityX", emitter.velocity.x, allocator);
	emitterObject.AddMember("VelocityY", emitter.velocity.y, allocator);
	emitterObject.AddMember("Single Sided", emitter.singleSided, allocator);

	rapidjson::Value particleTextures(rapidjson::kArrayType);
	for (std::string const& a : emitter.textures) {
		rapidjson::Value tex;
		tex.SetString(a.c_str(), static_cast<rapidjson::SizeType>(a.length()), allocator);
		particleTextures.PushBack(tex, allocator);
	}
	emitterObject.AddMember("Textures", particleTextures, allocator);

	return emitterObject;
}

rapidjson::Value SerializeTex(const Tex& tex, rapidjson::Document::AllocatorType& allocator) {
	rapidjson::Value texObject(rapidjson::kObjectType);
	texObject.AddMember("Texture Index", tex.texVariantIndex, allocator);
	texObject.AddMember("Rows", tex.rows, allocator);
	texObject.AddMember("Columns", tex.cols, allocator);
	texObject.AddMember("Sprite Number", tex.spritenum, allocator);
	rapidjson::Value filePathValue;
	if (tex.tex) {
		filePathValue.SetString(tex.tex->GetName().c_str(), static_cast<rapidjson::SizeType>(tex.tex->GetName().length()), allocator);
	}
	else {
		filePathValue.SetString("", 0, allocator);
	}
	
	texObject.AddMember("Texture File Path", filePathValue, allocator);
	return texObject;
}


rapidjson::Value SerializeVisible(const Visible& visible, rapidjson::Document::AllocatorType& allocator) {
	rapidjson::Value visibleObject(rapidjson::kObjectType);
	visibleObject.AddMember("isVisible", visible.isVisible, allocator);
	return visibleObject;
}

rapidjson::Value SerializeCircle(const Circle& circle, rapidjson::Document::AllocatorType& allocator) {
	rapidjson::Value circleObject(rapidjson::kObjectType);
	circleObject.AddMember("radius", circle.radius, allocator);
	return circleObject;
}
rapidjson::Value SerializeAABB(const AABB& aabb, rapidjson::Document::AllocatorType& allocator) {
	rapidjson::Value aabbObject(rapidjson::kObjectType);
	aabbObject.AddMember("Min X", aabb.min.x, allocator);
	aabbObject.AddMember("Min Y", aabb.min.y, allocator);
	aabbObject.AddMember("Max X", aabb.max.x, allocator);
	aabbObject.AddMember("Max Y", aabb.max.y, allocator);
	aabbObject.AddMember("Extent X", aabb.extents.x, allocator);
	aabbObject.AddMember("Extent Y", aabb.extents.y, allocator);
	return aabbObject;
}
rapidjson::Value SerializeAnimation(const Animator& anim, rapidjson::Document::AllocatorType& allocator) {
	rapidjson::Value animObject(rapidjson::kObjectType);
	animObject.AddMember("Animation Type", (int)anim.GetAnimationType(), allocator);
	animObject.AddMember("Frame Index", anim.GetFrameIndex(), allocator);
	animObject.AddMember("Frame Time Elapsed", anim.GetFrameTimeElapsed(), allocator);
	animObject.AddMember("Frame Display Duration", anim.GetFrameDisplayDuration() , allocator);
	return animObject;
}


rapidjson::Value SerializeCharacterStats(const CharacterStats& stats, rapidjson::Document::AllocatorType& allocator) {
	rapidjson::Value charstats(rapidjson::kObjectType);
	rapidjson::Value attacks(rapidjson::kArrayType);
	charstats.AddMember("Character type", (int)stats.tag, allocator);
	charstats.AddMember("Max Health", stats.stats.maxHealth, allocator);
	charstats.AddMember("Health", stats.stats.health, allocator);
	charstats.AddMember("Attack", stats.stats.attack, allocator);
	charstats.AddMember("Defense", stats.stats.defense, allocator);
	charstats.AddMember("Speed", stats.stats.speed, allocator);
	rapidjson::Value iconStr;
	iconStr.SetString(stats.icon.c_str(), static_cast<rapidjson::SizeType>(stats.icon.length()), allocator);
	charstats.AddMember("Icon", iconStr, allocator);
	//charstats.AddMember("Bleedstack", stats.debuffs.bleedStack, allocator);
	charstats.AddMember("Boss", stats.boss, allocator);
	charstats.AddMember("Untargetable", stats.untargetable, allocator);

	for (Attack const& a : stats.action.skills) {
		rapidjson::Value attackName;
		attackName.SetString(a.attackName.c_str(), static_cast<rapidjson::SizeType>(a.attackName.length()), allocator);
		attacks.PushBack(attackName, allocator);
	}
	charstats.AddMember("Skills", attacks, allocator);
	
	return charstats;
}

rapidjson::Value SerializeModel(const Model& model, rapidjson::Document::AllocatorType& allocator) {
	rapidjson::Value modelObject(rapidjson::kObjectType);
	modelObject.AddMember("Model type", (int)model.type, allocator);
	modelObject.AddMember("Background scroll speed", model.backgroundScrollSpeed, allocator);
	modelObject.AddMember("Red", model.GetColor().r, allocator);
	modelObject.AddMember("Green", model.GetColor().g, allocator);
	modelObject.AddMember("Blue", model.GetColor().b, allocator);
	modelObject.AddMember("Alpha", model.GetColor().a, allocator);
	modelObject.AddMember("Mirror", model.GetMirror(), allocator);
	return modelObject;
}

rapidjson::Value SerializeCollider(const Collider& collider, rapidjson::Document::AllocatorType& allocator) {
	rapidjson::Value colliderObject(rapidjson::kObjectType);
	colliderObject.AddMember("Collider Enum", (int)collider.bodyShape, allocator);
	colliderObject.AddMember("Dimension X", collider.dimension.x, allocator);
	colliderObject.AddMember("Dimension Y", collider.dimension.y, allocator);
	colliderObject.AddMember("Type", (int)collider.type, allocator);
	colliderObject.AddMember("Event Name", rapidjson::Value(collider.eventName.c_str(), allocator).Move(), allocator);
	colliderObject.AddMember("Event Input", rapidjson::Value(collider.eventInput.c_str(), allocator).Move(), allocator);
	return colliderObject;
}

rapidjson::Value SerializeSliderUI(const SliderUI& slider, rapidjson::Document::AllocatorType& allocator) {
	rapidjson::Value sliderObject(rapidjson::kObjectType);

	sliderObject.AddMember("Linked Entity", slider.linkedEntity, allocator);

	sliderObject.AddMember("Slider Type", (int)slider.type, allocator);
	sliderObject.AddMember("Control Which", (int)slider.controlWhich, allocator);

	return sliderObject;
}

rapidjson::Value SerializeTextLabel(const TextLabel& textLabel, rapidjson::Document::AllocatorType& allocator) {
	rapidjson::Value textObject(rapidjson::kObjectType);
	
	if (textLabel.font != nullptr) {
		auto fontInfo = textLabel.font->GetInfo();
		textObject.AddMember("Font Family", rapidjson::Value(fontInfo.first.c_str(), allocator).Move(), allocator);
		textObject.AddMember("Font Variant", rapidjson::Value(fontInfo.second.c_str(), allocator).Move(), allocator);
	}
	textObject.AddMember("Font Size", textLabel.relFontSize, allocator);
	textObject.AddMember("Text String", rapidjson::Value(textLabel.textString.c_str(), allocator).Move(), allocator);
	textObject.AddMember("r", textLabel.textColor.r, allocator);
	textObject.AddMember("g", textLabel.textColor.g, allocator);
	textObject.AddMember("b", textLabel.textColor.b, allocator);
	textObject.AddMember("a", textLabel.textColor.a, allocator);

	
	textObject.AddMember("Color Preset", rapidjson::Value(textLabel.initClr.c_str(), allocator), allocator);
	
	textObject.AddMember("Horizontal Alignment", (int)textLabel.hAlignment, allocator);
	textObject.AddMember("Vertical Alignment", (int)textLabel.vAlignment, allocator);
	textObject.AddMember("Text Wrap", (int)textLabel.textWrap, allocator);
	textObject.AddMember("Background", (bool)textLabel.hasBackground, allocator);

	return textObject;
}
rapidjson::Value SerializeButton(const Button& button, rapidjson::Document::AllocatorType& allocator) {
	rapidjson::Value buttonObject(rapidjson::kObjectType);

	buttonObject.AddMember("Default Button R", button.defaultColor.buttonColor.r, allocator);
	buttonObject.AddMember("Default Button G", button.defaultColor.buttonColor.g, allocator);
	buttonObject.AddMember("Default Button B", button.defaultColor.buttonColor.b, allocator);
	buttonObject.AddMember("Default Button A", button.defaultColor.buttonColor.a, allocator);
	
	buttonObject.AddMember("Default Text R", button.defaultColor.textColor.r, allocator);
	buttonObject.AddMember("Default Text G", button.defaultColor.textColor.g, allocator);
	buttonObject.AddMember("Default Text B", button.defaultColor.textColor.b, allocator);
	buttonObject.AddMember("Default Text A", button.defaultColor.textColor.a, allocator);

	buttonObject.AddMember("Hovered Button R", button.hoveredColor.buttonColor.r, allocator);
	buttonObject.AddMember("Hovered Button G", button.hoveredColor.buttonColor.g, allocator);
	buttonObject.AddMember("Hovered Button B", button.hoveredColor.buttonColor.b, allocator);
	buttonObject.AddMember("Hovered Button A", button.hoveredColor.buttonColor.a, allocator);

	buttonObject.AddMember("Hovered Text R", button.hoveredColor.textColor.r, allocator);
	buttonObject.AddMember("Hovered Text G", button.hoveredColor.textColor.g, allocator);
	buttonObject.AddMember("Hovered Text B", button.hoveredColor.textColor.b, allocator);
	buttonObject.AddMember("Hovered Text A", button.hoveredColor.textColor.a, allocator);

	buttonObject.AddMember("Focused Button R", button.focusedColor.buttonColor.r, allocator);
	buttonObject.AddMember("Focused Button G", button.focusedColor.buttonColor.g, allocator);
	buttonObject.AddMember("Focused Button B", button.focusedColor.buttonColor.b, allocator);
	buttonObject.AddMember("Focused Button A", button.focusedColor.buttonColor.a, allocator);

	buttonObject.AddMember("Focused Text R", button.focusedColor.textColor.r, allocator);
	buttonObject.AddMember("Focused Text G", button.focusedColor.textColor.g, allocator);
	buttonObject.AddMember("Focused Text B", button.focusedColor.textColor.b, allocator);
	buttonObject.AddMember("Focused Text A", button.focusedColor.textColor.a, allocator);

	buttonObject.AddMember("Event Name", rapidjson::Value(button.eventName.c_str(), allocator).Move(), allocator);
	buttonObject.AddMember("Event Input", rapidjson::Value(button.eventInput.c_str(), allocator).Move(), allocator);
	buttonObject.AddMember("Padding Top", button.padding.top, allocator);
	buttonObject.AddMember("Padding Bottom", button.padding.bottom, allocator);
	buttonObject.AddMember("Padding Left", button.padding.left, allocator);
	buttonObject.AddMember("Padding Right", button.padding.right, allocator);
	buttonObject.AddMember("Padding Setting", button.padding.setting, allocator);

	return buttonObject;
}

rapidjson::Value SerializeHealthBar(const HealthBar& hpBar, rapidjson::Document::AllocatorType& allocator) {
	rapidjson::Value hpBarObject(rapidjson::kObjectType);
	hpBarObject.AddMember("Show Health Stat", (bool)hpBar.showHealthStat, allocator);
	hpBarObject.AddMember("Show Value or Percentage", (bool)hpBar.showValOrPct, allocator);
	hpBarObject.AddMember("Bar Width", hpBar.barWidth, allocator);
	hpBarObject.AddMember("Bar Height", hpBar.barHeight, allocator);

	hpBarObject.AddMember("Current Health", hpBar.currentHealth, allocator);
	hpBarObject.AddMember("Max Health", hpBar.maxHealth, allocator);
	hpBarObject.AddMember("Health Percentage", hpBar.healthPct, allocator);
	return hpBarObject;
}

rapidjson::Value SerializeHealthRemaining(const HealthRemaining& hpRemBar, rapidjson::Document::AllocatorType& allocator) {
	rapidjson::Value hpRemBarObject(rapidjson::kObjectType);
	hpRemBarObject.AddMember("Current Health", hpRemBar.currentHealth, allocator);
	return hpRemBarObject;
}

rapidjson::Value SerializeSkillPointHUD(const SkillPointHUD& spHUD, rapidjson::Document::AllocatorType& allocator) {
	rapidjson::Value spHudObject(rapidjson::kObjectType);
	spHudObject.AddMember("Skill Point Balance", spHUD.skillPointBalance, allocator);
	spHudObject.AddMember("Skill Point Cap", spHUD.maxSkillPoints, allocator);
	return spHudObject;
}

rapidjson::Value SerializeSkillPoint(const SkillPoint& skillpt, rapidjson::Document::AllocatorType& allocator) {
	rapidjson::Value skillPtObject(rapidjson::kObjectType);
	skillPtObject.AddMember("Active State", skillpt.isActive, allocator);
	return skillPtObject;
}

rapidjson::Value SerializeAttackSkill(const AttackSkill& atkSkill, rapidjson::Document::AllocatorType& allocator) {
	rapidjson::Value atkSkillObject(rapidjson::kObjectType);
	atkSkillObject.AddMember("Skill Index", atkSkill.skillIndex, allocator);
	return atkSkillObject;
}

rapidjson::Value SerializeAllyHUD(const AllyHUD& allyHUD, rapidjson::Document::AllocatorType& allocator) {
	rapidjson::Value allyHudObject(rapidjson::kObjectType);
	allyHudObject.AddMember("Ally Index", allyHUD.allyIndex, allocator);
	return allyHudObject;
}

rapidjson::Value SerializeEnemyHUD(const EnemyHUD& enemyHUD, rapidjson::Document::AllocatorType& allocator) {
	rapidjson::Value enemyHudObject(rapidjson::kObjectType);
	enemyHudObject.AddMember("Enemy Index", enemyHUD.enemyIndex, allocator);
	return enemyHudObject;
}

//rapidjson::Value SerializeDialogueSpeaker(const DialogueSpeaker& dialogueSpeaker, rapidjson::Document::AllocatorType& allocator) {
//	rapidjson::Value dialogueSpeakerObject(rapidjson::kObjectType);
//	dialogueSpeakerObject.AddMember("Enemy Index", dialogueSpeaker.enemyIndex, allocator);
//	return dialogueSpeakerObject;
//}

rapidjson::Value SerializeDialogueHUD(const DialogueHUD& dialogueHUD, rapidjson::Document::AllocatorType& allocator) {

	rapidjson::Value dialogueHudObject(rapidjson::kObjectType);
	rapidjson::Value dialoguesVec(rapidjson::kArrayType);

	// Serialize dialoguesVec
	for (const DialogueHUD::Dialogue& dialogue : dialogueHUD.dialogues) {
		rapidjson::Value dialogueObject(rapidjson::kObjectType);
		rapidjson::Value dialogueLinesVec(rapidjson::kArrayType);

		// Serialize dialogueLinesVec
		for (const auto& line : dialogue.dialogueLines) {
			rapidjson::Value lineObject(rapidjson::kObjectType);
			lineObject.AddMember("Speaker", rapidjson::Value(line.speaker.c_str(), allocator).Move(), allocator);
			lineObject.AddMember("Line", rapidjson::Value(line.line.c_str(), allocator).Move(), allocator);
			lineObject.AddMember("Voice", rapidjson::Value(line.voice.c_str(), allocator).Move(), allocator);
			dialogueLinesVec.PushBack(lineObject, allocator);
		}
		dialogueObject.AddMember("Dialogue Lines", dialogueLinesVec, allocator);

		dialogueObject.AddMember("Trigger Type", (int)dialogue.triggerType, allocator);
		dialogueObject.AddMember("Round Trigger", (int)dialogue.roundTrigger, allocator);
		dialogueObject.AddMember("Health Trigger", (int)dialogue.healthTrigger, allocator);

		dialogueObject.AddMember("Viewing Index", dialogue.viewingIndex, allocator);
		dialogueObject.AddMember("Display Duration", dialogue.displayDuration, allocator);
		dialogueObject.AddMember("Is Active", (bool)dialogue.isActive, allocator);

		dialogueObject.AddMember("Is Triggered", (bool)dialogue.isTriggered, allocator);
		//dialogueHudObject.AddMember("Auto Launch", (bool)dialogue.autoLaunch, allocator);
		dialogueObject.AddMember("Speaker Required", (bool)dialogue.speakerRequired, allocator);
		dialogueObject.AddMember("Post Dialogue Scene", (bool)dialogue.postDialogueScene, allocator);
		dialogueObject.AddMember("Target Scene", rapidjson::Value(dialogue.targetScene.c_str(), allocator).Move(), allocator);

		dialoguesVec.PushBack(dialogueObject, allocator);
	}
	dialogueHudObject.AddMember("Dialogues", dialoguesVec, allocator);
	return dialogueHudObject;
}

//rapidjson::Value SerializeTurnIndicator(const TurnIndicator& turnIndicator, rapidjson::Document::AllocatorType& allocator) {
//	rapidjson::Value turnOrderObject(rapidjson::kObjectType);
//
//	return turnOrderObject;
//}

//rapidjson::Value SerializeStatusEffectsPanel(const StatusEffectsPanel& statusFxPanel, rapidjson::Document::AllocatorType& allocator) {
//	rapidjson::Value statusFxPanelObject(rapidjson::kObjectType);
//
//	return statusFxPanelObject;
//}
//
//rapidjson::Value SerializeStatusEffect(const StatusEffect& statusFx, rapidjson::Document::AllocatorType& allocator) {
//	rapidjson::Value statusFxObject(rapidjson::kObjectType);
//
//	return statusFxObject;
//}

rapidjson::Value SerializeAnimationSet(const AnimationSet& animSet, rapidjson::Document::AllocatorType& allocator) {
	rapidjson::Value set(rapidjson::kArrayType);

	for (auto const& a : animSet.animationSet) {
		rapidjson::Value group(rapidjson::kObjectType);
		rapidjson::Value animations(rapidjson::kArrayType);
		rapidjson::Value groupName;
		group.AddMember("Total Frames", a.totalFrames, allocator);
		group.AddMember("Group Name", rapidjson::Value(a.name.c_str(), allocator).Move(), allocator);
		group.AddMember("Loop", a.loop, allocator);
		group.AddMember("Frame Time", a.frametime, allocator);
		for (auto const& anim : a.animations) {
			rapidjson::Value perAnimation(rapidjson::kObjectType);
			rapidjson::Value keyFrames(rapidjson::kArrayType);
			std::string animType{ anim->GetType() };
			perAnimation.AddMember("Animation Type", rapidjson::Value(animType.c_str(), allocator).Move(), allocator);
			if (animType == "Sprite") {
				const std::shared_ptr<SpriteAnimation> sprite{ std::dynamic_pointer_cast<SpriteAnimation>(anim) };
				for (auto const& k : sprite->keyframes) {
					rapidjson::Value keyframe(rapidjson::kObjectType);
					keyframe.AddMember("Frame Number", k.frameNum, allocator);
					keyframe.AddMember("Reverse", k.data, allocator);
					keyFrames.PushBack(keyframe,allocator);
				}
			}
			else if (animType == "TextureChange") {
				const std::shared_ptr<ChangeTexAnimation> texchange{ std::dynamic_pointer_cast<ChangeTexAnimation>(anim) };
				for (auto const& k : texchange->keyframes) {
					rapidjson::Value keyframe(rapidjson::kObjectType);
					keyframe.AddMember("Frame Number", k.frameNum, allocator);
					keyframe.AddMember("Texture", rapidjson::Value(k.data.c_str(), allocator).Move(), allocator);
					keyFrames.PushBack(keyframe, allocator);
				}
			}
			else if (animType == "Sound") {
				const std::shared_ptr<SoundAnimation> sound{ std::dynamic_pointer_cast<SoundAnimation>(anim) };
				for (auto const& k : sound->keyframes) {
					rapidjson::Value keyframe(rapidjson::kObjectType);
					rapidjson::Value sounds(rapidjson::kArrayType);
					keyframe.AddMember("Frame Number", k.frameNum, allocator);
					//keyframe.AddMember("Sound", rapidjson::Value(k.data.c_str(), allocator).Move(), allocator);
					for (std::string const& s : k.data) {
						rapidjson::Value soundName;
						soundName.SetString(s.c_str(), static_cast<rapidjson::SizeType>(s.length()), allocator);
						sounds.PushBack(soundName, allocator);
					}
					keyframe.AddMember("Sounds", sounds, allocator);
					keyFrames.PushBack(keyframe, allocator);
				}
			}
			else if (animType == "Swap") {
				const std::shared_ptr<SwapAnimation> swap{ std::dynamic_pointer_cast<SwapAnimation>(anim) };
				rapidjson::Value keyframe(rapidjson::kObjectType);
				keyframe.AddMember("Frame Number", swap->keyframes.frameNum, allocator);
				keyframe.AddMember("Destination", rapidjson::Value(swap->keyframes.data.c_str(), allocator).Move(), allocator);
				keyFrames.PushBack(keyframe, allocator);
			}
			else if (animType == "TransformAttach") {
				const std::shared_ptr<TransformAttachAnimation> transAttach{ std::dynamic_pointer_cast<TransformAttachAnimation>(anim) };
				for (auto const& k : transAttach->keyframes) {
					rapidjson::Value keyframe(rapidjson::kObjectType);
					keyframe.AddMember("Frame Number", k.frameNum, allocator);
					keyframe.AddMember("Target", rapidjson::Value(k.data.c_str(), allocator).Move(), allocator);
					keyFrames.PushBack(keyframe, allocator);
				}
			}
			else if (animType == "TransformDirect") {
				const std::shared_ptr<TransformDirectAnimation> transDirect{ std::dynamic_pointer_cast<TransformDirectAnimation>(anim) };
				for (auto const& k : transDirect->keyframes) {
					rapidjson::Value keyframe(rapidjson::kObjectType);
					keyframe.AddMember("Frame Number", k.frameNum, allocator);
					keyframe.AddMember("X movement", k.data.position.x, allocator);
					keyframe.AddMember("Y movement", k.data.position.y, allocator);
					keyframe.AddMember("Rotation", k.data.rotation, allocator);
					keyframe.AddMember("Scale", k.data.scale, allocator);
					keyFrames.PushBack(keyframe, allocator);
				}
			}
			else if (animType == "Fade") {
				const std::shared_ptr<FadeAnimation> fade{ std::dynamic_pointer_cast<FadeAnimation>(anim) };
				for (auto const& k : fade->keyframes) {
					rapidjson::Value keyframe(rapidjson::kObjectType);
					keyframe.AddMember("Frame Number", k.frameNum, allocator);
					keyframe.AddMember("Alpha", k.data, allocator);
					keyFrames.PushBack(keyframe, allocator);
				}
			}
			else if (animType == "Color") {
				const std::shared_ptr<ColorAnimation> fade{ std::dynamic_pointer_cast<ColorAnimation>(anim) };
				for (auto const& k : fade->keyframes) {
					rapidjson::Value keyframe(rapidjson::kObjectType);
					keyframe.AddMember("Frame Number", k.frameNum, allocator);
					keyframe.AddMember("Red", k.data.r, allocator);
					keyframe.AddMember("Green", k.data.g, allocator);
					keyframe.AddMember("Blue", k.data.b, allocator);
					keyFrames.PushBack(keyframe, allocator);
				}
			}
			else if (animType == "SelfDestruct") {
				const std::shared_ptr<SelfDestructAnimation> selfd{ std::dynamic_pointer_cast<SelfDestructAnimation>(anim) };
				rapidjson::Value keyframe(rapidjson::kObjectType);
				keyframe.AddMember("Frame Number", selfd->keyframes.frameNum, allocator);
				keyFrames.PushBack(keyframe, allocator);
			}
			else if (animType == "DamageImpact") {
				const std::shared_ptr<DamageImpactAnimation> impact{ std::dynamic_pointer_cast<DamageImpactAnimation>(anim) };
				for (auto const& k : impact->keyframes) {
					rapidjson::Value keyframe(rapidjson::kObjectType);
					keyframe.AddMember("Frame Number", k.frameNum, allocator);
					keyframe.AddMember("Prefab", rapidjson::Value(k.data.c_str(), allocator).Move(), allocator);
					keyFrames.PushBack(keyframe, allocator);
				}
			}
			else if (animType == "CameraZoom") {
				const std::shared_ptr<CameraZoomAnimation> zoom{ std::dynamic_pointer_cast<CameraZoomAnimation>(anim) };
				for (auto const& k : zoom->keyframes) {
					rapidjson::Value keyframe(rapidjson::kObjectType);
					keyframe.AddMember("Frame Number", k.frameNum, allocator);
					keyframe.AddMember("Zoom", k.data, allocator);
					keyFrames.PushBack(keyframe, allocator);
				}
			}
			else if (animType == "CameraTarget") {
				const std::shared_ptr<CameraTargetAnimation> camtarget{ std::dynamic_pointer_cast<CameraTargetAnimation>(anim) };
				for (auto const& k : camtarget->keyframes) {
					rapidjson::Value keyframe(rapidjson::kObjectType);
					keyframe.AddMember("Frame Number", k.frameNum, allocator);
					keyFrames.PushBack(keyframe, allocator);
				}
			}
			else if (animType == "CameraReset") {
				const std::shared_ptr<CameraResetAnimation> camtarget{ std::dynamic_pointer_cast<CameraResetAnimation>(anim) };
				for (auto const& k : camtarget->keyframes) {
					rapidjson::Value keyframe(rapidjson::kObjectType);
					keyframe.AddMember("Frame Number", k.frameNum, allocator);
					keyFrames.PushBack(keyframe, allocator);
				}
			}
			else if (animType == "CreatePrefab") {
				const std::shared_ptr<CreatePrefabAnimation> camtarget{ std::dynamic_pointer_cast<CreatePrefabAnimation>(anim) };
				for (auto const& k : camtarget->keyframes) {
					rapidjson::Value keyframe(rapidjson::kObjectType);
					keyframe.AddMember("Frame Number", k.frameNum, allocator);
					keyframe.AddMember("Prefab", rapidjson::Value(k.data.c_str(), allocator).Move(), allocator);
					keyFrames.PushBack(keyframe, allocator);
				}
			}
			else if (animType == "Event") {
				const std::shared_ptr<EventAnimation> eventAnim{ std::dynamic_pointer_cast<EventAnimation>(anim) };
				for (auto const& k : eventAnim->keyframes) {
					rapidjson::Value keyframe(rapidjson::kObjectType);
					keyframe.AddMember("Frame Number", k.frameNum, allocator);
					keyframe.AddMember("Event Name", rapidjson::Value(k.data.first.c_str(), allocator).Move(), allocator);
					keyframe.AddMember("Event Input", rapidjson::Value(k.data.second.c_str(), allocator).Move(), allocator);
					keyFrames.PushBack(keyframe, allocator);
				}
			}
			else if (animType == "Child") {
				const std::shared_ptr<ChildAnimation> childAnim{ std::dynamic_pointer_cast<ChildAnimation>(anim) };
				for (auto const& k : childAnim->keyframes) {
					rapidjson::Value keyframe(rapidjson::kObjectType);
					keyframe.AddMember("Frame Number", k.frameNum, allocator);
					keyframe.AddMember("Child Name", rapidjson::Value(k.data.first.c_str(), allocator).Move(), allocator);
					keyframe.AddMember("Child Animation", rapidjson::Value(k.data.second.c_str(), allocator).Move(), allocator);
					keyFrames.PushBack(keyframe, allocator);
				}
			}
			else if (animType == "Parent") {
				const std::shared_ptr<ParentAnimation> childAnim{ std::dynamic_pointer_cast<ParentAnimation>(anim) };
				for (auto const& k : childAnim->keyframes) {
					rapidjson::Value keyframe(rapidjson::kObjectType);
					keyframe.AddMember("Frame Number", k.frameNum, allocator);
					keyframe.AddMember("Animation", rapidjson::Value(k.data.c_str(), allocator).Move(), allocator);
					keyFrames.PushBack(keyframe, allocator);
				}
			}
			perAnimation.AddMember("Key Frames", keyFrames, allocator);
			animations.PushBack(perAnimation, allocator);
		}
		group.AddMember("Animations", animations, allocator);
		set.PushBack(group, allocator);
	}

	return set;
}


/*
Helper function to check if class should be serialized
*/
template <typename T>
bool CheckSerialize(const Entity& entity, const bool& isPrefabClone, const std::unordered_set<std::string>* uComponentMap) {
	if (!isPrefabClone || (bool)(uComponentMap->count(typeid(T).name()))) {
		return ECS::ecs().HasComponent<T>(entity);
	}
	else {
		return false;
	}
}


void Serializer::SaveEntityToJson(const std::string& fileName, const std::vector<Entity>& m_entity, bool isPrefab) {
	// Create a JSON document
	rapidjson::Document document;
	document.SetArray();
	rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

	/************************FOR LAYERING***************************/
	PrepareLayeringForSerialization();
	EmbedSkipLockForSerialization();
	rapidjson::Value layeringHeader(rapidjson::kObjectType);

	rapidjson::Value layeringObject(rapidjson::kObjectType);

	layeringObject.AddMember("layerCounter", layerCounter, allocator);
	layeringObject.AddMember("groupCounter", groupCounter, allocator);
	if (!layerNames.empty()) {
		rapidjson::Value layerNamesArray(rapidjson::kArrayType);

		for (unsigned i = 0; i < layerCounter; ++i) {
		//for (const std::pair<std::string, bool>& layerName : layerNames) {
			layerNamesArray.PushBack(rapidjson::Value(layerNames[i].first.c_str(), allocator), allocator);
		}
		layeringObject.AddMember("layerNames", layerNamesArray, allocator);
	}

	layeringHeader.AddMember("LayeringSystems", layeringObject, allocator);

	document.PushBack(layeringHeader, allocator);

	/*******************For ENTITIES****************/
	Color* color = nullptr;
	Transform* transform = nullptr;
	Tex* tex = nullptr;
	Visible* visible = nullptr;
	Size* size = nullptr;
	Circle* circle = nullptr;
	AABB* aabb = nullptr;
	Emitter* emitter = nullptr;
	Name* name = nullptr;
	CharacterStats* charstats = nullptr;
	Model* model = nullptr;
	TextLabel* textLabel = nullptr;
	Button* button = nullptr;
	HealthBar* hpBar = nullptr;
	HealthRemaining* hpRemBar = nullptr;
	SkillPointHUD* spHUD = nullptr;
	SkillPoint* skillpt = nullptr;
	AttackSkill* atkSkill = nullptr;
	AllyHUD* allyHud = nullptr;
	EnemyHUD* enemyHud = nullptr;
	//StatusEffectsPanel* statusFxPanel = nullptr;
	//DialogueSpeaker* dialogueSpeaker = nullptr;
	DialogueHUD* dialogueHud = nullptr;
	Collider* collider = nullptr;
	AnimationSet* animset = nullptr;
	//Temporary* temporary = nullptr;
	SliderUI* sliderUI = nullptr;

	for (const Entity& entity : m_entity) {

		if (ECS::ecs().HasComponent<Temporary>(entity) && !isPrefab) {
			continue;
		}
		
		if (!isPrefab && !ECS::ecs().HasComponent<Clone>(entity)) {
			continue;
		}

		rapidjson::Value entityObject(rapidjson::kObjectType);

		bool isPrefabClone{ false };
		std::unordered_set<std::string>* uComponentMap{};

		if (ECS::ecs().HasComponent<Clone>(entity)) {
			rapidjson::Value cloneObject(rapidjson::kObjectType);
			std::string prefabName{ ECS::ecs().GetComponent<Clone>(entity).prefab };
			if (prefabName != "") {
				rapidjson::Value typeObject(rapidjson::kArrayType);
				cloneObject.AddMember("Prefab", rapidjson::Value(prefabName.c_str(), allocator).Move(), allocator);
				isPrefabClone = true;
				uComponentMap = &ECS::ecs().GetComponent<Clone>(entity).unique_components;
				for (auto& u : *uComponentMap) {
					typeObject.PushBack(rapidjson::Value(u.c_str(), allocator).Move(), allocator);
				}
				cloneObject.AddMember("Unique Components", typeObject, allocator);
			}
			entityObject.AddMember("Clone", cloneObject, allocator);
		}

		if (ECS::ecs().HasComponent<Clone>(entity) && ECS::ecs().HasComponent<Child>(entity)) {
			continue;
		}

		if (CheckSerialize<Name>(entity, isPrefabClone,uComponentMap)) {
			name = &ECS::ecs().GetComponent<Name>(entity);
			rapidjson::Value nameObject = SerializeName(*name, allocator);
			entityObject.AddMember("Entity", nameObject, allocator);
		}
		if (CheckSerialize<Master>(entity, isPrefabClone, uComponentMap)) {
			entityObject.AddMember("Master", rapidjson::Value(rapidjson::kObjectType), allocator);
		}
		if (CheckSerialize<Color>(entity, isPrefabClone, uComponentMap)) {
			color = &ECS::ecs().GetComponent<Color>(entity);
			rapidjson::Value colorObject = SerializeColor(*color, allocator);
			entityObject.AddMember("Color", colorObject, allocator);
			//DECLARE(float, redCol, color->color.r);
			//DECLARE(float, greenCol, color->color.g);
			//DECLARE(float, blueCol, color->color.b);
			//DECLARE(float, alphaCol, color->color.a);
		}
		if (CheckSerialize<Transform>(entity, isPrefabClone, uComponentMap)) {
			transform = &ECS::ecs().GetComponent<Transform>(entity);
			rapidjson::Value transformObject = SerializeTransform(*transform, allocator);
			entityObject.AddMember("Transform", transformObject, allocator);
		}
		if (CheckSerialize<Tex>(entity, isPrefabClone, uComponentMap)) {
			tex = &ECS::ecs().GetComponent<Tex>(entity);
			rapidjson::Value textureObject = SerializeTex(*tex, allocator);
			entityObject.AddMember("Texture", textureObject, allocator);
		}
		if (CheckSerialize<Visible>(entity, isPrefabClone, uComponentMap)) {
			visible = &ECS::ecs().GetComponent<Visible>(entity);
			rapidjson::Value visibleObject = SerializeVisible(*visible, allocator);
			entityObject.AddMember("Visible", visibleObject, allocator);
		}
		if (CheckSerialize<Size>(entity, isPrefabClone, uComponentMap)) {
			size = &ECS::ecs().GetComponent<Size>(entity);
			rapidjson::Value sizeObject = SerializeSize(*size, allocator);
			entityObject.AddMember("Size", sizeObject, allocator);
		}
		if (CheckSerialize<Circle>(entity, isPrefabClone, uComponentMap)) {
			circle = &ECS::ecs().GetComponent<Circle>(entity);
			rapidjson::Value circleObject = SerializeCircle(*circle, allocator);
			entityObject.AddMember("Circle", circleObject, allocator);

		}
		if (CheckSerialize<AABB>(entity, isPrefabClone, uComponentMap)) {
			aabb = &ECS::ecs().GetComponent<AABB>(entity);
			rapidjson::Value aabbObject = SerializeAABB(*aabb, allocator);
			entityObject.AddMember("Collision", aabbObject, allocator);
		}
		if (CheckSerialize<Emitter>(entity, isPrefabClone, uComponentMap)) {
			emitter = &ECS::ecs().GetComponent<Emitter>(entity);
			rapidjson::Value emitterObject = SerializeParticle(*emitter, allocator);
			entityObject.AddMember("Emitter", emitterObject, allocator);
		}
		//if (CheckSerialize<Animator>(entity, isPrefabClone, uComponentMap)) {
		//	anim = &ECS::ecs().GetComponent<Animator>(entity);
		//	rapidjson::Value animationObject = SerializeAnimation(*anim, allocator);
		//	entityObject.AddMember("Animation", animationObject, allocator);
		//}
		
		if (CheckSerialize<CharacterStats>(entity, isPrefabClone, uComponentMap)) {
			charstats = &ECS::ecs().GetComponent<CharacterStats>(entity);
			rapidjson::Value charstatsObject = SerializeCharacterStats(*charstats, allocator);
			entityObject.AddMember("CharacterStats", charstatsObject, allocator);
		}
		if (CheckSerialize<Model>(entity, isPrefabClone, uComponentMap)) {
			model = &ECS::ecs().GetComponent<Model>(entity);
			rapidjson::Value modelObject = SerializeModel(*model, allocator);
			entityObject.AddMember("Model", modelObject, allocator);
		}
		if (CheckSerialize<Movable>(entity, isPrefabClone, uComponentMap)) {
			entityObject.AddMember("Movable", rapidjson::Value(rapidjson::kObjectType), allocator);
		}
		if (CheckSerialize<MainCharacter>(entity, isPrefabClone, uComponentMap)) {
			entityObject.AddMember("MainCharacter", rapidjson::Value(rapidjson::kObjectType), allocator);
		}
		if (CheckSerialize<TextLabel>(entity, isPrefabClone, uComponentMap)) {
			textLabel = &ECS::ecs().GetComponent<TextLabel>(entity);
			rapidjson::Value textObject = SerializeTextLabel(*textLabel, allocator);
			entityObject.AddMember("Text Label", textObject, allocator);
		}
		if (CheckSerialize<Button>(entity, isPrefabClone, uComponentMap)) {
			button = &ECS::ecs().GetComponent<Button>(entity);
			rapidjson::Value buttonObject = SerializeButton(*button, allocator);
			entityObject.AddMember("Button", buttonObject, allocator);
		}
		if (CheckSerialize<HealthBar>(entity, isPrefabClone, uComponentMap)) {
			hpBar = &ECS::ecs().GetComponent<HealthBar>(entity);
			rapidjson::Value hpBarObject = SerializeHealthBar(*hpBar, allocator);
			entityObject.AddMember("HealthBar", hpBarObject, allocator);
		}
		if (CheckSerialize<HealthRemaining>(entity, isPrefabClone, uComponentMap)) {
			hpRemBar = &ECS::ecs().GetComponent<HealthRemaining>(entity);
			rapidjson::Value hpRemBarObject = SerializeHealthRemaining(*hpRemBar, allocator);
			entityObject.AddMember("HealthRemaining", hpRemBarObject, allocator);
		}
		if (CheckSerialize<HealthLerp>(entity, isPrefabClone, uComponentMap)) {
			entityObject.AddMember("HealthLerp", rapidjson::Value(rapidjson::kObjectType), allocator);
		}
		if (CheckSerialize<SkillPointHUD>(entity, isPrefabClone, uComponentMap)) {
			spHUD = &ECS::ecs().GetComponent<SkillPointHUD>(entity);
			rapidjson::Value spHudObject = SerializeSkillPointHUD(*spHUD, allocator);
			entityObject.AddMember("SkillPointHUD", spHudObject, allocator);
		}
		if (CheckSerialize<SkillPoint>(entity, isPrefabClone, uComponentMap)) {
			skillpt = &ECS::ecs().GetComponent<SkillPoint>(entity);
			rapidjson::Value skillPtObject = SerializeSkillPoint(*skillpt, allocator);
			entityObject.AddMember("SkillPoint", skillPtObject, allocator);
		}
		if (CheckSerialize<AttackSkill>(entity, isPrefabClone, uComponentMap)) {
			atkSkill = &ECS::ecs().GetComponent<AttackSkill>(entity);
			rapidjson::Value atkSkillObject = SerializeAttackSkill(*atkSkill, allocator);
			entityObject.AddMember("AttackSkill", atkSkillObject, allocator);
		}
		if (CheckSerialize<SkillIcon>(entity, isPrefabClone, uComponentMap)) {
			entityObject.AddMember("SkillIcon", rapidjson::Value(rapidjson::kObjectType), allocator);
		}
		if (CheckSerialize<SkillCost>(entity, isPrefabClone, uComponentMap)) {
			entityObject.AddMember("SkillCost", rapidjson::Value(rapidjson::kObjectType), allocator);
		}
		if (CheckSerialize<SkillAttackType>(entity, isPrefabClone, uComponentMap)) {
			entityObject.AddMember("SkillAttackType", rapidjson::Value(rapidjson::kObjectType), allocator);
		}
		if (CheckSerialize<AllyHUD>(entity, isPrefabClone, uComponentMap)) {
			allyHud = &ECS::ecs().GetComponent<AllyHUD>(entity);
			rapidjson::Value allyHudObject = SerializeAllyHUD(*allyHud, allocator);
			entityObject.AddMember("AllyHUD", allyHudObject, allocator);
		}
		if (CheckSerialize<EnemyHUD>(entity, isPrefabClone, uComponentMap)) {
			enemyHud = &ECS::ecs().GetComponent<EnemyHUD>(entity);
			rapidjson::Value enemyHudObject = SerializeEnemyHUD(*enemyHud, allocator);
			entityObject.AddMember("EnemyHUD", enemyHudObject, allocator);
		}
		if (CheckSerialize<TurnIndicator>(entity, isPrefabClone, uComponentMap)) {
			entityObject.AddMember("TurnIndicator", rapidjson::Value(rapidjson::kObjectType), allocator);
		}
		//if (CheckSerialize<StatusEffectsPanel>(entity, isPrefabClone, uComponentMap)) {
		//	//statusFxPanel = &ECS::ecs().GetComponent<StatusEffectsPanel>(entity);
		//	//rapidjson::Value statusFxPanelObject = SerializeStatusEffectsPanel(*statusFxPanel, allocator);
		//	entityObject.AddMember("StatusEffectsPanel", rapidjson::Value(rapidjson::kObjectType), allocator);
		//}
		if (CheckSerialize<StatusEffect>(entity, isPrefabClone, uComponentMap)) {
			entityObject.AddMember("StatusEffect", rapidjson::Value(rapidjson::kObjectType), allocator);
		}
		if (CheckSerialize<DialogueSpeaker>(entity, isPrefabClone, uComponentMap)) {
			entityObject.AddMember("DialogueSpeaker", rapidjson::Value(rapidjson::kObjectType), allocator);
		}
		if (CheckSerialize<DialogueHUD>(entity, isPrefabClone, uComponentMap)) {
			dialogueHud = &ECS::ecs().GetComponent<DialogueHUD>(entity);
			rapidjson::Value dialogueHudObject = SerializeDialogueHUD(*dialogueHud, allocator);
			entityObject.AddMember("DialogueHUD", dialogueHudObject, allocator);
		}
		if (CheckSerialize<Collider>(entity, isPrefabClone, uComponentMap)) {
			collider = &ECS::ecs().GetComponent<Collider>(entity);
			rapidjson::Value colliderObject = SerializeCollider(*collider, allocator);
			entityObject.AddMember("Collider", colliderObject, allocator);
		}
		if (CheckSerialize<AnimationSet>(entity, isPrefabClone, uComponentMap)) {
			animset = &ECS::ecs().GetComponent<AnimationSet>(entity);
			rapidjson::Value animsetObject = SerializeAnimationSet(*animset, allocator);
			entityObject.AddMember("Animation Set", animsetObject, allocator);
			entityObject.AddMember("Animation Set Default Animation", rapidjson::Value(animset->defaultAnimation.c_str(), allocator).Move(), allocator);
		}
		if (CheckSerialize<Parent>(entity, isPrefabClone, uComponentMap)) {
			entityObject.AddMember("Parent", rapidjson::Value(rapidjson::kObjectType), allocator);
		}
		if (CheckSerialize<Child>(entity, isPrefabClone, uComponentMap)) {
			transform = &ECS::ecs().GetComponent<Child>(entity).offset;
			rapidjson::Value transformObject = SerializeTransform(*transform, allocator);
			entityObject.AddMember("Child", transformObject, allocator);
		}
		if (CheckSerialize<Emitter>(entity, isPrefabClone, uComponentMap)) {
			entityObject.AddMember("Emitter", rapidjson::Value(rapidjson::kObjectType), allocator);
		}
		if (CheckSerialize<Temporary>(entity, isPrefabClone, uComponentMap)) {
			entityObject.AddMember("Temporary", rapidjson::Value(rapidjson::kObjectType), allocator);
		}
		if (CheckSerialize<SliderUI>(entity, isPrefabClone, uComponentMap)) {
			printf("Inside CheckSerialize<SliderUI>\n");
			sliderUI = &ECS::ecs().GetComponent<SliderUI>(entity);
			rapidjson::Value sliderObject = SerializeSliderUI(*sliderUI, allocator);
			entityObject.AddMember("SliderUI", sliderObject, allocator);
		}
		document.PushBack(entityObject, allocator);
		//document.PushBack(entityArray, allocator);
	}
	selectedLayer = std::numeric_limits<size_t>().max();
	
	// Save the JSON document to a file
	std::ofstream ofs(fileName);
	if (ofs.is_open()) {
		rapidjson::StringBuffer buffer;
		rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		ofs << buffer.GetString();
		ofs.close();
		std::cout << "Entity saved to " << fileName << std::endl;
	}
	else {
		std::cerr << "Failed to open file: " << fileName << std::endl;
	}

	// To save the state to a file for reflection
	//SerializeVariablesToFile("variables.sav", variablesTEST);
}

void LoadLayeringData(const rapidjson::Value& layeringObject) {
	
	if (layeringObject.HasMember("layerCounter")) {
		layerCounter = layeringObject["layerCounter"].GetUint();
	}
	if (layeringObject.HasMember("groupCounter")) {
		groupCounter = layeringObject["groupCounter"].GetUint();
	}

	if (layeringObject.HasMember("layerNames") && layeringObject["layerNames"].IsArray()) {
		const rapidjson::Value& layerNamesArray = layeringObject["layerNames"];
		if (layerNames.size() < layerCounter) {
			layerNames.clear();
			for (rapidjson::SizeType i = 0; i < layerNamesArray.Size(); ++i) {
				if (layerNamesArray[i].IsString()) {
					layerNames.push_back(std::make_pair(layerNamesArray[i].GetString(), true)); //
				}
			}
		}
	}
}

Entity Serializer::LoadEntityFromJson(const std::string& fileName, bool isPrefab) {
	rapidjson::Document document;
	std::ifstream file(fileName);
	Entity entity{0};
	Parent* parent{};
	Entity parentID{};
	if (!file.is_open()) {
		std::cerr << "Failed to open file: " << fileName << std::endl;
		return entity;
	}
	rapidjson::IStreamWrapper isw(file);
	document.ParseStream(isw);
	if (document.HasParseError()) {
		std::cerr << "Failed to parse .json file: " << fileName << std::endl;
	}
	//layering.clear();

	for (rapidjson::SizeType i = 0; i < document.Size(); ++i) {
		const rapidjson::Value& entityObject = document[i];
		
		if (document.Size() > 0 && document[i].HasMember("LayeringSystems")) {
			const rapidjson::Value& layeringObject = document[0]["LayeringSystems"];
			LoadLayeringData(layeringObject);
		}
		else {
			entity = 0;
			if (entityObject.HasMember("Clone")) {
				const rapidjson::Value& cloneObject = entityObject["Clone"];
				if (cloneObject.HasMember("Prefab")) {
					std::string prefabName = cloneObject["Prefab"].GetString();
					Entity prefabID{ assetmanager.GetPrefab(prefabName) };
					entity = EntityFactory::entityFactory().CloneMaster(prefabID);
					const rapidjson::Value& componentSet{ cloneObject["Unique Components"] };
					Clone& cloneComponent{ ECS::ecs().GetComponent<Clone>(entity) };
					cloneComponent.prefab = prefabName;
					for (rapidjson::SizeType s = 0; s < componentSet.Size(); s++) {
						std::string componentName{ componentSet[s].GetString() };
						cloneComponent.unique_components.insert(componentName);
					}
				}
				else {
					entity = ECS::ecs().CreateEntity();
					(EntityFactory::entityFactory().cloneCounter)++;
					ECS::ecs().AddComponent(entity, Clone{});
				}
				if (!stopButton) {
					selectedLayer = std::numeric_limits<size_t>().max();
				}
			}

			if (entity == 0) {
				entity = ECS::ecs().CreateEntity();
				(EntityFactory::entityFactory().cloneCounter)++;
			}

			if (entityObject.HasMember("Entity")) {
				const rapidjson::Value& nameObject = entityObject["Entity"];
				Name name{};
				name.name = nameObject["Name"].GetString();
				if (nameObject.HasMember("Current Layer")) {
					name.serializationLayer = nameObject["Current Layer"].GetUint64();
				}
				if (nameObject.HasMember("Order in Layer")) {
					name.serializationOrderInLayer = nameObject["Order in Layer"].GetUint64();
				}
				if (nameObject.HasMember("isLocked")) {
					name.lock = nameObject["isLocked"].GetBool();
				}
				if (nameObject.HasMember("isSkipped")) {
					name.skip = nameObject["isSkipped"].GetBool();
				}

				if (ECS::ecs().HasComponent<Name>(entity)) {
					ECS::ecs().GetComponent<Name>(entity) = name;
				}
				else {
					ECS::ecs().AddComponent<Name>(entity, name);
				}
			}

			if (entityObject.HasMember("Color")) {
				const rapidjson::Value& colorObject = entityObject["Color"];
				Color color{};
				color.color.r = colorObject["r"].GetFloat();
				color.color.g = colorObject["g"].GetFloat();
				color.color.b = colorObject["b"].GetFloat();
				color.color.a = colorObject["a"].GetFloat();

				if (ECS::ecs().HasComponent<Color>(entity)) {
					ECS::ecs().GetComponent<Color>(entity) = color;
				}
				else {
					ECS::ecs().AddComponent<Color>(entity, color);
				}
			}

			if (entityObject.HasMember("Transform")) {
				const rapidjson::Value& transformObject = entityObject["Transform"];
				Transform transform;
				transform.position.x = transformObject["position_x"].GetFloat();
				transform.position.y = transformObject["position_y"].GetFloat();
				transform.rotation = transformObject["rotation"].GetFloat();
				transform.scale = transformObject["scale"].GetFloat();
				transform.velocity.x = transformObject["velocity_x"].GetFloat();
				transform.velocity.y = transformObject["velocity_y"].GetFloat();

				if (ECS::ecs().HasComponent<Transform>(entity)) {
					ECS::ecs().GetComponent<Transform>(entity) = transform;
				}
				else {
					ECS::ecs().AddComponent<Transform>(entity, transform);
				}
			}

			if (entityObject.HasMember("Texture")) {
				const rapidjson::Value& texObject = entityObject["Texture"];
				Tex tex;
				tex.texVariantIndex = texObject["Texture Index"].GetUint();
				tex.rows = texObject["Rows"].GetUint();
				tex.cols = texObject["Columns"].GetUint();
				tex.spritenum = texObject["Sprite Number"].GetUint();

				// Get the file path from JSON
				const char* filePath = texObject["Texture File Path"].GetString();

				// Attempt to add or retrieve the Texture from the TextureManager
				Texture* texture = assetmanager.texture.Get(filePath);

				if (texture) {
					tex.tex = texture;
				}

				if (ECS::ecs().HasComponent<Tex>(entity)) {
					ECS::ecs().GetComponent<Tex>(entity) = tex;
				}
				else {
					ECS::ecs().AddComponent<Tex>(entity, tex);
				}
			}

			if (entityObject.HasMember("Visible")) {
				const rapidjson::Value& visibleObject = entityObject["Visible"];
				Visible visible{};
				visible.isVisible = visibleObject["isVisible"].GetBool();

				if (ECS::ecs().HasComponent<Visible>(entity)) {
					ECS::ecs().GetComponent<Visible>(entity) = visible;
				}
				else {
					ECS::ecs().AddComponent<Visible>(entity, visible);
				}
			}

			if (entityObject.HasMember("Size")) {
				const rapidjson::Value& sizeObject = entityObject["Size"];
				Size size{};
				size.width = sizeObject["width"].GetFloat();
				size.height = sizeObject["height"].GetFloat();

				if (ECS::ecs().HasComponent<Size>(entity)) {
					ECS::ecs().GetComponent<Size>(entity) = size;
				}
				else {
					ECS::ecs().AddComponent<Size>(entity, size);
				}
			}

			if (entityObject.HasMember("Circle")) {
				const rapidjson::Value& circleObject = entityObject["Circle"];
				Circle circle{};
				circle.radius = circleObject["radius"].GetFloat();

				if (ECS::ecs().HasComponent<Circle>(entity)) {
					ECS::ecs().GetComponent<Circle>(entity) = circle;
				}
				else {
					ECS::ecs().AddComponent<Circle>(entity, circle);
				}
			}

			if (entityObject.HasMember("Collision")) {
				const rapidjson::Value& aabbObject = entityObject["Collision"];
				AABB aabb;
				aabb.min.x = aabbObject["Min X"].GetFloat();
				aabb.min.y = aabbObject["Min Y"].GetFloat();
				aabb.max.x = aabbObject["Max X"].GetFloat();
				aabb.max.y = aabbObject["Max Y"].GetFloat();
				aabb.extents.x = aabbObject["Extent X"].GetFloat();
				aabb.extents.y = aabbObject["Extent Y"].GetFloat();

				if (ECS::ecs().HasComponent<AABB>(entity)) {
					ECS::ecs().GetComponent<AABB>(entity) = aabb;
				}
				else {
					ECS::ecs().AddComponent<AABB>(entity, aabb);
				}
			}

			if (entityObject.HasMember("Emitter")) {
				const rapidjson::Value& emitterObject = entityObject["Emitter"];
				Emitter emitter;
				emitter.position.x= (emitterObject.HasMember("Emitter PositionX")) ? emitterObject["Emitter PositionX"].GetFloat() : 0.f;
				emitter.position.y = (emitterObject.HasMember("Emitter PositionY")) ? emitterObject["Emitter PositionY"].GetFloat() : 0.f;
				emitter.emitterLifetime = (emitterObject.HasMember("Emitter Lifetime")) ? emitterObject["Emitter Lifetime"].GetFloat() : 2.f;
				emitter.size.x = (emitterObject.HasMember("Particles SizeX")) ? emitterObject["Particles SizeX"].GetFloat() : 5.f;
				emitter.size.y = (emitterObject.HasMember("Particles SizeY")) ? emitterObject["Particles SizeY"].GetFloat() : 5.f;
				emitter.particleColor.color.r = (emitterObject.HasMember("Particle ColorR")) ? emitterObject["Particle ColorR"].GetFloat() : 1.f;
				emitter.particleColor.color.g = (emitterObject.HasMember("Particle ColorG")) ? emitterObject["Particle ColorG"].GetFloat() : 1.f;
				emitter.particleColor.color.b = (emitterObject.HasMember("Particle ColorB")) ? emitterObject["Particle ColorB"].GetFloat() : 1.f;
				emitter.particleColor.color.a = (emitterObject.HasMember("Particle ColorA")) ? emitterObject["Particle ColorA"].GetFloat() : 1.f;
				emitter.particlesRate = (emitterObject.HasMember("Particle Rate")) ? emitterObject["Particle Rate"].GetInt() : 1;
				emitter.frequency = (emitterObject.HasMember("Frequency")) ? emitterObject["Frequency"].GetFloat() : 1.f;
				emitter.particleLifetime = (emitterObject.HasMember("Particle Lifetime")) ? emitterObject["Particle Lifetime"].GetFloat() : 1.f;
				emitter.rotation = (emitterObject.HasMember("Rotation")) ? emitterObject["Rotation"].GetFloat() : 0.f;
				emitter.rotationSpeed = (emitterObject.HasMember("Rotation Speed")) ? emitterObject["Rotation Speed"].GetFloat() : 0.f;
				emitter.velocity.x = (emitterObject.HasMember("VelocityX")) ? emitterObject["VelocityX"].GetFloat() : 0.f;
				emitter.velocity.y = (emitterObject.HasMember("VelocityY")) ? emitterObject["VelocityY"].GetFloat() : 0.f;
				emitter.singleSided = (emitterObject.HasMember("Single Sided")) ? emitterObject["Single Sided"].GetBool() : false;

				if (emitterObject.HasMember("Textures")) {
					for (auto& a : emitterObject["Textures"].GetArray()) {
						emitter.textures.push_back(a.GetString());
					}
				}
				

				if (ECS::ecs().HasComponent<Emitter>(entity)) {
					ECS::ecs().GetComponent<Emitter>(entity) = emitter;
				}
				else {
					ECS::ecs().AddComponent<Emitter>(entity, emitter);
				}
			}

			if (entityObject.HasMember("Master")) {
				if (!ECS::ecs().HasComponent<Master>(entity)) {
					ECS::ecs().AddComponent(entity, Master{});
					EntityFactory::entityFactory().masterEntitiesList[ECS::ecs().GetComponent<Name>(entity).name] = entity;
					++(EntityFactory::entityFactory().masterCounter);
				}
			}
			if (entityObject.HasMember("MainCharacter")) {
				if (!ECS::ecs().HasComponent<MainCharacter>(entity)) {
					ECS::ecs().AddComponent(entity, MainCharacter{});
				}
			}
			if (entityObject.HasMember("Model")) {
				const rapidjson::Value& modelObject = entityObject["Model"];
				Model model{ modelObject["Model type"].GetInt(),modelObject["Background scroll speed"].GetFloat() };
				if (modelObject.HasMember("Red")) {
					glm::vec4 modelColor{};
					modelColor.r = modelObject["Red"].GetFloat();
					modelColor.g = modelObject["Green"].GetFloat();
					modelColor.b = modelObject["Blue"].GetFloat();
					modelColor.a = modelObject["Alpha"].GetFloat();
					model.SetColor(modelColor.r, modelColor.g, modelColor.b);
					model.SetAlpha(modelColor.a);
				}

				if (modelObject.HasMember("Mirror")) {
					bool mirror{};
					mirror = modelObject["Mirror"].GetBool();
					model.SetMirror(mirror);
				}

				if (ECS::ecs().HasComponent<Model>(entity)) {
					ECS::ecs().GetComponent<Model>(entity) = model;
				}
				else {
					ECS::ecs().AddComponent<Model>(entity, model);
				}
			}
			if (entityObject.HasMember("Collider")) {
				const rapidjson::Value& colliderObject = entityObject["Collider"];
				Collider collider;
				int enumID = colliderObject["Collider Enum"].GetInt();
				if (colliderObject.HasMember("Dimension X")) {
					collider.dimension.x = colliderObject["Dimension X"].GetFloat();
					collider.dimension.y = colliderObject["Dimension Y"].GetFloat();
				}
				collider.bodyShape = static_cast<Collider::SHAPE_ID>(enumID);
				if (colliderObject.HasMember("Type")) {
					int typeID = colliderObject["Type"].GetInt();
					collider.type = static_cast<Collider::COLLISION_TYPE>(typeID);
					collider.eventName = colliderObject["Event Name"].GetString();
					collider.eventInput = colliderObject["Event Input"].GetString();
				}
				if (ECS::ecs().HasComponent<Collider>(entity)) {
					ECS::ecs().GetComponent<Collider>(entity) = collider;
				}
				else {
					ECS::ecs().AddComponent<Collider>(entity, collider);
				}
			}
			if (entityObject.HasMember("Movable")) {
				if (!ECS::ecs().HasComponent<Movable>(entity)) {
					ECS::ecs().AddComponent(entity, Movable{});
				}
			}

			if (entityObject.HasMember("Temporary")) {
				ECS::ecs().AddComponent<Temporary>(entity, Temporary{});
			}
			if (entityObject.HasMember("CharacterStats")) {
				const rapidjson::Value& statsObject = entityObject["CharacterStats"];
				CharacterStats charstats;
				charstats.stats.attack = statsObject["Attack"].GetFloat();
				charstats.stats.defense = statsObject["Defense"].GetFloat();
				charstats.stats.maxHealth = statsObject["Max Health"].GetFloat();
				charstats.stats.health = charstats.stats.maxHealth;
				charstats.stats.speed = statsObject["Speed"].GetInt();
				charstats.tag = (CharacterType)statsObject["Character type"].GetInt();
				if (statsObject.HasMember("Icon")) {
					charstats.icon = statsObject["Icon"].GetString();
				}
				if (statsObject.HasMember("Boss")) {
					charstats.boss = statsObject["Boss"].GetBool();
				}
				for (auto& a : statsObject["Skills"].GetArray()) {
					charstats.action.skills.push_back(assetmanager.attacks.data[a.GetString()]);
				}

				if (statsObject.HasMember("Untargetable")) {
					charstats.untargetable = statsObject["Untargetable"].GetBool();
				}

				if (ECS::ecs().HasComponent<CharacterStats>(entity)) {
					ECS::ecs().GetComponent<CharacterStats>(entity) = charstats;
				}
				else {
					ECS::ecs().AddComponent<CharacterStats>(entity, charstats);
				}
			}
			if (entityObject.HasMember("Text Label")) {
				const rapidjson::Value& textObject = entityObject["Text Label"];
				TextLabel textLabel;
				std::string fontFamily = textObject["Font Family"].GetString();
				std::string fontvariant = textObject["Font Variant"].GetString();
				textLabel.font = fonts.GetFont(fontFamily, fontvariant);

				if (textObject.HasMember("Font Size")) {
					textLabel.relFontSize = textObject["Font Size"].GetFloat();
				}

				textLabel.textString = textObject["Text String"].GetString();

				textLabel.textColor.r = textObject["r"].GetFloat();
				textLabel.textColor.g = textObject["g"].GetFloat();
				textLabel.textColor.b = textObject["b"].GetFloat();
				textLabel.textColor.a = textObject["a"].GetFloat();

				textLabel.initClr = textObject["Color Preset"].GetString();

				if (textObject.HasMember("Horizontal Alignment") && textObject.HasMember("Vertical Alignment")) {
					textLabel.hAlignment = (UI_HORIZONTAL_ALIGNMENT)(textObject["Horizontal Alignment"].GetInt());
					textLabel.vAlignment = (UI_VERTICAL_ALIGNMENT)(textObject["Vertical Alignment"].GetInt());
				}

				if (textObject.HasMember("Text Wrap")) {
					textLabel.textWrap = (UI_TEXT_WRAP)(textObject["Text Wrap"].GetInt());
				}

				if (textObject.HasMember("Background")) {
					textLabel.hasBackground = textObject["Background"].GetBool();
				}				

				if (ECS::ecs().HasComponent<TextLabel>(entity)) {
					ECS::ecs().GetComponent<TextLabel>(entity) = textLabel;
				}
				else {
					ECS::ecs().AddComponent<TextLabel>(entity, textLabel);
				}
			}
			if (entityObject.HasMember("Button")) {
				const rapidjson::Value& buttonObject = entityObject["Button"];
				Button button;
				glm::vec4 buttonColor{};
				glm::vec4 textColor{};
				// init with default ColorSet
				if (buttonObject.HasMember("Default Button R") && buttonObject.HasMember("Default Button G") &&
					buttonObject.HasMember("Default Button B") && buttonObject.HasMember("Default Button A")) {
					buttonColor.r = buttonObject["Default Button R"].GetFloat();
					buttonColor.g = buttonObject["Default Button G"].GetFloat();
					buttonColor.b = buttonObject["Default Button B"].GetFloat();
					buttonColor.a = buttonObject["Default Button A"].GetFloat();
				}
				if (buttonObject.HasMember("Default Text R") && buttonObject.HasMember("Default Text G") &&
					buttonObject.HasMember("Default Text B") && buttonObject.HasMember("Default Text A")) {
					textColor.r = buttonObject["Default Text R"].GetFloat();
					textColor.g = buttonObject["Default Text G"].GetFloat();
					textColor.b = buttonObject["Default Text B"].GetFloat();
					textColor.a = buttonObject["Default Text A"].GetFloat();
				}
				button = { buttonColor, textColor };

				// load hovered and focused ColorSets
				if (buttonObject.HasMember("Hovered Button R") && buttonObject.HasMember("Hovered Button G") &&
					buttonObject.HasMember("Hovered Button B") && buttonObject.HasMember("Hovered Button A")) {
					buttonColor.r = buttonObject["Hovered Button R"].GetFloat();
					buttonColor.g = buttonObject["Hovered Button G"].GetFloat();
					buttonColor.b = buttonObject["Hovered Button B"].GetFloat();
					buttonColor.a = buttonObject["Hovered Button A"].GetFloat();
				}
				if (buttonObject.HasMember("Hovered Text R") && buttonObject.HasMember("Hovered Text G") &&
					buttonObject.HasMember("Hovered Text B") && buttonObject.HasMember("Hovered Text A")) {
					textColor.r = buttonObject["Hovered Text R"].GetFloat();
					textColor.g = buttonObject["Hovered Text G"].GetFloat();
					textColor.b = buttonObject["Hovered Text B"].GetFloat();
					textColor.a = buttonObject["Hovered Text A"].GetFloat();
				}
				button.hoveredColor = { buttonColor, textColor };

				if (buttonObject.HasMember("Focused Button R") && buttonObject.HasMember("Focused Button G") &&
					buttonObject.HasMember("Focused Button B") && buttonObject.HasMember("Focused Button A")) {
					buttonColor.r = buttonObject["Focused Button R"].GetFloat();
					buttonColor.g = buttonObject["Focused Button G"].GetFloat();
					buttonColor.b = buttonObject["Focused Button B"].GetFloat();
					buttonColor.a = buttonObject["Focused Button A"].GetFloat();
				}
				if (buttonObject.HasMember("Focused Text R") && buttonObject.HasMember("Focused Text G") &&
					buttonObject.HasMember("Focused Text B") && buttonObject.HasMember("Focused Text A")) {
					textColor.r = buttonObject["Focused Text R"].GetFloat();
					textColor.g = buttonObject["Focused Text G"].GetFloat();
					textColor.b = buttonObject["Focused Text B"].GetFloat();
					textColor.a = buttonObject["Focused Text A"].GetFloat();
				}
				button.focusedColor = { buttonColor, textColor };


				if (buttonObject.HasMember("Padding Top") && buttonObject.HasMember("Padding Bottom") &&
					buttonObject.HasMember("Padding Left") && buttonObject.HasMember("Padding Right")) {
					button.padding.top = buttonObject["Padding Top"].GetFloat();
					button.padding.bottom = buttonObject["Padding Bottom"].GetFloat();
					button.padding.left = buttonObject["Padding Left"].GetFloat();
					button.padding.right = buttonObject["Padding Right"].GetFloat();
					if (buttonObject.HasMember("Padding Setting")) {
						button.padding.setting = buttonObject["Padding Setting"].GetInt();
					}					
				}

				if (buttonObject.HasMember("Event Name")) {
					button.eventName = buttonObject["Event Name"].GetString();
				}

				if (buttonObject.HasMember("Event Input")) {
					button.eventInput = buttonObject["Event Input"].GetString();
				}

				if (ECS::ecs().HasComponent<Button>(entity)) {
					ECS::ecs().GetComponent<Button>(entity) = button;
				}
				else {
					ECS::ecs().AddComponent<Button>(entity, button);
				}
			}
			if (entityObject.HasMember("HealthBar")) {
				HealthBar hpBar;
				const rapidjson::Value& hpBarObject = entityObject["HealthBar"];

				if (hpBarObject.HasMember("Show Health Stat") && hpBarObject.HasMember("Show Value or Percentage")) {
					hpBar.showHealthStat = hpBarObject["Show Health Stat"].GetBool();
					hpBar.showValOrPct = hpBarObject["Show Value or Percentage"].GetBool();
				}
				
				hpBar.barWidth = hpBarObject["Bar Width"].GetFloat();
				hpBar.barHeight = hpBarObject["Bar Height"].GetFloat();

				hpBar.currentHealth = hpBarObject["Current Health"].GetFloat();
				hpBar.maxHealth = hpBarObject["Max Health"].GetFloat();
				hpBar.healthPct = hpBarObject["Health Percentage"].GetFloat();

				if (ECS::ecs().HasComponent<HealthBar>(entity)) {
					ECS::ecs().GetComponent<HealthBar>(entity) = hpBar;
				}
				else {
					ECS::ecs().AddComponent<HealthBar>(entity, hpBar);
				}
			}
			if (entityObject.HasMember("HealthRemaining")) {
				HealthRemaining hpRemBar;
				const rapidjson::Value& hpBarObject = entityObject["HealthRemaining"];
				hpRemBar.currentHealth = hpBarObject["Current Health"].GetFloat();

				if (ECS::ecs().HasComponent<HealthRemaining>(entity)) {
					ECS::ecs().GetComponent<HealthRemaining>(entity) = hpRemBar;
				}
				else {
					ECS::ecs().AddComponent<HealthRemaining>(entity, hpRemBar);
				}
			}
			if (entityObject.HasMember("HealthLerp")) {
				ECS::ecs().AddComponent<HealthLerp>(entity, HealthLerp{});
			}
			if (entityObject.HasMember("SkillPointHUD")) {
				SkillPointHUD spHud;
				const rapidjson::Value& spHudObject = entityObject["SkillPointHUD"];
				spHud.skillPointBalance = spHudObject["Skill Point Balance"].GetInt();
				spHud.maxSkillPoints = spHudObject["Skill Point Cap"].GetInt();

				if (ECS::ecs().HasComponent<SkillPointHUD>(entity)) {
					ECS::ecs().GetComponent<SkillPointHUD>(entity) = spHud;
				}
				else {
					ECS::ecs().AddComponent<SkillPointHUD>(entity, spHud);
				}
			}
			if (entityObject.HasMember("SkillPoint")) {
				SkillPoint skillpt;
				const rapidjson::Value& spObject = entityObject["SkillPoint"];
				skillpt.isActive = spObject["Active State"].GetBool();

				if (ECS::ecs().HasComponent<SkillPoint>(entity)) {
					ECS::ecs().GetComponent<SkillPoint>(entity) = skillpt;
				}
				else {
					ECS::ecs().AddComponent<SkillPoint>(entity, skillpt);
				}
			}
			if (entityObject.HasMember("AttackSkill")) {
				AttackSkill atkSkill;
				const rapidjson::Value& atkSkillObject = entityObject["AttackSkill"];
				atkSkill.skillIndex = atkSkillObject["Skill Index"].GetInt();

				if (ECS::ecs().HasComponent<AttackSkill>(entity)) {
					ECS::ecs().GetComponent<AttackSkill>(entity) = atkSkill;
				}
				else {
					ECS::ecs().AddComponent<AttackSkill>(entity, atkSkill);
				}
			}
			if (entityObject.HasMember("SkillIcon")) {
				ECS::ecs().AddComponent<SkillIcon>(entity, SkillIcon{});
			}
			if (entityObject.HasMember("SkillCost")) {
				ECS::ecs().AddComponent<SkillCost>(entity, SkillCost{});
			}
			if (entityObject.HasMember("SkillAttackType")) {
				ECS::ecs().AddComponent<SkillAttackType>(entity, SkillAttackType{});
			}
			if (entityObject.HasMember("AllyHUD")) {
				AllyHUD allyHud;
				const rapidjson::Value& allyHudObject = entityObject["AllyHUD"];
				if (allyHudObject.HasMember("Ally Index")) {
					allyHud.allyIndex = allyHudObject["Ally Index"].GetInt();
				}

				if (ECS::ecs().HasComponent<AllyHUD>(entity)) {
					ECS::ecs().GetComponent<AllyHUD>(entity) = allyHud;
				}
				else {
					ECS::ecs().AddComponent<AllyHUD>(entity, allyHud);
				}
			}
			if (entityObject.HasMember("EnemyHUD")) {
				EnemyHUD enemyHud;
				const rapidjson::Value& enemyHudObject = entityObject["EnemyHUD"];
				if (enemyHudObject.HasMember("Enemy Index")) {
					enemyHud.enemyIndex = enemyHudObject["Enemy Index"].GetInt();
				}

				if (ECS::ecs().HasComponent<EnemyHUD>(entity)) {
					ECS::ecs().GetComponent<EnemyHUD>(entity) = enemyHud;
				}
				else {
					ECS::ecs().AddComponent<EnemyHUD>(entity, enemyHud);
				}
			}
			if (entityObject.HasMember("DialogueSpeaker")) {
				ECS::ecs().AddComponent<DialogueSpeaker>(entity, DialogueSpeaker{});
			}
			if (entityObject.HasMember("DialogueHUD")) {
				DialogueHUD dialogueHud;
				const rapidjson::Value& dialogueHudObject = entityObject["DialogueHUD"];

				// Check for dialogues
				if (dialogueHudObject.HasMember("Dialogues") && dialogueHudObject["Dialogues"].IsArray()) {
					const rapidjson::Value& dialoguesArray = dialogueHudObject["Dialogues"];
					for (rapidjson::SizeType j = 0; j < dialoguesArray.Size(); ++j) {
						const rapidjson::Value& dialogueObject = dialoguesArray[j];
						DialogueHUD::Dialogue dialogue;

						// Check if any dialogue lines have been added
						if (dialogueObject.HasMember("Dialogue Lines") && dialogueObject["Dialogue Lines"].IsArray()) {
							const rapidjson::Value& dialogueLinesArray = dialogueObject["Dialogue Lines"];
							for (rapidjson::SizeType k = 0; k < dialogueLinesArray.Size(); ++k) {
								if (dialogueLinesArray[k].IsObject()) {
									DialogueHUD::DialogueLine line;
									line.speaker = dialogueLinesArray[k]["Speaker"].GetString();
									line.line = dialogueLinesArray[k]["Line"].GetString();
									if (dialogueLinesArray[k].HasMember("Voice")) {
										line.voice = dialogueLinesArray[k]["Voice"].GetString();
									}
									dialogue.dialogueLines.push_back(line);
								}
							}
						}

						if (dialogueObject.HasMember("Trigger Type")) {
							dialogue.triggerType = static_cast<DIALOGUE_TRIGGER>(dialogueObject["Trigger Type"].GetInt());
						}
						if (dialogueObject.HasMember("Round Trigger")) {
							dialogue.roundTrigger = dialogueObject["Round Trigger"].GetInt();
						}
						if (dialogueObject.HasMember("Health Trigger")) {
							dialogue.healthTrigger = dialogueObject["Health Trigger"].GetInt();
						}

						if (dialogueObject.HasMember("Viewing Index")) {
							dialogue.viewingIndex = dialogueObject["Viewing Index"].GetInt();
						}

						if (dialogueObject.HasMember("Display Duration")) {
							dialogue.displayDuration = dialogueObject["Display Duration"].GetFloat();
						}

						if (dialogueObject.HasMember("Is Active")) {
							dialogue.isActive = dialogueObject["Is Active"].GetBool();
						}

						if (dialogueObject.HasMember("Is Triggered")) {
							dialogue.isTriggered = dialogueObject["Is Triggered"].GetBool();
						}
						/*if (dialogueHudObject.HasMember("Auto Launch")) {
							dialogue.autoLaunch = dialogueHudObject["Auto Launch"].GetBool();
						}*/
						if (dialogueObject.HasMember("Speaker Required")) {
							dialogue.speakerRequired = dialogueObject["Speaker Required"].GetBool();
						}
						if (dialogueObject.HasMember("Post Dialogue Scene")) {
							dialogue.postDialogueScene = dialogueObject["Post Dialogue Scene"].GetBool();
						}
						if (dialogueObject.HasMember("Target Scene")) {
							dialogue.targetScene = dialogueObject["Target Scene"].GetString();
						}

						dialogueHud.dialogues.push_back(dialogue);
					}
				}						

				if (ECS::ecs().HasComponent<DialogueHUD>(entity)) {
					ECS::ecs().GetComponent<DialogueHUD>(entity) = dialogueHud;
				}
				else {
					ECS::ecs().AddComponent<DialogueHUD>(entity, dialogueHud);
				}
			}
			if (entityObject.HasMember("TurnIndicator")) {
				ECS::ecs().AddComponent<TurnIndicator>(entity, TurnIndicator{});
			}
			if (entityObject.HasMember("StatusEffect")) {
				ECS::ecs().AddComponent<StatusEffect>(entity, StatusEffect{});
			}
			if (entityObject.HasMember("Animation Set")) {
				AnimationSet animset{};
				
				if (entityObject.HasMember("Animation Set Default Animation")) {
					const rapidjson::Value& defaultanimObject = entityObject["Animation Set Default Animation"];
					animset.defaultAnimation = defaultanimObject.GetString();
				}

				for (auto& animGroups : entityObject["Animation Set"].GetArray()) {
					AnimationGroup anigrp{};
					anigrp.totalFrames = animGroups["Total Frames"].GetInt();
					anigrp.name = animGroups["Group Name"].GetString();
					anigrp.loop = animGroups["Loop"].GetBool();
					if (animGroups.HasMember("Frame Time")) {
						anigrp.frametime = animGroups["Frame Time"].GetFloat();
					}
					for (auto& animations : animGroups["Animations"].GetArray()) {
						std::string animType = animations["Animation Type"].GetString();
						if (animType == "Sprite") {
							SpriteAnimation a{};
							for (auto& k : animations["Key Frames"].GetArray()) {
								bool data{ false };
								if (k.HasMember("Reverse")) {
									data = k["Reverse"].GetBool();
								}
								a.AddKeyFrame(k["Frame Number"].GetInt(), &data);
							}
							anigrp.animations.push_back(std::make_shared<SpriteAnimation>(a));
						}
						else if (animType == "TextureChange") {
							ChangeTexAnimation a{};
							for (auto& k : animations["Key Frames"].GetArray()) {
								std::string data{ k["Texture"].GetString() };
								a.AddKeyFrame(k["Frame Number"].GetInt(), &data);
							}
							anigrp.animations.push_back(std::make_shared<ChangeTexAnimation>(a));
						}
						else if (animType == "Sound") {
							SoundAnimation a{};
							for (auto& k : animations["Key Frames"].GetArray()) {
								std::vector<std::string> data{};
								if (k.HasMember("Sound")) {
									data.push_back(k["Sound"].GetString());
								}
								else if (k.HasMember("Sounds")) {
									for (auto& s : k["Sounds"].GetArray()) {
										data.push_back(s.GetString());
									}
								}
								a.AddKeyFrame(k["Frame Number"].GetInt(), &data);
							}
							anigrp.animations.push_back(std::make_shared<SoundAnimation>(a));
						}
						else if (animType == "Swap") {
							SwapAnimation a{};
							for (auto& k : animations["Key Frames"].GetArray()) {
								std::string data{ k["Destination"].GetString() };
								a.AddKeyFrame(k["Frame Number"].GetInt(), &data);
							}
							anigrp.animations.push_back(std::make_shared<SwapAnimation>(a));
						}
						else if (animType == "TransformAttach") {
							TransformAttachAnimation a{};
							for (auto& k : animations["Key Frames"].GetArray()) {
								std::string data = k["Target"].GetString();
								a.AddKeyFrame(k["Frame Number"].GetInt(), &data);
							}
							anigrp.animations.push_back(std::make_shared<TransformAttachAnimation>(a));
						}
						else if (animType == "TransformDirect") {
							TransformDirectAnimation a{};
							for (auto& k : animations["Key Frames"].GetArray()) {
								Transform data{};
								data.position.x = k["X movement"].GetFloat();
								data.position.y = k["Y movement"].GetFloat();
								data.rotation = k["Rotation"].GetFloat();
								data.scale = k["Scale"].GetFloat();
								a.AddKeyFrame(k["Frame Number"].GetInt(), &data);
							}
							anigrp.animations.push_back(std::make_shared<TransformDirectAnimation>(a));
						}
						else if (animType == "Color") {
							ColorAnimation a{};
							for (auto& k : animations["Key Frames"].GetArray()) {
								glm::vec3 data{};
								data.r = k["Red"].GetFloat();
								data.g = k["Green"].GetFloat();
								data.b = k["Blue"].GetFloat();
								a.AddKeyFrame(k["Frame Number"].GetInt(), &data);
							}
							anigrp.animations.push_back(std::make_shared<ColorAnimation>(a));
						}
						else if (animType == "Fade") {
							FadeAnimation a{};
							for (auto& k : animations["Key Frames"].GetArray()) {
								float data{ k["Alpha"].GetFloat() };
								a.AddKeyFrame(k["Frame Number"].GetInt(), &data);
							}
							anigrp.animations.push_back(std::make_shared<FadeAnimation>(a));
						}
						else if (animType == "SelfDestruct") {
							SelfDestructAnimation a{};
							for (auto& k : animations["Key Frames"].GetArray()) {
								a.AddKeyFrame(k["Frame Number"].GetInt(), nullptr);
							}
							anigrp.animations.push_back(std::make_shared<SelfDestructAnimation>(a));
						}
						else if (animType == "DamageImpact") {
							DamageImpactAnimation a{};
							for (auto& k : animations["Key Frames"].GetArray()) {
								std::string data{};
								if (k.HasMember("Prefab")) {
									data = k["Prefab"].GetString();
								}
								a.AddKeyFrame(k["Frame Number"].GetInt(), &data);
							}
							anigrp.animations.push_back(std::make_shared<DamageImpactAnimation>(a));
						}
						else if (animType == "CameraZoom") {
							CameraZoomAnimation a{};
							for (auto& k : animations["Key Frames"].GetArray()) {
								float data{ k["Zoom"].GetFloat() };
								a.AddKeyFrame(k["Frame Number"].GetInt(), &data);
							}
							anigrp.animations.push_back(std::make_shared<CameraZoomAnimation>(a));
						}
						else if (animType == "CameraTarget") {
							CameraTargetAnimation a{};
							for (auto& k : animations["Key Frames"].GetArray()) {
								a.AddKeyFrame(k["Frame Number"].GetInt(), nullptr);
							}
							anigrp.animations.push_back(std::make_shared<CameraTargetAnimation>(a));
						}
						else if (animType == "CameraReset") {
							CameraResetAnimation a{};
							for (auto& k : animations["Key Frames"].GetArray()) {
								a.AddKeyFrame(k["Frame Number"].GetInt(), nullptr);
							}
							anigrp.animations.push_back(std::make_shared<CameraResetAnimation>(a));
						}
						else if (animType == "CreatePrefab") {
							CreatePrefabAnimation a{};
							for (auto& k : animations["Key Frames"].GetArray()) {
								std::string data = k["Prefab"].GetString();
								a.AddKeyFrame(k["Frame Number"].GetInt(), &data);
							}
							anigrp.animations.push_back(std::make_shared<CreatePrefabAnimation>(a));
						}
						else if (animType == "Event") {
							EventAnimation a{};
							for (auto& k : animations["Key Frames"].GetArray()) {
								std::pair<std::string, std::string> data{};
								data.first = k["Event Name"].GetString();
								data.second = k["Event Input"].GetString();
								a.AddKeyFrame(k["Frame Number"].GetInt(), &data);
							}
							anigrp.animations.push_back(std::make_shared<EventAnimation>(a));
						}
						else if (animType == "Child") {
							ChildAnimation a{};
							for (auto& k : animations["Key Frames"].GetArray()) {
								std::pair<std::string, std::string> data{};
								data.first = k["Child Name"].GetString();
								data.second = k["Child Animation"].GetString();
								a.AddKeyFrame(k["Frame Number"].GetInt(), &data);
							}
							anigrp.animations.push_back(std::make_shared<ChildAnimation>(a));
						}
						else if (animType == "Parent") {
							ParentAnimation a{};
							for (auto& k : animations["Key Frames"].GetArray()) {
								std::string data{};
								data = k["Animation"].GetString();
								a.AddKeyFrame(k["Frame Number"].GetInt(), &data);
							}
							anigrp.animations.push_back(std::make_shared<ParentAnimation>(a));
							}
					}
					animset.animationSet.push_back(anigrp);
				}

				if (ECS::ecs().HasComponent<AnimationSet>(entity)) {
					ECS::ecs().GetComponent<AnimationSet>(entity) = animset;
				}
				else {
					ECS::ecs().AddComponent<AnimationSet>(entity, animset);
				}
			}
			if (entityObject.HasMember("Parent") && !ECS::ecs().HasComponent<Parent>(entity) && parentID == 0) {
				ECS::ecs().AddComponent<Parent>(entity, Parent{});
				parent = &ECS::ecs().GetComponent<Parent>(entity);
				parentID = entity;
			}
			if (entityObject.HasMember("Child") && parent != nullptr) {
				const rapidjson::Value& transformObject = entityObject["Child"];
				Transform transform;
				transform.position.x = transformObject["position_x"].GetFloat();
				transform.position.y = transformObject["position_y"].GetFloat();
				transform.rotation = transformObject["rotation"].GetFloat();
				transform.scale = transformObject["scale"].GetFloat();
				transform.velocity.x = transformObject["velocity_x"].GetFloat();
				transform.velocity.y = transformObject["velocity_y"].GetFloat();

				ECS::ecs().AddComponent<Child>(entity, Child{ parentID, transform });
				parent->children.push_back(entity);
			}
			if (entityObject.HasMember("SliderUI")) {
				const rapidjson::Value& sliderObject = entityObject["SliderUI"];
				SliderUI sliderUI;
				sliderUI.linkedEntity = sliderObject["Linked Entity"].GetUint();
				sliderUI.type = static_cast<SliderUI::UI_TYPE>(sliderObject["Slider Type"].GetInt());
				sliderUI.controlWhich = static_cast<SliderUI::CONTROL_WHICH>(sliderObject["Control Which"].GetInt());
				ECS::ecs().AddComponent<SliderUI>(entity, sliderUI);

			}
			//if (entityObject.HasMember("Emitter")) {
			//	ECS::ecs().AddComponent<Emitter>(entity, Emitter{});
			//}
		}
	}

	if (!isPrefab) {
		RebuildLayeringAfterDeserialization();
		ExtractSkipLockAfterDeserialization();
	}

	if (isPrefab && ECS::ecs().HasComponent<Clone>(entity)) {
		ECS::ecs().RemoveComponent<Clone>(entity);
	}

	
	// To load the state from a file for reflection
	if (parentID == 0) {
		return entity;
	}
	else {
		return parentID;
	}
}

void LoadConfig() {
	// Open the file
	std::ifstream ifs("Config.txt");
	if (!ifs.is_open()) {
	// Check if the file is open
		std::cerr << "Unable to load Configuration File!" << std::endl;
	}

	std::string temp;

	int height;
	int width;

	// Read and extract the width
	ifs >> temp >> width;

	// Read and extract the height
	ifs >> temp >> height;

	GRAPHICS::UpdateConstants(width, height);

	// Close the file
	ifs.close();
}

void WriteSpriteConfig(const char* filename, int rows, int cols) {
	std::string tempFilename = filename;
	std::ostringstream oss;
	oss << "..\\Assets\\Textures\\" << tempFilename.substr(0, tempFilename.find_last_of('.')) << ".spritesheet";

	// Open the file
	std::ofstream ofs(oss.str().c_str());
	if (!ofs.is_open()) {
		// Check if the file is open
		std::cerr << "Unable to write spritesheet config!" << std::endl;
	}

	ofs << filename << "\n" << rows << "\n" << cols << "\n" << rows * cols;

	// Close the file
	ofs.close();
}