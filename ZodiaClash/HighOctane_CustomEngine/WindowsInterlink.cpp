#include "WindowsInterlink.h"
#include "Global.h"
#include <iostream>
#include <shobjidl.h>


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



