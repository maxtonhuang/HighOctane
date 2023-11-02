#include "ImGuiIcons.h"
#include "AssetManager.h"

std::map<std::string, ImTextureID> loadedIcons;
ImTextureID LoadIconTexture(const std::string& iconPath) {
	// Create an instance of Texture
	Texture iconTexture;

	// Define a path to your icons directory in the AssetManager
	std::string pathToIcons = assetmanager.GetDefaultPath() + "Textures/Icons/";

	// Construct the full path to the icon within the icons directory
	std::string fullPath = pathToIcons + iconPath;

	// Initialize the icon texture
	iconTexture.Init(fullPath.c_str(), iconPath.c_str());

	// Check if the icon texture was loaded successfully
	if (iconTexture.IsActive()) {
		// Return the ImTextureID from the loaded texture
		return reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(iconTexture.GetID()));
	}

	else {
		// Return nullptr if loading failed (you may want to handle errors differently)
		return nullptr;
	}
}

void LoadIcons() {
	loadedIcons["folderIcon"] = LoadIconTexture("foldericon.png");
	loadedIcons["fileIcon"] = LoadIconTexture("fileicon.png");
	loadedIcons["backIcon"] = LoadIconTexture("backicon.png");
	loadedIcons["lockIcon"] = LoadIconTexture("lockicon.png");
	loadedIcons["viewIcon"] = LoadIconTexture("viewicon.png");
}

void UnloadIcons() {
	for (const auto& iconPair : loadedIcons) {
		ImTextureID imTextureID = iconPair.second;
		Texture* iconTexture = reinterpret_cast<Texture*>(imTextureID);

		/*if (iconTexture) {
			GLuint textureID = imTextureID->GetID();
			glDeleteTextures(1, &textureID);
		}*/
		glDeleteTextures(1, reinterpret_cast<GLuint*>(&imTextureID));
	}

	loadedIcons.clear();
}

