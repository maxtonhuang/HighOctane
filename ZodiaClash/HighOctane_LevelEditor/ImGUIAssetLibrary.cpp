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
*	@file		ImGuiAssetLibrary.cpp
*
*	@author		Maxton Huang Xinghua
*
*	@email		m.huang\@digipen.edu
*
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		3 November 2023
*
* *****************************************************************************
*
*	@brief		Functions to update the Asset Library panel in the editor
*
*	The Asset Library panel is used to display all the master entities that
*	are available for the user to use. The user can also create new master
*	entities from this panel by importing images. New Master Entities will
*	automatically be created from the imported images. The user will also be
*	able to choose whether the image to be imported is a static image or a
*	spritesheet.
*
******************************************************************************/

#include "ImGuiAssetLibrary.h"
#include "Serialization.h"
#include "EntityFactory.h"
#include "AssetManager.h"
#include "WindowsInterlink.h"
#include "Global.h"
#include "GraphicConstants.h"
#include "EntityFactory.h"
#include <Windows.h>
#include <shobjidl.h>
#include <filesystem>
#include "File.h"

constexpr float thumbnailSize = 128.f;
constexpr float paddingSize = 20.f;
constexpr float buttonHeight = 50.f;
constexpr float spacing = 30.f;
static Entity selectedEntity;
static Entity selectedMaster;
static bool clicked;

/******************************************************************************
*
*	@brief Creates the Asset Library panel and handles importing of images
*
*	Creates the Asset Library panel. The user can import images by clicking
*	on the "Create New Master Entity From Image" button. The user can then
*	choose the image to import. The user can only import images of the format
*	".png", ".jpg", ".jpeg", or ".bmp". The user can also choose whether the
*	image to be imported is a static image or a spritesheet. If the user 
*	cancels the import, the image will not be imported. If the user chooses
*	to import the image, the image will be copied to the Assets/Textures
*	folder and a new Master Entity will be created from the image.
*
******************************************************************************/
void UpdateAssetLibrary() {
	
	static bool showDialog = false;

	ImGui::Begin("Master Entities Library");

	float panelWidth = ImGui::GetContentRegionAvail().x;

	if (ImGui::Button("Create New Master Entity From Image", { panelWidth, buttonHeight })) {

		OpenFileDialog();
		
		if (!importFileList.empty()) { // user did not press Cancel button

			for (std::string val : importFileList) {

				std::string ext = val.substr(val.find_last_of('.'), val.length() - (val.find_last_of('.')));
				if (ext != ".png" && ext != ".jpg" && ext != ".jpeg" && ext != ".bmp") {

					std::cout << "Texture rendering and Entity creation only accepts \".png\", \".jpg\", \".jpeg\", or \".bmp\" file formats!" << std::endl;
					importFileList.clear();
					importFileCount = 0;

				}
			}
		}
	}

	int columnCount = std::max(1, static_cast<int>(panelWidth / (thumbnailSize + paddingSize)));
	ImGui::Columns(columnCount, NULL, false);

	for (std::pair<std::string, Entity> val : EntityFactory::entityFactory().masterEntitiesList)
	{
		Texture* tex = ECS::ecs().GetComponent<Tex>(val.second).tex;
		float imageWidth = static_cast<float>(tex->GetWidth());
		float imageHeight = static_cast<float>(tex->GetHeight());
		int rowCount = tex->GetRowCount() == 0 ? 1 : tex->GetRowCount();
		int colCount = tex->GetColCount() == 0 ? 1 : tex->GetColCount();
		ImGui::ImageButton(reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(tex->GetID())), { (imageWidth < imageHeight) ? (thumbnailSize * imageWidth / imageHeight) : thumbnailSize, (imageWidth < imageHeight) ? thumbnailSize : (thumbnailSize * imageHeight / imageWidth) }, { 0 , 0 }, { 1.f / static_cast<float>(colCount), 1.f / static_cast<float>(rowCount) });
		if (ImGui::IsItemHovered()) {
			if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
				selectedMaster = val.second;
				popupMasterRightClicked = true;
			}
			else if (ImGui::IsMouseDoubleClicked(0)) {
				selectedMaster = val.second;
				clicked = true;
			}
		}
		ImGui::TextWrapped(val.first.c_str());
		ImGui::NextColumn();
	}

	ImGui::Columns(1);
	
	if (importFileCount) {
		showDialog = true;
	}

	if (showDialog) {
		CheckImageTypeDialog(showDialog);
	}

	if (clicked) {
		EntityFactory::entityFactory().CloneMaster(selectedMaster);
		clicked = false;
	}

	if (popupMasterRightClicked) {
		if (ImGui::BeginPopupContextWindow("1")) {
			if (ImGui::MenuItem("Delete")) {
				EntityFactory::entityFactory().DeleteMasterModel(selectedMaster);
				popupMasterRightClicked = false;
			}
			ImGui::EndPopup();
		}	
	}

	ImGui::End();

}

