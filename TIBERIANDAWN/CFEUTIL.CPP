#define WIN32_LEAN_AND_MEAN
#include <algorithm>
#include <Windows.h>
#include <ShlObj.h>
#include <string>

#include "FUNCTION.H"
#include "INI.H"
#include "RANGEWRAPPER.H"

std::string Get_User_Mod_Dir()
{
	std::string dirPath;
	char documentsPath[1024];
	HRESULT result = SHGetFolderPathA(NULL, CSIDL_MYDOCUMENTS, NULL, NULL, documentsPath);
	if (SUCCEEDED(result))
	{
		dirPath = documentsPath;
		dirPath += "\\CnCRemastered\\Mods\\Tiberian_Dawn";
	}
	return dirPath;
}

bool Load_INI_File(INIClass& ini, const char* path)
{
	CCFileClass file(path);
	if (file.Is_Available())
	{
		ini.Clear();
		return ini.Load(file);
	}
	return false;
}

void Populate_CFE_Patch_Config_From_INI(const INIClass& ini)
{
	static constexpr int InvalidSetting = std::numeric_limits<int>::lowest();
	const auto GetSetting = [&ini](auto& target, const char* section, const char* entry)
	{
		const int value = ini.Get_Int(section, entry, InvalidSetting);
		if (value != InvalidSetting)
			target = value;
	};

    // Make sure target has enough room for length elements before you call this!
    const auto GetString = [&ini](char target[], const char* section, const char* entry, const unsigned int length)
	{
		char* buffer = new char[length];
        const int len = ini.Get_String(section, entry, "", &buffer[0], length) ;
        buffer[length - 1] = 0;			// Make sure it's terminated
		//CFE_Debug_Printf("getstring buffer is |%s|", buffer);
        if (len > 0){
            strncpy(target, buffer, length);
            //target[length - 1] = 0;			// Make sure it's terminated
        }
        delete buffer;
	};


	//cfehunter 12/06/2020 It's really starting to get to the point where I should make feature flags
	//Features
	GetSetting(ActiveCFEPatchConfig.EnableRallyPoints         , "FEATURES", "RALLY_POINTS");
	GetSetting(ActiveCFEPatchConfig.EnableASPathing           , "FEATURES", "ASTAR_PATHING");
	GetSetting(ActiveCFEPatchConfig.EnableOOMRepair           , "FEATURES", "OOM_REPAIR");
	GetSetting(ActiveCFEPatchConfig.EnableHarvyQueueJump      , "FEATURES", "HARVY_QUEUE_JUMP");
    GetSetting(ActiveCFEPatchConfig.EnableHarvyLoadBalancing      , "FEATURES", "HARVY_LOAD_BALANCING");
    GetSetting(ActiveCFEPatchConfig.EnableHarvyOptimize      , "FEATURES", "HARVY_OPTIMIZE");
	GetSetting(ActiveCFEPatchConfig.EnableHarvySelfRepair     , "FEATURES", "HARVY_SELF_REPAIR");
	GetSetting(ActiveCFEPatchConfig.DisableCommandoAirstrikes , "FEATURES", "DISABLE_COMMANDO_AIRSTRIKES");
	GetSetting(ActiveCFEPatchConfig.EnableAttackMove          , "FEATURES", "ATTACK_MOVE");
	GetSetting(ActiveCFEPatchConfig.EnableCommandoGuard       , "FEATURES", "COMMANDO_GUARD");
	GetSetting(ActiveCFEPatchConfig.EnableSafeSabotage        , "FEATURES", "SAFE_SABOTAGE");
    GetSetting(ActiveCFEPatchConfig.EnableSmarterAircraft      , "FEATURES", "SMARTER_AIRCRAFT");
    GetSetting(ActiveCFEPatchConfig.EnableSmarterSAMs      , "FEATURES", "SMARTER_SAMS");
    GetSetting(ActiveCFEPatchConfig.BetterTibGrowth      , "FEATURES", "BETTER_TIB_GROWTH");
    GetSetting(ActiveCFEPatchConfig.TSVisceroidSpawn      , "FEATURES", "TS_VISCEROID_SPAWN");
    GetSetting(ActiveCFEPatchConfig.MeanerVisceroids      , "FEATURES", "MEANER_VISCEROIDS");
    GetSetting(ActiveCFEPatchConfig.EnableVeterancy      , "FEATURES", "VETERANCY");
    
	//Settings
    GetSetting(ActiveCFEPatchConfig.InfantryTiberiumAversion               , "SETTINGS", "INFANTRY_TIBERIUM_AVERSION");
	GetSetting(ActiveCFEPatchConfig.BuildingGap               , "SETTINGS", "BUILDING_GAP");
	GetSetting(ActiveCFEPatchConfig.HarvyQueueJumpCutoff      , "SETTINGS", "QUEUE_JUMP_CUTOFF");
    GetSetting(ActiveCFEPatchConfig.HarvyOptimizeUnloadWaitWeight      , "SETTINGS", "HARVY_OPTIMIZE_UNLOAD_WAIT_WEIGHT");
    GetSetting(ActiveCFEPatchConfig.HarvyOptimizeThrashingCutoff      , "SETTINGS", "HARVY_OPTIMIZE_THRASHING_CUTOFF");
    GetSetting(ActiveCFEPatchConfig.HarvyOptimizeThrashingNumerator      , "SETTINGS", "HARVY_OPTIMIZE_THRASHING_NUMERATOR");
    GetSetting(ActiveCFEPatchConfig.HarvyOptimizeThrashingDenominator      , "SETTINGS", "HARVY_OPTIMIZE_THRASHING_DENOMINATOR");
    GetSetting(ActiveCFEPatchConfig.HarvyOptimizeCommunalismNumerator      , "SETTINGS", "HARVY_OPTIMIZE_COMMUNALISM_NUMERATOR");
    GetSetting(ActiveCFEPatchConfig.HarvyOptimizeCommunalismDenominator      , "SETTINGS", "HARVY_OPTIMIZE_COMMUNALISM_DENOMINATOR");
	GetSetting(ActiveCFEPatchConfig.TibGrowthScale            , "SETTINGS", "TIB_GROWTH_SCALE");
	GetSetting(ActiveCFEPatchConfig.WallBuildLength           , "SETTINGS", "WALL_BUILD_LENGTH");
	GetSetting(ActiveCFEPatchConfig.WallsAlwaysHaveFullCost   , "SETTINGS", "WALLS_HAVE_FULL_COST");
    GetSetting(ActiveCFEPatchConfig.InverseBuildingPreviewMode           , "SETTINGS", "INVERSE_PREVIEW_MODE");
    GetSetting(ActiveCFEPatchConfig.SmarterSAMAmmo   , "SETTINGS", "SMARTER_SAM_AMMO");
    GetSetting(ActiveCFEPatchConfig.SmarterSAMBalanceDelay   , "SETTINGS", "SMARTER_SAM_BALANCE_DELAY");
    
    // Multiplayer
    // This works if you use first 11 chars of SteamID for your name.
    // BUT, it's useless since the server is doing EVERYTHING and the clients' DLLs appear totally dormant.
    // So everyone ends up with whatever the server's INI file says.
    //GetString(ActiveCFEPatchConfig.RealMPlayerName   , "MULTIPLAYER", "MPLAYERNAME", MPLAYER_NAME_MAX);
    
	//Clamp Wall Length between 1 (build at all) and 10 (buffer limits)
	ActiveCFEPatchConfig.WallBuildLength = (std::max)(1, (std::min)(ActiveCFEPatchConfig.WallBuildLength, 10));
    
    // Clamp inverse preview mode to legal values (0-3)
    ActiveCFEPatchConfig.InverseBuildingPreviewMode = (std::max)(0, (std::min)(ActiveCFEPatchConfig.InverseBuildingPreviewMode, 3));
    
    // If A-Star isn't enabled, distance constants will be compared against leptons rather than cells, so convert here
    if (!ActiveCFEPatchConfig.EnableASPathing){
        ActiveCFEPatchConfig.HarvyQueueJumpCutoff = Cell_To_Lepton(ActiveCFEPatchConfig.HarvyQueueJumpCutoff);
        ActiveCFEPatchConfig.HarvyOptimizeUnloadWaitWeight = Cell_To_Lepton(ActiveCFEPatchConfig.HarvyOptimizeUnloadWaitWeight);
        ActiveCFEPatchConfig.HarvyOptimizeThrashingCutoff = Cell_To_Lepton(ActiveCFEPatchConfig.HarvyOptimizeThrashingCutoff);
    }
    
    // Denominators can't be zero
    if (ActiveCFEPatchConfig.HarvyOptimizeThrashingDenominator == 0){
        ActiveCFEPatchConfig.HarvyOptimizeThrashingDenominator = 1;
    }
    if (ActiveCFEPatchConfig.HarvyOptimizeCommunalismDenominator == 0){
        ActiveCFEPatchConfig.HarvyOptimizeCommunalismDenominator = 1;
    }
    
    //growth can't be negative
    if (ActiveCFEPatchConfig.TibGrowthScale < 0){
        ActiveCFEPatchConfig.TibGrowthScale = 0;
    }
    
    // SAMs can't have less than 1 ammo
    if (ActiveCFEPatchConfig.SmarterSAMAmmo < 1){
        ActiveCFEPatchConfig.SmarterSAMAmmo = 1;
    }
    
    // overall SAM delay can't be less than 1 tick
    if (ActiveCFEPatchConfig.SmarterSAMBalanceDelay + (TICKS_PER_SECOND*3) < 1){
        ActiveCFEPatchConfig.SmarterSAMBalanceDelay = (-1*(TICKS_PER_SECOND*3))+1;
    }
    
    // Compute complex weights here so they only have to be computed once
    ActiveCFEPatchConfig.HarvyOptimizeThrashingWeight = (ActiveCFEPatchConfig.HarvyOptimizeUnloadWaitWeight * ActiveCFEPatchConfig.HarvyOptimizeThrashingNumerator) / ActiveCFEPatchConfig.HarvyOptimizeThrashingDenominator;
    ActiveCFEPatchConfig.HarvyOptimizeCommunalismWeight = (ActiveCFEPatchConfig.HarvyOptimizeUnloadWaitWeight * ActiveCFEPatchConfig.HarvyOptimizeCommunalismNumerator) / ActiveCFEPatchConfig.HarvyOptimizeCommunalismDenominator;
    
    // Only allow building off walls if wall build distance isn't longer than building gap
    if (ActiveCFEPatchConfig.WallBuildLength > ActiveCFEPatchConfig.BuildingGap){
        ActiveCFEPatchConfig.AllowSandbagging = false;
    }
    else {
        ActiveCFEPatchConfig.AllowSandbagging = true;
    }
}

