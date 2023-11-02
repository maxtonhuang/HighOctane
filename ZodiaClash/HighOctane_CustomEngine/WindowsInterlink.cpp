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
*	@file		WindowsInterlink.cpp
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
*	@brief		Functions to handle intercommunication with Windows
*
*	This file contains functions to enable Windows file dialogs and file drag
*   and drop from Windows File Explorer into the appication window.
*
******************************************************************************/


#include "WindowsInterlink.h"
#include "Global.h"
#include <iostream>
#include <shobjidl.h>


/******************************************************************************
*
*	@brief Creates the Open File Dialog to load multiple files
*
*	Handles the windows communications to create the Open File Dialog and
*	retrieve the file paths of the selected file(s). Accepts multiple file
*	selection.
*
******************************************************************************/
void OpenFileDialog() {

	std::deque<std::string> filesList;

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
						hr = p_si->GetDisplayName(SIGDN_FILESYSPATH, &p_szPath); // use "SIGDN_FILESYSPATH" for full file path. use "SIGDN_NORMALDISPLAY" for filename.

						if (SUCCEEDED(hr)) {
							// pszPath contains the full path to one of the chosen files

							// Convert PWSTR (wide string) to std::string
							int stringSize = WideCharToMultiByte(CP_UTF8, 0, p_szPath, -1, NULL, 0, NULL, NULL);
							std::string convertedPath(stringSize, 0);
							WideCharToMultiByte(CP_UTF8, 0, p_szPath, -1, &convertedPath[0], stringSize, NULL, NULL);

							// Remove the extra null terminator from the string
							convertedPath.pop_back();

							importFileList.emplace_back(convertedPath);
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

	importFileCount = importFileList.size();
}

/******************************************************************************
*
*	@brief Sets the callback for OpenGL window to detect file drag and drop
*
*	Enables the OpenGL window to detect file drag and drop from Windows File
*	Explorer into the application window.
*
******************************************************************************/
void FileDropCallback(GLFWwindow* window, int count, const char** paths) {

	UNREFERENCED_PARAMETER(window);

	fileDropped = true;
	dropTimer = 0.08f;	

	for (int i = 0; i < count; i++)	{
		// Handle each path: load the file, etc.
		std::cout << paths[i] << std::endl;

		std::string tempPath(paths[i]);

		importFileList.emplace_back(tempPath);

		importFileCount = count;

	}
}

/******************************************************************************
*
*	@brief Creates the Save File Dialog to save a file into static storage
*
*	Handles the windows communications to create the Save File Dialog and
*	save a file into a chosen folder.
* 
******************************************************************************/
std::string SaveFileDialog(std::string extensionName, std::string extensionDescription) {
	// Initialize COM
	CoInitialize(NULL);

	// Create the File Save Dialog
	IFileSaveDialog* p_fsd = NULL; // Pointer to FileSaveDialog
	HRESULT hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&p_fsd));

	if (SUCCEEDED(hr)) {
		// Show the dialog
		if (extensionName != "") {
			std::wstring edtemp = std::wstring(extensionDescription.begin(), extensionDescription.end());
			LPCWSTR eDescription = edtemp.c_str();
			std::wstring entemp = std::wstring(extensionName.begin(), extensionName.end());
			LPCWSTR eName = entemp.c_str();
			COMDLG_FILTERSPEC extension{ eDescription, eName};
			p_fsd->SetFileTypes(1, &extension);
			p_fsd->SetFileTypeIndex(1);
			p_fsd->SetDefaultExtension(eName);
		}
		
		hr = p_fsd->Show(NULL);
		if (SUCCEEDED(hr)) {
			IShellItem* p_si = NULL; // Pointer to ShellItem
			hr = p_fsd->GetResult(&p_si);

			if (SUCCEEDED(hr)) {
				PWSTR p_szPath; // Pointer to Zero-terminated String
				hr = p_si->GetDisplayName(SIGDN_FILESYSPATH, &p_szPath);

				if (SUCCEEDED(hr)) {
					// p_szPath contains the full path to the selected file
					// Convert PWSTR (wide string) to std::string
					int stringSize = WideCharToMultiByte(CP_UTF8, 0, p_szPath, -1, NULL, 0, NULL, NULL);
					std::string convertedPath(stringSize, 0);
					WideCharToMultiByte(CP_UTF8, 0, p_szPath, -1, &convertedPath[0], stringSize, NULL, NULL);

					// Remove the extra null terminator from the string
					convertedPath.pop_back();

					CoTaskMemFree(p_szPath);
					p_si->Release();

					// Cleanup COM
					CoUninitialize();

					return convertedPath;
				}
			}
			p_fsd->Release();
		}
	}

	// Cleanup COM
	CoUninitialize();

	return ""; // Return an empty string if the user cancels or an error occurs
}

/******************************************************************************
*
*	@brief Creates the Open File Dialog to load a single file
*
*	Handles the windows communications to create the Open File Dialog and
*	retrieve the file path of a selected file.
*
******************************************************************************/
std::string OpenSingleFileDialog() {

	std::string selectedFilePath;

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
			IShellItemArray* p_Results = NULL; // Pointer to Results
			hr = p_fod->GetResults(&p_Results);
			if (SUCCEEDED(hr)) {
				DWORD count = 0;
				p_Results->GetCount(&count);
				if (count > 0) {
					IShellItem* p_si; // Pointer to ShellItem
					hr = p_Results->GetItemAt(0, &p_si); // Get the first selected file.
					if (SUCCEEDED(hr)) {
						PWSTR p_szPath; // Pointer to Zero-terminated String (wide string).
						hr = p_si->GetDisplayName(SIGDN_FILESYSPATH, &p_szPath);

						if (SUCCEEDED(hr)) {
							// Convert PWSTR (wide string) to std::string
							int stringSize = WideCharToMultiByte(CP_UTF8, 0, p_szPath, -1, NULL, 0, NULL, NULL);
							selectedFilePath.resize(stringSize);
							WideCharToMultiByte(CP_UTF8, 0, p_szPath, -1, &selectedFilePath[0], stringSize, NULL, NULL);

							// Remove the extra null terminator from the string
							selectedFilePath.pop_back();
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

	return selectedFilePath;
}