void ImportImage(bool& showDialog) {
	ImGui::OpenPopup("Import Image");


	// Variables to hold the state and data
	static bool isStaticImageSelected = true;
	static bool isSpritesheetSelected = false;
	static char rowsInput[7] = "";
	static char colsInput[7] = "";
	static bool allConditionsMet = false;

	// Create a centered popup
	//ImGui::SetNextWindowContentSize(ImVec2(400, 0));
	//bool modalOpen = true;
	if (ImGui::BeginPopupModal("Import Image", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		std::string destinationFilePath = importFileList[0];
		destinationFilePath = "..\\Assets\\Textures\\" + destinationFilePath.substr(destinationFilePath.find_last_of('\\') + 1, destinationFilePath.size());
		int counter = 1;
		while (std::filesystem::exists(destinationFilePath)) {
			if (counter == 1) {
				std::string tempStr = " (" + std::to_string(counter++) + ")";
				destinationFilePath.insert(destinationFilePath.find_last_of('.'), tempStr);
			}
			else {
				destinationFilePath.replace(destinationFilePath.find_last_of('.') - 2, 1, std::to_string(counter++));
			}
		}
		counter = 1;
		std::string destinationFilename = destinationFilePath.substr(destinationFilePath.find_last_of('\\') + 1, destinationFilePath.size() - destinationFilePath.find_last_of('\\') + 1);

		ImGui::Dummy(ImVec2(0.0f, spacing));

		ImGui::Dummy(ImVec2(spacing, 0.f)); // Adjusting the height
		ImGui::SameLine();
		ImGui::Text(destinationFilename.c_str());

		// Line 1: Display the main question
		ImGui::Dummy(ImVec2(spacing, buttonHeight)); // Adjusting the height
		ImGui::SameLine();
		ImGui::Text("What type of image are you importing?");
		//ImGui::Spacing();

		// Line 2 & 3: Radio buttons for type selection
		// Line 2 & 3: Radio buttons for type selection
		ImGui::Dummy(ImVec2(spacing, buttonHeight)); // Adjusting the height
		ImGui::SameLine();
		if (ImGui::RadioButton("Static Image", isStaticImageSelected))
		{
			isStaticImageSelected = true;
			isSpritesheetSelected = false;
		}

		ImGui::Dummy(ImVec2(spacing, buttonHeight)); // Adjusting the height
		ImGui::SameLine();
		if (ImGui::RadioButton("Spritesheet", isSpritesheetSelected))
		{
			isSpritesheetSelected = true;
			isStaticImageSelected = false;
		}

		int enteredRows = 0, enteredCols = 0;

		if (isSpritesheetSelected)
		{
			ImGui::SameLine();
			ImGui::Dummy(ImVec2(spacing, 0.0f)); // Inserting the gap
			ImGui::SameLine();
			// Line 4: Enter rows and columns for spritesheet
			ImGui::Text("Rows");
			ImGui::SameLine();
			ImGui::PushItemWidth(buttonHeight * 2.f);
			ImGui::InputText("##RowsInput", rowsInput, sizeof(rowsInput));
			ImGui::PopItemWidth();
			ImGui::SameLine();
			ImGui::Dummy(ImVec2(spacing, 0.0f)); // Inserting the gap
			ImGui::SameLine();
			ImGui::Text("Columns");
			ImGui::SameLine();
			ImGui::PushItemWidth(buttonHeight * 2.f);
			ImGui::InputText("##ColsInput", colsInput, sizeof(colsInput));
			ImGui::PopItemWidth();
			enteredRows = atoi(rowsInput);
			enteredCols = atoi(colsInput);
		}

		ImGui::Dummy(ImVec2(spacing, buttonHeight)); // Adjusting the height
		ImGui::SameLine();
		// Line 5: OK and Cancel buttons
		if (isStaticImageSelected || (isSpritesheetSelected && enteredRows > 0 && enteredCols > 0))
		{
			if (ImGui::Button("OK", { spacing * 3.f, buttonHeight }))
			{
		
				std::filesystem::path srcPath = importFileList[0];

				std::filesystem::path destPath = destinationFilePath;

				std::cout << "Source path is: " << srcPath << std::endl;
				std::cout << "Destination path is: " << destPath << std::endl;

				// check if the source file exists
				if (!std::filesystem::exists(importFileList[0])) {
					std::cout << "Source file does not exist: " << importFileList[0] << std::endl;
				}

				// check if destination exists
				else if (!std::filesystem::exists(destPath.parent_path())) {
					std::cout << "Destination directory does not exist: " << destPath << std::endl;
				}

				else {
					std::filesystem::copy(srcPath, destPath);
				}

				if (isStaticImageSelected) {

					EntityFactory::entityFactory().CreateMasterModel(destinationFilename.c_str());

				}
				else {

					EntityFactory::entityFactory().CreateMasterModel(destinationFilename.c_str(), enteredRows, enteredCols);

				}

				rowsInput[0] = '\0';
				colsInput[0] = '\0';
				isStaticImageSelected = true;
				isSpritesheetSelected = false;

				importFileList.pop_front();
				--importFileCount;

				showDialog = false;
				ImGui::CloseCurrentPopup();
			}
		}
		else
		{
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			ImGui::Button("OK", { spacing * 3.f, buttonHeight });
			ImGui::PopStyleVar();
		}

		ImGui::SameLine();
		if (ImGui::Button("Cancel", { spacing * 3.f, buttonHeight }))
		{
			// Clear data if needed
			if (isSpritesheetSelected && !isStaticImageSelected)
			{
				rowsInput[0] = '\0';
				colsInput[0] = '\0';
			}
			importFileList.clear();
			importFileCount = 0;
			rowsInput[0] = '\0';
			colsInput[0] = '\0';
			isStaticImageSelected = true;
			isSpritesheetSelected = false;
			showDialog = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void ImportAudio(bool& showDialog) {
	ImGui::OpenPopup("Import Audio");


	// Variables to hold the state and data
	static bool isSound = true;

	// Create a centered popup
	//ImGui::SetNextWindowContentSize(ImVec2(400, 0));
	//bool modalOpen = true;
	if (ImGui::BeginPopupModal("Import Audio", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		std::string destinationFilename = importFileList[0].substr(importFileList[0].find_last_of('\\') + 1, importFileList[0].size() - importFileList[0].find_last_of('\\') + 1);

		ImGui::Dummy(ImVec2(0.0f, spacing));

		ImGui::Dummy(ImVec2(spacing, 0.f)); // Adjusting the height
		ImGui::SameLine();
		ImGui::Text(destinationFilename.c_str());

		// Line 1: Display the main question
		ImGui::Dummy(ImVec2(spacing, buttonHeight)); // Adjusting the height
		ImGui::SameLine();
		ImGui::Text("Are you importing sound or music?");
		//ImGui::Spacing();

		// Line 2 & 3: Radio buttons for type selection
		ImGui::Dummy(ImVec2(spacing, buttonHeight)); // Adjusting the height
		ImGui::SameLine();
		if (ImGui::RadioButton("Sound", isSound))
		{
			isSound = true;
		}

		ImGui::Dummy(ImVec2(spacing, buttonHeight)); // Adjusting the height
		ImGui::SameLine();
		if (ImGui::RadioButton("Music", !isSound))
		{
			isSound = false;
		}

		ImGui::Dummy(ImVec2(spacing, buttonHeight)); // Adjusting the height
		ImGui::SameLine();

		if (ImGui::Button("OK", { spacing * 3.f, buttonHeight }))
		{
			std::string destinationFilePath{ assetmanager.GetDefaultPath() };
			if (isSound) {
				destinationFilePath += "Sound/";
			}
			else {
				destinationFilePath += "Music/";
			}
			destinationFilePath += importFileList[0].substr(importFileList[0].find_last_of('\\') + 1, importFileList[0].size());

			std::filesystem::path srcPath = importFileList[0];

			std::filesystem::path destPath = destinationFilePath;

			std::cout << "Source path is: " << srcPath << std::endl;
			std::cout << "Destination path is: " << destPath << std::endl;

			if (!std::filesystem::exists(destPath)) {
				// check if the source file exists
				if (!std::filesystem::exists(importFileList[0])) {
					std::cout << "Source file does not exist: " << importFileList[0] << std::endl;
				}

				// check if destination exists
				else if (!std::filesystem::exists(destPath.parent_path())) {
					std::cout << "Destination directory does not exist: " << destPath << std::endl;
				}
				std::filesystem::copy(srcPath, destPath);
			}
			else {
				std::cout << "File already exists in destination directory!" << std::endl;
			}

			isSound = true;

			importFileList.pop_front();
			--importFileCount;

			showDialog = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();
		if (ImGui::Button("Cancel", { spacing * 3.f, buttonHeight }))
		{
			// Clear data
			importFileList.clear();
			importFileCount = 0;
			isSound = true;
			showDialog = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void ImportErrorPopup(bool& showDialog) {
	ImGui::OpenPopup("Import Error");

	if (ImGui::BeginPopupModal("Import Error", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		std::string destinationFilename = importFileList[0].substr(importFileList[0].find_last_of('\\') + 1, importFileList[0].size() - importFileList[0].find_last_of('\\') + 1);

		ImGui::Dummy(ImVec2(spacing, 0.f)); // Adjusting the height
		ImGui::SameLine();
		ImGui::Text("File type is not supported!");

		ImGui::Dummy(ImVec2(spacing, buttonHeight)); // Adjusting the height
		ImGui::SameLine();
		ImGui::Text(destinationFilename.c_str());

		ImGui::Dummy(ImVec2(spacing, 0.f)); // Adjusting the height
		ImGui::SameLine();
		ImGui::Text("Valid file types:");

		ImGui::Dummy(ImVec2(spacing, 0.f)); // Adjusting the height
		ImGui::SameLine();
		ImGui::Text("Textures:");
		ImGui::Dummy(ImVec2(spacing, 0.f)); // Adjusting the height
		ImGui::SameLine();
		ImGui::Text(".png / .jpg / .jpeg files");

		ImGui::Dummy(ImVec2(spacing, 0.f)); // Adjusting the height
		ImGui::SameLine();
		ImGui::Text("Audio:");
		ImGui::Dummy(ImVec2(spacing, 0.f)); // Adjusting the height
		ImGui::SameLine();
		ImGui::Text(".wav / .ogg files");

		ImGui::Dummy(ImVec2(spacing, 0.f)); // Adjusting the height
		ImGui::SameLine();
		if (ImGui::Button("OK", { spacing * 5.f, buttonHeight })) {
			importFileList.pop_front();
			--importFileCount;
			showDialog = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

/******************************************************************************
*
*	@brief Creates the popup dialog for importing images
*
*	Creates the popup dialog for the user to choose the type of image to be
*	imported. The user can choose between a static image or a spritesheet.
*	If the user chooses to import a spritesheet, the user will have to enter
*	the number of rows and columns in the spritesheet.
*
******************************************************************************/
void CheckImageTypeDialog(bool & showDialog) {
	ImGui::SetNextWindowSize(ImVec2(GRAPHICS::defaultWidthF / 3.f, GRAPHICS::defaultHeightF / 3.f));
	ImGui::SetNextWindowPos(ImVec2(GRAPHICS::defaultWidthF / 3.f, GRAPHICS::defaultHeightF / 3.f));
	
	std::string extension = FilePath::GetFileExtension(importFileList[0]);

	if (extension == ".png" || extension == ".jpg" || extension == ".jpeg" || extension == ".bmp") {
		ImportImage(showDialog);
	}
	else if (extension == ".wav" || extension == ".ogg") {
		ImportAudio(showDialog);
	}
	else {
		ImportErrorPopup(showDialog);
	}
}
