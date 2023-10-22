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
* 
*	!--Future Updates--!
*	
*	-M1 Checkpoint-
*	- Attempt at creating a CSV parser
*	- Create a Json to CSV, Vice versa
*	- Find better optimization techniques for serialization
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
#include <shobjidl.h>

//extern std::unordered_map<std::string, Entity> masterEntitiesList;

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
		//DEBUG_PRINT("ID:%d Name:%s  Health:%f  Attack:%f  Defence:%f  Speed:%f", characterStats.id, characterStats.name.c_str(), characterStats.health, characterStats.attack, characterStats.defence, characterStats.speed);

		// Check for newline to indicate the end of a line
		if (c == '\n' || stream.stream.eof()) {
			// Reset characterStats for the next line
			characterStats = PseudoCharacter();
		}
	}
}


rapidjson::Value SerializeTransform(const Transform& transform, rapidjson::Document::AllocatorType& allocator) {
	rapidjson::Value transformObject(rapidjson::kObjectType);
	transformObject.AddMember("position_x", transform.position.x, allocator);
	transformObject.AddMember("position_y", transform.position.y, allocator);
	transformObject.AddMember("rotation", transform.rotation, allocator);
	transformObject.AddMember("scale_x", transform.scale, allocator);
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

rapidjson::Value SerializeMainCharacter(const MainCharacter& /* mainCharacter */, rapidjson::Document::AllocatorType& allocator) {
	rapidjson::Value MCObject(rapidjson::kObjectType);
	MCObject.AddMember("isMainCharacter", true, allocator);
	return MCObject;
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

void Serializer::SaveEntityToJson(const std::string& fileName, const std::set<Entity>& m_entity) {
	// Create a JSON document
	rapidjson::Document document;
	document.SetArray();
	rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

	Color* color = nullptr;
	Transform* transform = nullptr;
	Tex* tex = nullptr;
	Visible* visible = nullptr;
	Size* size = nullptr;
	MainCharacter* mainCharacter = nullptr;
	Circle* circle = nullptr;
	AABB* aabb = nullptr;
	Animator* anim = nullptr;
	
	for (const Entity& entity : m_entity) {
		//rapidjson::Value entityArray(rapidjson::kArrayType);

		rapidjson::Value entityObject(rapidjson::kObjectType);

		entityObject.AddMember("Entity Name", "Duck", allocator);
		if (ECS::ecs().HasComponent<Color>(entity)) {
			color = &ECS::ecs().GetComponent<Color>(entity);
			rapidjson::Value colorObject = SerializeColor(*color, allocator);
			entityObject.AddMember("Color", colorObject, allocator);

		}
		if (ECS::ecs().HasComponent<Transform>(entity)) {
			transform = &ECS::ecs().GetComponent<Transform>(entity);
			rapidjson::Value transformObject = SerializeTransform(*transform, allocator);
			entityObject.AddMember("Transform", transformObject, allocator);
		}
		if (ECS::ecs().HasComponent<Tex>(entity)) {
			tex = &ECS::ecs().GetComponent<Tex>(entity);
			rapidjson::Value textureObject = SerializeTex(*tex, allocator);
			entityObject.AddMember("Texture", textureObject, allocator);
		}
		if (ECS::ecs().HasComponent<Visible>(entity)) {
			visible = &ECS::ecs().GetComponent<Visible>(entity);
			rapidjson::Value visibleObject = SerializeVisible(*visible, allocator);
			entityObject.AddMember("Visible", visibleObject, allocator);
		}
		if (ECS::ecs().HasComponent<Size>(entity)) {
			size = &ECS::ecs().GetComponent<Size>(entity);
			rapidjson::Value sizeObject = SerializeSize(*size, allocator);
			entityObject.AddMember("Size", sizeObject, allocator);
		}
		if (ECS::ecs().HasComponent<MainCharacter>(entity)) {
			mainCharacter = &ECS::ecs().GetComponent<MainCharacter>(entity);
			rapidjson::Value MCObject = SerializeMainCharacter(*mainCharacter, allocator);
			entityObject.AddMember("MainCharacter", MCObject, allocator);
		}
		if (ECS::ecs().HasComponent<Circle>(entity)) {
			circle = &ECS::ecs().GetComponent<Circle>(entity);
			rapidjson::Value circleObject = SerializeCircle(*circle, allocator);
			entityObject.AddMember("Circle", circleObject, allocator);
		}
		if (ECS::ecs().HasComponent<AABB>(entity)) {
			aabb = &ECS::ecs().GetComponent<AABB>(entity);
			rapidjson::Value aabbObject = SerializeAABB(*aabb, allocator);
			entityObject.AddMember("Collision", aabbObject, allocator);
		}
		if (ECS::ecs().HasComponent<Animator>(entity)) {
			anim = &ECS::ecs().GetComponent<Animator>(entity);
			rapidjson::Value animationObject = SerializeAnimation(*anim, allocator);
			entityObject.AddMember("Animation", animationObject, allocator);
		}
		document.PushBack(entityObject, allocator);
		//document.PushBack(entityArray, allocator);
	}
	
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
}

bool Serializer::LoadEntityFromJson(const std::string& fileName) {
	rapidjson::Document document;
	std::ifstream file(fileName);
	if (!file.is_open()) {
		std::cerr << "Failed to open file: " << fileName << std::endl;
		//Assert("Failed to open .json file %s", fileName);
		return false;
	}
	rapidjson::IStreamWrapper isw(file);
	document.ParseStream(isw);

	if (document.HasParseError()) {
		std::cerr << "Failed to parse .json file: " << fileName << std::endl;
	}

	std::cout << "Before loading " << ECS::ecs().GetEntityCount() << std::endl;

	for (rapidjson::SizeType i = 0; i < document.Size(); ++i) {
		const rapidjson::Value& entityObject = document[i];

		Entity entity = ECS::ecs().CreateEntity();

		if (entityObject.HasMember("Entity Name")) {
			std::string entityName = entityObject["Entity Name"].GetString();
			EntityFactory::entityFactory().masterEntitiesList[entityName] = entity;
		}

		if (entityObject.HasMember("Color")) {
			std::cout << ((ECS::ecs().isComponentTypeRegistered<Color>()) ? "Color is registered" : "Color is not registered") << std::endl;
			const rapidjson::Value& colorObject = entityObject["Color"];
			Color color{};
			color.color.r = colorObject["r"].GetFloat();
			color.color.g = colorObject["g"].GetFloat();
			color.color.b = colorObject["b"].GetFloat();
			color.color.a = colorObject["a"].GetFloat();
			ECS::ecs().AddComponent<Color>(entity, color);
		}

		if (entityObject.HasMember("Transform")) {
			std::cout << ((ECS::ecs().isComponentTypeRegistered<Transform>()) ? "Transform is registered" : "Transform is not registered") << std::endl;
			const rapidjson::Value& transformObject = entityObject["Transform"];
			Transform transform;
			transform.position.x = transformObject["position_x"].GetFloat();
			transform.position.y = transformObject["position_y"].GetFloat();
			transform.rotation = transformObject["rotation"].GetFloat();
			transform.scale = transformObject["scale"].GetFloat();
			transform.velocity.x = transformObject["velocity_x"].GetFloat();
			transform.velocity.y = transformObject["velocity_y"].GetFloat();
			ECS::ecs().AddComponent<Transform>(entity, transform);
		}

		if (entityObject.HasMember("Texture")) {
			std::cout << ((ECS::ecs().isComponentTypeRegistered<Tex>()) ? "Tex is registered" : "Tex is not registered") << std::endl;
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

			ECS::ecs().AddComponent<Tex>(entity, tex);
		}

		if (entityObject.HasMember("Visible")) {
			std::cout << ((ECS::ecs().isComponentTypeRegistered<Visible>()) ? "Visible is registered" : "Visible is not registered") << std::endl;
			const rapidjson::Value& visibleObject = entityObject["Visible"];
			Visible visible{};
			visible.isVisible = visibleObject["isVisible"].GetBool();
			ECS::ecs().AddComponent<Visible>(entity, visible);
		}

		if (entityObject.HasMember("Size")) {
			std::cout << ((ECS::ecs().isComponentTypeRegistered<Size>()) ? "Size is registered" : "Size is not registered") << std::endl;
			const rapidjson::Value& sizeObject = entityObject["Size"];
			Size size{};
			size.width = sizeObject["width"].GetFloat();
			size.height = sizeObject["height"].GetFloat();
			ECS::ecs().AddComponent<Size>(entity, size);
		}

		if (entityObject.HasMember("Circle")) {
			std::cout << ((ECS::ecs().isComponentTypeRegistered<Circle>()) ? "Circle is registered" : "Circle is not registered") << std::endl;
			const rapidjson::Value& circleObject = entityObject["Circle"];
			Circle circle{};
			circle.radius = circleObject["radius"].GetFloat();
			ECS::ecs().AddComponent<Circle>(entity, circle);
		}

		if (entityObject.HasMember("Collision")) {
			std::cout << ((ECS::ecs().isComponentTypeRegistered<AABB>()) ? "Collision is registered" : "Collision is not registered") << std::endl;
			const rapidjson::Value& aabbObject = entityObject["Collision"];
			AABB aabb;
			aabb.min.x = aabbObject["Min X"].GetFloat();
			aabb.min.y = aabbObject["Min Y"].GetFloat();
			aabb.max.x = aabbObject["Max X"].GetFloat();
			aabb.max.y = aabbObject["Max Y"].GetFloat();
			aabb.extents.x = aabbObject["Extent X"].GetFloat();
			aabb.extents.y = aabbObject["Extent Y"].GetFloat();
			ECS::ecs().AddComponent<AABB>(entity, aabb);
		}

		if (entityObject.HasMember("Animator")) {
			std::cout << ((ECS::ecs().isComponentTypeRegistered<Animator>()) ? "Anim is registered" : "Anim is not registered") << std::endl;
			const rapidjson::Value& animObject = entityObject["Animator"];
			Animator anim{ 
				static_cast<Animator::ANIMATION_TYPE>(animObject["Animation Type"].GetInt()), 
				animObject["Frame Display Duration"].GetFloat() 
			};
			//anim.animationType = static_cast<Animator::ANIMATION_TYPE>(animObject["Animation Type"].GetInt());
			//anim.frameIndex = animObject["Frame Index"].GetUint();
			//anim.frameTimeElapsed = animObject["Frame Time Elapsed"].GetFloat();
			//anim.frameDisplayDuration = animObject["Frame Display Duration"].GetFloat();
			ECS::ecs().AddComponent<Animator>(entity, anim);
		}

		ECS::ecs().AddComponent(entity, Model{});
		ECS::ecs().AddComponent(entity, MainCharacter{});
	}

	std::cout << "All loaded " << ECS::ecs().GetEntityCount() << std::endl;

	return true;
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


std::vector<std::string> OpenFileDialog() {

	std::vector<std::string> filesList;

	// Initialize COM
	CoInitialize(NULL);

	// Create the File Open Dialog
	IFileOpenDialog* p_fod = NULL; // Pointer to FileOpenDialog
	HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&p_fod));

	if (SUCCEEDED(hr)) {

		// Set the options for multiple file selection
		DWORD dwOptions;
		hr = p_fod->GetOptions(&dwOptions);
		if (SUCCEEDED(hr)) {
			hr = p_fod->SetOptions(dwOptions | FOS_ALLOWMULTISELECT);
		}

		// Show the dialog
		hr = p_fod->Show(NULL);

		// Get the chosen files if the user didn't cancel
		if (SUCCEEDED(hr)) {
			IShellItemArray* p_Results = NULL; // Pointer to Results. This is a pointer to an IShellItemArray, which is an array of shell items. Each shell item represents a file that was selected.
			hr = p_fod->GetResults(&p_Results);
			if (SUCCEEDED(hr)) {
				DWORD count = 0;
				p_Results->GetCount(&count);
				for (DWORD i = 0; i < count; i++) {
					IShellItem* p_si; // Pointer to ShellItem. It points to a single IShellItem object, which represents a single selected file.
					hr = p_Results->GetItemAt(i, &p_si);
					if (SUCCEEDED(hr)) {
						PWSTR p_szPath; // Pointer to Zero-terminated String, aka wchar_t*, because this project is in Unicode.
						hr = p_si->GetDisplayName(SIGDN_FILESYSPATH, &p_szPath);

						if (SUCCEEDED(hr)) {
							// pszPath contains the full path to one of the chosen files

							// Convert PWSTR (wide string) to std::string
							int stringSize = WideCharToMultiByte(CP_UTF8, 0, p_szPath, -1, NULL, 0, NULL, NULL);
							std::string convertedPath(stringSize, 0);
							WideCharToMultiByte(CP_UTF8, 0, p_szPath, -1, &convertedPath[0], stringSize, NULL, NULL);

							// Remove the extra null terminator from the string
							convertedPath.pop_back();

							filesList.emplace_back(convertedPath);
							CoTaskMemFree(p_szPath);
						}
						p_si->Release();
					}
				}
				p_Results->Release();
			}
		}
		p_fod->Release();
	}

	// Cleanup COM
	CoUninitialize();

	return filesList;
}

