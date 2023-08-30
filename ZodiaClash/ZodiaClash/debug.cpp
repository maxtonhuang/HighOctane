/*
\copyright
		All content(C) 2023 DigiPen Institute of Technology Singapore.All rights
		reserved.Reproduction or disclosure of this file or its contents without the prior
		written consent of DigiPen Institute of Technology is prohibited.
*/
/*!
@file		debug.cpp
@author		Koh Wen Yuan
@Email		k.wenyuan@digipen.edu
@course		CSD 2401
@section	Section A
@date		30 August 2023
@brief		This file contains the functions declaration for debugging
*//*______________________________________________________________________*/

#include "debug.h"
#include <iostream>
#include <string>
#include <fstream>

namespace debug {
	void writeFile(void) {
		// Create and open a text file
		std::ofstream MyFile("testDebug.txt");

		// Write to the file
		MyFile << "This is a test";

		// Close the file
		MyFile.close();
	}

	void writeFile(float num) {
		// Create and open a text file
		std::ofstream MyFile("testDebug.txt");

		// Write to the file
		MyFile << num << '\n';

		// Close the file
		MyFile.close();
	}

	void writeFile(vmath::Vector2 vec) {
		// Create and open a text file
		std::ofstream MyFile("testDebug.txt");

		// Write to the file
		MyFile << vec << '\n';

		// Close the file
		MyFile.close();
	}
}

