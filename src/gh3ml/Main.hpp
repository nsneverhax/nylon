#pragma once

#include <GH3ML/LogSource.hpp>

#include <map>

#include <GH3ML/ModInfo.hpp>

namespace gh3ml::internal
{
	extern LogSource Log;
	extern LogSource LogGH3;
	
	extern std::string ModsPath;

	extern std::map<std::string, ModInfo> LoadedMods;
	
	extern void SetupDefaultHooks();
	void SetupCFuncRedirection();

	extern void LoadMods();
	
	void ReadConfig();
	
}