bool Create_CFE_Patch_Directory(const char* path)
{
	const int directory_result = SHCreateDirectoryExA(NULL, path, NULL);
	return directory_result == ERROR_SUCCESS || directory_result == ERROR_ALREADY_EXISTS;
}

bool Reconcile_Config_With_Default(const INIClass& default, INIClass& config)
{
	bool changed = false;
	using KVPair = std::pair<const char*, const char*>;

	//Add missing entries
	for (const char* section : default)
	{
		for (KVPair entry : make_iterator_range(default.section_begin(section), default.section_end(section)))
		{
			if (!config.Is_Present(section, entry.first))
			{
				config.Put_String(section, entry.first, entry.second);
				changed = true;
			}
		}
	}

	//Remove spurious entries
	std::vector<KVPair> to_remove;
	for (const char* section : config)
	{
		for (KVPair entry : make_iterator_range(config.section_begin(section), config.section_end(section)))
		{
			if (!default.Is_Present(section, entry.first))
			{
				to_remove.push_back({ section, entry.first });
				changed = true;
			}
		}
	}

	for (KVPair entry : to_remove)
	{
		config.Clear(entry.first, entry.second);
	}

	return changed;
}

//cfehunter 12/06/2020 I really miss std::filesystem
void Initialise_CFE_Patch_Config()
{
	static constexpr const char* DefaultININame = "DEFAULT.CFEPATCHREDUX.INI";

	const std::string ini_dir = Get_User_Mod_Dir() + "\\CFEPatchRedux";
	const std::string ini_path = ini_dir + "\\CFEPATCHREDUX.INI";

	INIClass default_ini;
	Load_INI_File(default_ini, DefaultININame);

	

	//Initialise from the default config
	Populate_CFE_Patch_Config_From_INI(default_ini);

	//Overwrite settings from the user config
	INIClass user_ini;
	if (Load_INI_File(user_ini, ini_path.c_str()))
	{
		Populate_CFE_Patch_Config_From_INI(user_ini);

		//Add any missing settings
		if (Reconcile_Config_With_Default(default_ini, user_ini))
		{
			//Create the target directory
			if (Create_CFE_Patch_Directory(ini_dir.c_str()))
			{
				//Write the modified ini to a temp file
				const std::string write_path = ini_path + ".temp";
				CCFileClass write_file(write_path.c_str());
				if (write_file.Open(WRITE))
				{
					//If it was written successfully, overwrite the existing config
					if (user_ini.Save(write_file))
					{
						write_file.Close();
						CopyFileA(write_path.c_str(), ini_path.c_str(), FALSE);

						//Delete the old file
						DeleteFileA(write_path.c_str());
					}
				}
			}
		}
	}
	else
	{
		//Clone the default
		CCFileClass defaultConfig(DefaultININame);
		if (defaultConfig.Is_Available())
		{
			//Create directories and clone the default config
			if (Create_CFE_Patch_Directory(ini_dir.c_str()))
			{
				//Copy the default ini to the user directory so they can modify it later
				CopyFileA(defaultConfig.File_Name(), ini_path.c_str(), TRUE);
			}
		}
	}

}
