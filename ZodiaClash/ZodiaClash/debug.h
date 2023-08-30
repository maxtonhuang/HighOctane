/*
\copyright
		All content(C) 2023 DigiPen Institute of Technology Singapore.All rights
		reserved.Reproduction or disclosure of this file or its contents without the prior
		written consent of DigiPen Institute of Technology is prohibited.
*/
/*!
@file		debug.h
@author		Koh Wen Yuan
@Email		k.wenyuan@digipen.edu
@course		CSD 2401
@section	Section A
@date		30 August 2023
@brief		This file contains the functions declaration for debugging
*//*______________________________________________________________________*/
#pragma once

#include <iostream>
#include <string>
#include <fstream>

//By defining G_ENABLE_DEBUG_DIAGNOSTICS you can explicitly 
//enable or disable debugging and diagnostic macros
#if !defined(G_ENABLE_DEBUG_DIAGNOSTICS) 
#   if defined(_DEBUG)
#       define G_ENABLE_DEBUG_DIAGNOSTICS 1
#   else
#       define G_ENABLE_DEBUG_DIAGNOSTICS 0
#   endif
#endif