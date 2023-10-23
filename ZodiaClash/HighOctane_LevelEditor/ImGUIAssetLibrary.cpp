#include "ImGuiAssetLibrary.h"
#include "Serialization.h"
#include "EntityFactory.h"
#include "AssetManager.h"
#include <Windows.h>
#include <shobjidl.h>

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
				EntityFactory::entityFactory().CreateMasterModel(val.c_str());//.substr(val.find_last_of('\\') + 1, val.length() - (val.find_last_of('\\') + 1)).c_str());
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
	
	ImGui::End();

}