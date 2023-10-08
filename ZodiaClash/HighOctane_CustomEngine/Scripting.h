#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/mono-config.h>
#include <mono/metadata/mono-debug.h>
#include <mono/metadata/exception.h>
#include <iostream>
#include <filesystem>

#pragma once

void InitMono();

void LoadCSharpScript();