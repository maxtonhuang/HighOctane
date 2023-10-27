#include "ImGuiAssetLibrary.h"
#include "Serialization.h"
#include "EntityFactory.h"
#include "AssetManager.h"
#include "WindowsInterlink.h"
#include "Global.h"
#include "GraphicConstants.h"
#include <Windows.h>
#include <shobjidl.h>
#include <filesystem>

constexpr float thumbnailSize = 128.f;
constexpr float paddingSize = 20.f;
constexpr float buttonHeight = 50.f;
static Entity selectedEntity;
static bool clicked;
void UpdateAssetLibrary() {
	
	static bool showDialog = false;

	ImGui::Begin("Asset Library");

	float panelWidth = ImGui::GetContentRegionAvail().x;

	if (ImGui::Button("Import From Image", { panelWidth, buttonHeight })) {

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
		if (ImGui::IsItemClicked(0)) {
			std::cout << "Clicked" << std::endl;
			selectedEntity = val.second;
			clicked = true;
		}
		ImGui::TextWrapped(val.first.c_str());
		ImGui::NextColumn();
	}

	ImGui::Columns(1);
	
	ImGui::End();
	
	
	if (importFileCount) {
		showDialog = true;
	}

	if (showDialog) {
		CheckImageTypeDialog(showDialog);
	}

	if (clicked) {
		EntityFactory::entityFactory().CloneMaster(selectedEntity);
		clicked = false;
	}


}




void CheckImageTypeDialog(bool & showDialog) {


	ImGui::SetNextWindowSize(ImVec2(GRAPHICS::defaultWidthF / 3.f, GRAPHICS::defaultHeightF / 3.f));
	ImGui::SetNextWindowPos(ImVec2(GRAPHICS::defaultWidthF / 3.f, GRAPHICS::defaultHeightF / 3.f));
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
					
		ImGui::Dummy(ImVec2(0.0f, 30.0f));

		ImGui::Dummy(ImVec2(30.0f, 0.f)); // Adjusting the height
		ImGui::SameLine();
		ImGui::Text(destinationFilename.c_str());

		// Line 1: Display the main question
		ImGui::Dummy(ImVec2(30.0f, 50.f)); // Adjusting the height
		ImGui::SameLine();
		ImGui::Text("What type of image are you importing?");
		//ImGui::Spacing();

		// Line 2 & 3: Radio buttons for type selection
		// Line 2 & 3: Radio buttons for type selection
		ImGui::Dummy(ImVec2(30.0f, 50.0f)); // Adjusting the height
		ImGui::SameLine();
		if (ImGui::RadioButton("Static Image", isStaticImageSelected))
		{
			isStaticImageSelected = true;
			isSpritesheetSelected = false;
		}

		ImGui::Dummy(ImVec2(30.0f, 50.0f)); // Adjusting the height
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
			ImGui::Dummy(ImVec2(30.f, 0.0f)); // Inserting the gap
			ImGui::SameLine();
			// Line 4: Enter rows and columns for spritesheet
			ImGui::Text("Rows");
			ImGui::SameLine();
			ImGui::PushItemWidth(50.f * 2.f);
			ImGui::InputText("##RowsInput", rowsInput, sizeof(rowsInput));
			ImGui::PopItemWidth();
			ImGui::SameLine();
			ImGui::Dummy(ImVec2(30.f, 0.0f)); // Inserting the gap
			ImGui::SameLine();
			ImGui::Text("Columns");
			ImGui::SameLine();
			ImGui::PushItemWidth(50.f * 2.f);
			ImGui::InputText("##ColsInput", colsInput, sizeof(colsInput));
			ImGui::PopItemWidth();
			enteredRows = atoi(rowsInput);
			enteredCols = atoi(colsInput);
		}

		ImGui::Dummy(ImVec2(30.0f, 50.0f)); // Adjusting the height
		ImGui::SameLine();
		// Line 5: OK and Cancel buttons
		if (isStaticImageSelected || (isSpritesheetSelected && enteredRows > 0 && enteredCols > 0))
		{
			if (ImGui::Button("OK", { 30.f * 3.f, 50.f }))
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
			ImGui::Button("OK", { 30.f * 3.f, 50.f });
			ImGui::PopStyleVar();
		}

		ImGui::SameLine();
		if (ImGui::Button("Cancel", { 30.f * 3.f, 50.f }))
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