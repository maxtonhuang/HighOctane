#include "ImGuiAssetLibrary.h"
#include "Serialization.h"
#include <Windows.h>
#include <shobjidl.h>

void UpdateAssetLibrary() {

	ImGui::Begin("Asset Library");

	if (ImGui::Button("Import Asset", { ImGui::GetContentRegionAvail().x, 50})) {


		std::vector<std::string> filesToOpen;
		/*
		OPENFILENAME ofn;
		wchar_t szFiles[4096] = { 0 };
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lpstrFile = szFiles;
		ofn.nMaxFile = sizeof(szFiles);
		
		InitializeOpenFileName(ofn, szFiles, sizeof(szFiles));
		if (GetOpenFileName(&ofn)) {
			if (ofn.Flags & OFN_ALLOWMULTISELECT) {
				wchar_t* ptr = ofn.lpstrFile;

				// convert Unicode to UTF-8
				int stringSize = WideCharToMultiByte(CP_UTF8, 0, ptr, -1, NULL, 0, NULL, NULL);
				std::string directory(stringSize, 0);
				WideCharToMultiByte(CP_UTF8, 0, ptr, -1, &directory[0], stringSize, NULL, NULL);

				ptr += directory.size() + 1;  // move past directory and NULL terminator

				while (*ptr) {

					stringSize = WideCharToMultiByte(CP_UTF8, 0, ptr, -1, NULL, 0, NULL, NULL);
					std::string filename(stringSize, 0);
					WideCharToMultiByte(CP_UTF8, 0, ptr, -1, &filename[0], stringSize, NULL, NULL);

					std::string fullpath = directory + "\\" + filename;

					// Process or store fullpath
					filesToOpen.emplace_back(fullpath);
					
					ptr += filename.size() + 1;  // move past filename and NULL terminator

				}
			}
			else {
				wchar_t* ptr = ofn.lpstrFile;

				// convert Unicode to UTF-8
				int stringSize = WideCharToMultiByte(CP_UTF8, 0, ptr, -1, NULL, 0, NULL, NULL);
				std::string filepath(stringSize, 0);
				WideCharToMultiByte(CP_UTF8, 0, ptr, -1, &filepath[0], stringSize, NULL, NULL);

				// Process or store filepath
				filesToOpen.emplace_back(filepath);


			}
		}
		*/



		// Initialize COM
		CoInitialize(NULL);

		// Create the File Open Dialog
		IFileOpenDialog* pfod = NULL;
		HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfod));

		if (SUCCEEDED(hr)) {
			// Set the options for multiple file selection
			DWORD dwOptions;
			hr = pfod->GetOptions(&dwOptions);
			if (SUCCEEDED(hr)) {
				hr = pfod->SetOptions(dwOptions | FOS_ALLOWMULTISELECT);
			}

			// Show the dialog
			hr = pfod->Show(NULL);

			// Get the chosen files if the user didn't cancel
			if (SUCCEEDED(hr)) {
				IShellItemArray* pResults = NULL;
				hr = pfod->GetResults(&pResults);
				if (SUCCEEDED(hr)) {
					DWORD count = 0;
					pResults->GetCount(&count);
					for (DWORD i = 0; i < count; i++) {
						IShellItem* psi;
						hr = pResults->GetItemAt(i, &psi);
						if (SUCCEEDED(hr)) {
							PWSTR pszPath;
							hr = psi->GetDisplayName(SIGDN_FILESYSPATH, &pszPath);

							if (SUCCEEDED(hr)) {
								// pszPath contains the full path to one of the chosen files

								// Convert PWSTR (wide string) to std::string
								int stringSize = WideCharToMultiByte(CP_UTF8, 0, pszPath, -1, NULL, 0, NULL, NULL);
								std::string convertedPath(stringSize, 0);
								WideCharToMultiByte(CP_UTF8, 0, pszPath, -1, &convertedPath[0], stringSize, NULL, NULL);

								// Remove the extra null terminator from the string
								convertedPath.pop_back();

								filesToOpen.emplace_back(convertedPath);
								CoTaskMemFree(pszPath);
							}
							psi->Release();
						}
					}
					pResults->Release();
				}
			}
			pfod->Release();
		}

		// Cleanup COM
		CoUninitialize();

		for (std::string val : filesToOpen) {
			std::cout << val << std::endl;
		}

	}
	

	ImGui::End();

}