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

void UpdateAssetLibrary() {

	ImGui::Begin("Asset Library");

	float panelWidth = ImGui::GetContentRegionAvail().x;

	if (ImGui::Button("Import From Image", { panelWidth, buttonHeight })) {

		std::vector<std::string> filesToOpen = OpenFileDialog();
		
		if (!filesToOpen.empty()) { // user did not press Cancel button

			// Create new Master Entity Here using Path! <<<---------------------------------


			for (std::string val : filesToOpen) {

				if (val.substr(val.find_last_of('.'), val.length() - (val.find_last_of('.'))) == ".png") {

					std::filesystem::path srcPath = val;

					std::filesystem::path destPath = "..\\Assets\\Textures";

					destPath /= srcPath.filename(); // append the filename to the destination directory

					std::cout << "Source path is: " << srcPath << std::endl;
					std::cout << "Destination path is: " << destPath << std::endl;
					
					// check if the source file exists
					if (!std::filesystem::exists(val)) {
						std::cout << "Source file does not exist: " << val << std::endl;
					}

					// check if destination exists
					else if (!std::filesystem::exists(destPath.parent_path())) {
						std::cout << "Destination directory does not exist: " << destPath << std::endl;
					}
					
					else {
						std::filesystem::copy(srcPath, destPath);
						EntityFactory::entityFactory().CreateMasterModel(srcPath.filename().string().c_str());
					}
				}
				else {
					std::cout << "Texture rendering and Entity creation only accepts .png files!" << std::endl;
				}
			}
		}
	}

	int columnCount = std::max(1, static_cast<int>(panelWidth / (thumbnailSize + paddingSize)));
	ImGui::Columns(columnCount, NULL, false);

	for (std::pair<std::string, Entity> val : EntityFactory::entityFactory().masterEntitiesList)
	{
		Texture* tex = ECS::ecs().GetComponent<Tex>(val.second).tex;
		float imageWidth = tex->GetWidth();
		float imageHeight = tex->GetHeight();
		int rowCount = tex->GetRowCount() == 0 ? 1 : tex->GetRowCount();
		int colCount = tex->GetColCount() == 0 ? 1 : tex->GetColCount();
		ImGui::ImageButton((ImTextureID)(tex->GetID()), { (imageWidth < imageHeight) ? (thumbnailSize * imageWidth / imageHeight) : thumbnailSize, (imageWidth < imageHeight) ? thumbnailSize : (thumbnailSize * imageHeight / imageWidth) }, { 0 , 0 }, { 1.f / colCount, 1.f / rowCount });
		ImGui::TextWrapped(val.first.c_str());
		ImGui::NextColumn();
	}

	ImGui::Columns(1);
	



	if (dragAndDropped) {

		for (const std::string& val : dragAndDropFilesList) {
			std::cout << val << std::endl;
		}




	static bool showDialog = false;

	if (ImGui::Button("Show Dialog")) {
		showDialog = true;
	}

	if (showDialog) {
		ImGui::SetNextWindowSize(ImVec2(GRAPHICS::defaultWidthF / 3.f, GRAPHICS::defaultHeightF / 3.f));
		ImGui::SetNextWindowPos(ImVec2(GRAPHICS::defaultWidthF / 3.f, GRAPHICS::defaultHeightF / 3.f));
		ImGui::OpenPopup("Import Image");


		// Variables to hold the state and data
		static bool isStaticImageSelected = false;
		static bool isSpritesheetSelected = false;
		static char rowsInput[7] = "";
		static char colsInput[7] = "";
		static bool allConditionsMet = false;

		// Create a centered popup
		//ImGui::SetNextWindowContentSize(ImVec2(400, 0));
		//bool modalOpen = true;
		if (ImGui::BeginPopupModal("Import Image", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			// Line 1: Display the main question
			ImGui::Dummy(ImVec2(0.0f, 50.0f)); // Adjusting the height
			ImGui::SameLine();
			ImGui::Text("What type of image are you importing?");
			ImGui::Spacing();

			// Line 2 & 3: Radio buttons for type selection
			// Line 2 & 3: Radio buttons for type selection
			ImGui::Dummy(ImVec2(0.0f, 50.0f)); // Adjusting the height
			ImGui::SameLine();
			if (ImGui::RadioButton("Static Image", isStaticImageSelected))
			{
				isStaticImageSelected = true;
				isSpritesheetSelected = false;
			}

			ImGui::Dummy(ImVec2(0.0f, 50.0f)); // Adjusting the height
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

			// Check conditions for enabling "OK" button
			allConditionsMet = (isStaticImageSelected || (isSpritesheetSelected && enteredRows > 0 && enteredCols > 0));

			ImGui::Dummy(ImVec2(0.0f, 50.0f)); // Adjusting the height
			ImGui::SameLine();
			// Line 5: OK and Cancel buttons
			if (allConditionsMet)
			{
				if (ImGui::Button("OK", {30.f * 3.f, 50.f}))
				{
					// Handle OK action
					ImGui::CloseCurrentPopup();
					showDialog = false;
				}
			}
			else
			{
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				ImGui::Button("OK", {30.f*3.f, 50.f});
				ImGui::PopStyleVar();
			}

			ImGui::SameLine();
			if (ImGui::Button("Cancel", {30.f*3.f, 50.f}))
			{
				// Clear data if needed
				if (isSpritesheetSelected && !isStaticImageSelected)
				{
					rowsInput[0] = '\0';
					colsInput[0] = '\0';
				}
				ImGui::CloseCurrentPopup();
				showDialog = false;
			}
			ImGui::EndPopup();
		}
	}

		dragAndDropped = false;


	}





	ImGui::End();



}



