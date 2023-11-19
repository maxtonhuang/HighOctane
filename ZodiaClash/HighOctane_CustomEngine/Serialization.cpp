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

//extern std::unordered_map<std::string, Entity> masterEntitiesList;

extern std::vector<VariableInfo> variablesTEST;

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



rapidjson::Value SerializeTex(const Tex& tex, rapidjson::Document::AllocatorType& allocator) {
	rapidjson::Value texObject(rapidjson::kObjectType);
	texObject.AddMember("Texture Index", tex.texVariantIndex, allocator);
	texObject.AddMember("Rows", tex.rows, allocator);
	texObject.AddMember("Columns", tex.cols, allocator);
	texObject.AddMember("Sprite Number", tex.spritenum, allocator);
	rapidjson::Value filePathValue;
	filePathValue.SetString(tex.tex->GetName().c_str(), static_cast<rapidjson::SizeType>(tex.tex->GetName().length()), allocator);
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
rapidjson::Value SerializeScript(const Script& script, rapidjson::Document::AllocatorType& allocator) {
	rapidjson::Value scriptObject(rapidjson::kObjectType);

	// Serialize the className
	scriptObject.AddMember("className", rapidjson::Value(script.className.c_str(), allocator).Move(), allocator);

	// Serialize the scriptNameVec
	rapidjson::Value scriptNameArray(rapidjson::kArrayType);
	for (const std::string& name : script.scriptNameVec) {
		scriptNameArray.PushBack(rapidjson::Value(name.c_str(), allocator).Move(), allocator);
	}

	scriptObject.AddMember("scriptNameVec", scriptNameArray, allocator);

	return scriptObject;
}

rapidjson::Value SerializeCharacterStats(const CharacterStats& stats, rapidjson::Document::AllocatorType& allocator) {
	rapidjson::Value charstats(rapidjson::kObjectType);
	rapidjson::Value attacks(rapidjson::kArrayType);
	charstats.AddMember("Character type", (int)stats.tag, allocator);
	charstats.AddMember("Max Health", stats.stats.maxHealth, allocator);
	charstats.AddMember("Attack", stats.stats.attack, allocator);
	charstats.AddMember("Defense", stats.stats.defense, allocator);
	charstats.AddMember("Speed", stats.stats.speed, allocator);

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
	return modelObject;
}

rapidjson::Value SerializeCollider(const Collider& collider, rapidjson::Document::AllocatorType& allocator) {
	rapidjson::Value colliderObject(rapidjson::kObjectType);
	colliderObject.AddMember("Collider Enum", (int)collider.bodyShape, allocator);
	return colliderObject;
}

rapidjson::Value SerializeTextLabel(const TextLabel& textLabel, rapidjson::Document::AllocatorType& allocator) {
	rapidjson::Value textObject(rapidjson::kObjectType);
	
	if (textLabel.font != nullptr) {
		auto fontInfo = textLabel.font->GetInfo();
		textObject.AddMember("Font Family", rapidjson::Value(fontInfo.first.c_str(), allocator).Move(), allocator);
		textObject.AddMember("Font Variant", rapidjson::Value(fontInfo.second.c_str(), allocator).Move(), allocator);
	}
	textObject.AddMember("Text String", rapidjson::Value(textLabel.textString.c_str(), allocator).Move(), allocator);
	textObject.AddMember("r", textLabel.textColor.r, allocator);
	textObject.AddMember("g", textLabel.textColor.g, allocator);
	textObject.AddMember("b", textLabel.textColor.b, allocator);
	textObject.AddMember("a", textLabel.textColor.a, allocator);

	
	textObject.AddMember("Color Preset", rapidjson::Value(textLabel.initClr.c_str(), allocator), allocator);
	
	textObject.AddMember("Horizontal Alignment", (int)textLabel.hAlignment, allocator);
	textObject.AddMember("Vertical Alignment", (int)textLabel.vAlignment, allocator);

	return textObject;
}
rapidjson::Value SerializeButton(const Button& button, rapidjson::Document::AllocatorType& allocator) {
	rapidjson::Value buttonObject(rapidjson::kObjectType);

	buttonObject.AddMember("Button R", button.defaultColor.buttonColor.r, allocator);
	buttonObject.AddMember("Button G", button.defaultColor.buttonColor.g, allocator);
	buttonObject.AddMember("Button B", button.defaultColor.buttonColor.b, allocator);
	buttonObject.AddMember("Button A", button.defaultColor.buttonColor.a, allocator);
	
	buttonObject.AddMember("Text R", button.defaultColor.textColor.r, allocator);
	buttonObject.AddMember("Text G", button.defaultColor.textColor.g, allocator);
	buttonObject.AddMember("Text B", button.defaultColor.textColor.b, allocator);
	buttonObject.AddMember("Text A", button.defaultColor.textColor.a, allocator);

	buttonObject.AddMember("Event Name", rapidjson::Value(button.eventName.c_str(), allocator).Move(), allocator);
	buttonObject.AddMember("Event Input", rapidjson::Value(button.eventInput.c_str(), allocator).Move(), allocator);
	buttonObject.AddMember("Padding Top", button.padding.top, allocator);
	buttonObject.AddMember("Padding Bottom", button.padding.bottom, allocator);
	buttonObject.AddMember("Padding Left", button.padding.left, allocator);
	buttonObject.AddMember("Padding Right", button.padding.right, allocator);
	buttonObject.AddMember("Padding Setting", button.padding.setting, allocator);

	return buttonObject;
}

rapidjson::Value SerializeAnimationSet(const AnimationSet& animSet, rapidjson::Document::AllocatorType& allocator) {
	rapidjson::Value set(rapidjson::kArrayType);

	for (auto const& a : animSet.animationSet) {
		rapidjson::Value group(rapidjson::kObjectType);
		rapidjson::Value animations(rapidjson::kArrayType);
		rapidjson::Value groupName;
		group.AddMember("Total Frames", a.totalFrames, allocator);
		group.AddMember("Group Name", rapidjson::Value(a.name.c_str(), allocator).Move(), allocator);
		group.AddMember("Loop", a.loop, allocator);
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
					keyframe.AddMember("Frame Number", k.frameNum, allocator);
					keyframe.AddMember("Sound", rapidjson::Value(k.data.c_str(), allocator).Move(), allocator);
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


void Serializer::SaveEntityToJson(const std::string& fileName, const std::set<Entity>& m_entity) {
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
		for (const std::string& layerName : layerNames) {
			layerNamesArray.PushBack(rapidjson::Value(layerName.c_str(), allocator), allocator);
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
	//Animator* anim = nullptr;
	Name* name = nullptr;
	Script* script = nullptr;
	CharacterStats* charstats = nullptr;
	Model* model = nullptr;
	TextLabel* textLabel = nullptr;
	Button* button = nullptr;
	Collider* collider = nullptr;
	AnimationSet* animset = nullptr;

	for (const Entity& entity : m_entity) {
		
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
			DECLARE(float, redCol, color->color.r);
			DECLARE(float, greenCol, color->color.g);
			DECLARE(float, blueCol, color->color.b);
			DECLARE(float, alphaCol, color->color.a);
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
		//if (CheckSerialize<Animator>(entity, isPrefabClone, uComponentMap)) {
		//	anim = &ECS::ecs().GetComponent<Animator>(entity);
		//	rapidjson::Value animationObject = SerializeAnimation(*anim, allocator);
		//	entityObject.AddMember("Animation", animationObject, allocator);
		//}
		if (CheckSerialize<Script>(entity, isPrefabClone, uComponentMap)) {
			script = &ECS::ecs().GetComponent<Script>(entity);
			rapidjson::Value scriptObject = SerializeScript(*script, allocator);
			entityObject.AddMember("Scripts", scriptObject, allocator);
		}
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
		if (CheckSerialize<Collider>(entity, isPrefabClone, uComponentMap)) {
			collider = &ECS::ecs().GetComponent<Collider>(entity);
			rapidjson::Value colliderObject = SerializeCollider(*collider, allocator);
			entityObject.AddMember("Collider", colliderObject, allocator);
		}
		if (CheckSerialize<AnimationSet>(entity, isPrefabClone, uComponentMap)) {
			animset = &ECS::ecs().GetComponent<AnimationSet>(entity);
			rapidjson::Value animsetObject = SerializeAnimationSet(*animset, allocator);
			entityObject.AddMember("Animation Set", animsetObject, allocator);
		}
		document.PushBack(entityObject, allocator);
		//document.PushBack(entityArray, allocator);
	}
	selectedLayer = 0;
	currentLayer = 0;
	
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
	SerializeVariablesToFile("variables.sav", variablesTEST);
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
		layerNames.clear();
		for (rapidjson::SizeType i = 0; i < layerNamesArray.Size(); ++i) {
			if (layerNamesArray[i].IsString()) {
				layerNames.push_back(layerNamesArray[i].GetString());
			}
		}
	}
	
}

Entity Serializer::LoadEntityFromJson(const std::string& fileName, bool isPrefab) {
	rapidjson::Document document;
	std::ifstream file(fileName);
	Entity entity{0};
	if (!file.is_open()) {
		std::cerr << "Failed to open file: " << fileName << std::endl;
		//Assert("Failed to open .json file %s", fileName);
		return entity;
	}
	rapidjson::IStreamWrapper isw(file);
	document.ParseStream(isw);
	//const rapidjson::Value& layeringObject = document[i];
	if (document.HasParseError()) {
		std::cerr << "Failed to parse .json file: " << fileName << std::endl;
	}
	layering.clear();
	std::cout << "Before loading " << ECS::ecs().GetEntityCount() << std::endl;

	printf("Total entites before loading %d \n", static_cast<int>(s_ptr->m_Entities.size()));

	for (rapidjson::SizeType i = 0; i < document.Size(); ++i) {
		const rapidjson::Value& entityObject = document[i];
		
		if (document.Size() > 0 && document[i].HasMember("LayeringSystems")) {
			const rapidjson::Value& layeringObject = document[0]["LayeringSystems"];
			LoadLayeringData(layeringObject);
		}
		else {
			//entity = ECS::ecs().CreateEntity();
			if (entityObject.HasMember("Clone")) {
				const rapidjson::Value& cloneObject = entityObject["Clone"];
				//ECS::ecs().AddComponent(entity, Clone{});
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
				//////////////////////////////////////////////////////////////////////////// <-------
				if (!stopButton) {
					//stop crashing if there is no selected layer
					//if (selectedLayer == std::numeric_limits<size_t>::max()) {
					selectedLayer = currentLayer = 0;
					//}
					/*if (layering.size() == 0) {
						std::deque<Entity> temp;
						layering.emplace_back(temp);
					}
					layering[selectedLayer].push_back(entity);*/
				}
			}

			if (entity == 0) {
				entity = ECS::ecs().CreateEntity();
				(EntityFactory::entityFactory().cloneCounter)++;
			}

			if (entityObject.HasMember("Entity")) {
				//std::string entityName = entityObject["Entity Name"].GetString();
				//EntityFactory::entityFactory().masterEntitiesList[entityName] = entity;
				const rapidjson::Value& nameObject = entityObject["Entity"];
				Name name{};
				name.name = nameObject["Name"].GetString();
				//name.selected = nameObject["Selected"].GetBool();
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
				//Texture* texture = assetmanager.LoadTexture(filePath);

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

			//if (entityObject.HasMember("Animation")) {
			//	const rapidjson::Value& animObject = entityObject["Animation"];
			//	Animator anim{
			//		static_cast<Animator::ANIMATION_TYPE>(animObject["Animation Type"].GetInt()),
			//		animObject["Frame Display Duration"].GetFloat()
			//	};
			//	//anim.animationType = static_cast<Animator::ANIMATION_TYPE>(animObject["Animation Type"].GetInt());
			//	//anim.frameIndex = animObject["Frame Index"].GetUint();
			//	//anim.frameTimeElapsed = animObject["Frame Time Elapsed"].GetFloat();
			//	//anim.frameDisplayDuration = animObject["Frame Display Duration"].GetFloat();
			//	ECS::ecs().AddComponent<Animator>(entity, anim);
			//}
			if (entityObject.HasMember("Scripts")) {
				const rapidjson::Value& scriptObject = entityObject["Scripts"];
				Script script;

				if (scriptObject.HasMember("className")) {
					script.className = scriptObject["className"].GetString();
				}
				if (scriptObject.HasMember("scriptNameVec") && scriptObject["scriptNameVec"].IsArray()) {
					const rapidjson::Value& scriptNameArray = scriptObject["scriptNameVec"];
					for (rapidjson::SizeType j = 0; j < scriptNameArray.Size(); ++j) {
						if (scriptNameArray[j].IsString()) {
							script.scriptNameVec.push_back(scriptNameArray[j].GetString());
						}
					}
				}
				if (scriptObject.HasMember("scriptNameVecForImGui") && scriptObject["scriptNameVecForImGui"].IsArray()) {
					const rapidjson::Value& scriptNameArray = scriptObject["scriptNameVecForImGui"];
					for (rapidjson::SizeType j = 0; j < scriptNameArray.Size(); ++j) {
						if (scriptNameArray[j].IsString()) {
							script.scriptNameVec.push_back(scriptNameArray[j].GetString());
						}
					}
				}

				if (ECS::ecs().HasComponent<Script>(entity)) {
					ECS::ecs().GetComponent<Script>(entity) = script;
				}
				else {
					ECS::ecs().AddComponent<Script>(entity, script);
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
				collider.bodyShape = static_cast<Collider::SHAPE_ID>(enumID);

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
			if (entityObject.HasMember("CharacterStats")) {
				const rapidjson::Value& statsObject = entityObject["CharacterStats"];
				CharacterStats charstats;
				charstats.stats.attack = statsObject["Attack"].GetFloat();
				charstats.stats.defense = statsObject["Defense"].GetFloat();
				charstats.stats.maxHealth = statsObject["Max Health"].GetFloat();
				charstats.stats.health = charstats.stats.maxHealth;
				charstats.stats.speed = statsObject["Speed"].GetInt();
				charstats.tag = (CharacterType)statsObject["Character type"].GetInt();
				for (auto& a : statsObject["Skills"].GetArray()) {
					charstats.action.skills.push_back(assetmanager.attacks.data[a.GetString()]);
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

				textLabel.textString = textObject["Text String"].GetString();

				textLabel.textColor.r = textObject["r"].GetFloat();
				textLabel.textColor.g = textObject["g"].GetFloat();
				textLabel.textColor.b = textObject["b"].GetFloat();
				textLabel.textColor.a = textObject["a"].GetFloat();

				textLabel.initClr = textObject["Color Preset"].GetString();
				//TextLabel(textLabel.textString, textLabel.textColor);

				if (textObject.HasMember("Horizontal Alignment") && textObject.HasMember("Vertical Alignment")) {
					textLabel.hAlignment = (UI_HORIZONTAL_ALIGNMENT)(textObject["Horizontal Alignment"].GetInt());
					textLabel.vAlignment = (UI_VERTICAL_ALIGNMENT)(textObject["Vertical Alignment"].GetInt());
				}

				// ECS::ecs().AddComponent(entity, textLabel);
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
				//button.colorSet = Button::ColorSet(); // Initialize the colorSet struct
				glm::vec4 buttonColor{};
				if (buttonObject.HasMember("Button R") && buttonObject.HasMember("Button G") &&
					buttonObject.HasMember("Button B") && buttonObject.HasMember("Button A")) {
					buttonColor.r = buttonObject["Button R"].GetFloat();
					buttonColor.g = buttonObject["Button G"].GetFloat();
					buttonColor.b = buttonObject["Button B"].GetFloat();
					buttonColor.a = buttonObject["Button A"].GetFloat();
				}

				glm::vec4 textColor{};
				if (buttonObject.HasMember("Text R") && buttonObject.HasMember("Text G") &&
					buttonObject.HasMember("Text B") && buttonObject.HasMember("Text A")) {
					textColor.r = buttonObject["Text R"].GetFloat();
					textColor.g = buttonObject["Text G"].GetFloat();
					textColor.b = buttonObject["Text B"].GetFloat();
					textColor.a = buttonObject["Text A"].GetFloat();
				}

				// update states
				button = { buttonColor, textColor };

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
			if (entityObject.HasMember("Animation Set")) {
				AnimationSet animset{};
				for (auto& animGroups : entityObject["Animation Set"].GetArray()) {
					AnimationGroup anigrp{};
					anigrp.totalFrames = animGroups["Total Frames"].GetInt();
					anigrp.name = animGroups["Group Name"].GetString();
					anigrp.loop = animGroups["Loop"].GetBool();
					for (auto& animations : animGroups["Animations"].GetArray()) {
						std::string animType = animations["Animation Type"].GetString();
						if (animType == "Sprite") {
							SpriteAnimation a{};
							for (auto& k : animations["Key Frames"].GetArray()) {
								a.AddKeyFrame(k["Frame Number"].GetInt(), nullptr);
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
								std::string data{ k["Sound"].GetString() };
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
		}
	}
	if (isPrefab && ECS::ecs().HasComponent<Clone>(entity)) {
		ECS::ecs().RemoveComponent<Clone>(entity);
	}
	RebuildLayeringAfterDeserialization();
	ExtractSkipLockAfterDeserialization();
	//std::cout << "All loaded " << ECS::ecs().GetEntityCount() << std::endl;
	printf("Total entites loading %d \n", static_cast<int>(s_ptr->m_Entities.size()));

	// To load the state from a file for reflection
	//DeserializeVariablesFromFile("variables.sav", variablesTEST);
	return entity;
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