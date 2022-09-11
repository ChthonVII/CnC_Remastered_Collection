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
    GetSetting(ActiveCFEPatchConfig.EnableQMove          , "FEATURES", "Q_MOVE");
    GetSetting(ActiveCFEPatchConfig.EnableSmarterAircraft      , "FEATURES", "SMARTER_AIRCRAFT");
    GetSetting(ActiveCFEPatchConfig.EnableSmarterSAMs      , "FEATURES", "SMARTER_SAMS");
    GetSetting(ActiveCFEPatchConfig.EnableSmarterRepairBays , "FEATURES", "SMARTER_REPAIRBAY");
    GetSetting(ActiveCFEPatchConfig.EnableSmarterMammoths      , "FEATURES", "SMARTER_MAMMOTHS");
    GetSetting(ActiveCFEPatchConfig.EnableGunboatBugfix      , "FEATURES", "GUNBOAT_BUGFIX");
    GetSetting(ActiveCFEPatchConfig.BetterTibGrowth      , "FEATURES", "BETTER_TIB_GROWTH");
    GetSetting(ActiveCFEPatchConfig.TibIndexBugfix      , "FEATURES", "TIB_INDEX_BUGFIX");
    GetSetting(ActiveCFEPatchConfig.TreeVisceroidSpawnAnyway      , "FEATURES", "TREE_VISCEROID_SPAWN_OVERRIDE");
    GetSetting(ActiveCFEPatchConfig.TSVisceroidSpawn      , "FEATURES", "TS_VISCEROID_SPAWN");
    GetSetting(ActiveCFEPatchConfig.EnableNukeTank      , "FEATURES", "NUKE_TANK");
    GetSetting(ActiveCFEPatchConfig.DisableMPNeutralAttacks      , "FEATURES", "DISABLE_MP_NEUTRAL_ATTACKS");
    GetSetting(ActiveCFEPatchConfig.EnableGDI9Fix      , "FEATURES", "GDI9_FIX");
    GetSetting(ActiveCFEPatchConfig.EnableBurningTreeFix      , "FEATURES", "BURNING_TREE_FIX");
    GetSetting(ActiveCFEPatchConfig.EnableNukeRadiusFix      , "FEATURES", "NUKE_RADIUS_FIX");
    GetSetting(ActiveCFEPatchConfig.EnableDeathsDoorFix      , "FEATURES", "DEATHS_DOOR_FIX");
    
	//Settings
    GetSetting(ActiveCFEPatchConfig.EnableVeterancy      , "SETTINGS", "VETERANCY");
    GetSetting(ActiveCFEPatchConfig.EnableCommandoGuard       , "SETTINGS", "COMMANDO_GUARD");
    GetSetting(ActiveCFEPatchConfig.SafeSabotage        , "SETTINGS", "SAFE_SABOTAGE");
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
    GetSetting(ActiveCFEPatchConfig.TreeVisceroidSpawnMode      , "SETTINGS", "TREE_VISCEROID_SPAWN_MODE");
    GetSetting(ActiveCFEPatchConfig.MeanerVisceroids      , "SETTINGS", "MEANER_VISCEROIDS");
	GetSetting(ActiveCFEPatchConfig.WallBuildLength           , "SETTINGS", "WALL_BUILD_LENGTH");
	GetSetting(ActiveCFEPatchConfig.WallsAlwaysHaveFullCost   , "SETTINGS", "WALLS_HAVE_FULL_COST");
    GetSetting(ActiveCFEPatchConfig.InverseBuildingPreviewMode           , "SETTINGS", "INVERSE_PREVIEW_MODE");
    GetSetting(ActiveCFEPatchConfig.SmarterSAMAmmo   , "SETTINGS", "SMARTER_SAM_AMMO");
    GetSetting(ActiveCFEPatchConfig.SmarterSAMBalanceDelay   , "SETTINGS", "SMARTER_SAM_BALANCE_DELAY");
    GetSetting(ActiveCFEPatchConfig.MPPreplacedUnitOrderMode   , "SETTINGS", "MP_PREPLACED_UNIT_ORDER_MODE");
    GetSetting(ActiveCFEPatchConfig.BuildingDeathAnnounceMode   , "SETTINGS", "BUILDING_DEATH_ANNOUNCE_MODE");
    
    //Balance
    GetSetting(ActiveCFEPatchConfig.OverrideAPCReqs         , "BALANCE", "DO_OVERRIDE_APC_NOREQ");
    GetSetting(ActiveCFEPatchConfig.OverrideWarFactoryHP         , "BALANCE", "DO_OVERRIDE_WFACTORY_HP");
    GetSetting(ActiveCFEPatchConfig.OverrideWarFactoryHP_Value         , "BALANCE", "WFACTORY_HP_OVERRIDE");
    GetSetting(ActiveCFEPatchConfig.OverrideStealthTankCost         , "BALANCE", "DO_OVERRIDE_STEALTHTANK_COST");
    GetSetting(ActiveCFEPatchConfig.OverrideStealthTankCost_Value         , "BALANCE", "STEALTHTANK_COST_OVERRIDE");
    GetSetting(ActiveCFEPatchConfig.OverrideArtyTurn         , "BALANCE", "DO_OVERRIDE_ARTY_TURN");
    GetSetting(ActiveCFEPatchConfig.OverrideArtyTurn_Value         , "BALANCE", "ARTY_TURN_OVERRIDE");
    GetSetting(ActiveCFEPatchConfig.OverrideMLRSReqs         , "BALANCE", "DO_MLRS_REQ_RADAR");
    GetSetting(ActiveCFEPatchConfig.OverrideMLRSHP         , "BALANCE", "DO_OVERRIDE_MLRS_HP");
    GetSetting(ActiveCFEPatchConfig.OverrideMLRSHP_Value         , "BALANCE", "MLRS_HP_OVERRIDE");
    GetSetting(ActiveCFEPatchConfig.OverrideMLRSGDIOnly         , "BALANCE", "DO_MLRS_GDI_ONLY");
    GetSetting(ActiveCFEPatchConfig.OverrideMCVSight         , "BALANCE", "DO_OVERRIDE_MCV_SIGHT");
    GetSetting(ActiveCFEPatchConfig.OverrideMCVSight_Value         , "BALANCE", "MCV_SIGHT_OVERRIDE");
    GetSetting(ActiveCFEPatchConfig.OverrideConYardSight_Value         , "BALANCE", "CONYARD_SIGHT_OVERRIDE");
    GetSetting(ActiveCFEPatchConfig.OverrideChemGuyBuff         , "BALANCE", "DO_CHEM_GUY_BUFF");
    GetSetting(ActiveCFEPatchConfig.OverrideRocketGuyHP         , "BALANCE", "DO_OVERRIDE_ROCKEYGUY_HP");
    GetSetting(ActiveCFEPatchConfig.OverrideRocketGuyHP_Value         , "BALANCE", "ROCKEYGUY_HP_OVERRIDE");
    GetSetting(ActiveCFEPatchConfig.OverrideRocketGuyCost         , "BALANCE", "DO_OVERRIDE_ROCKEYGUY_COST");
    GetSetting(ActiveCFEPatchConfig.OverrideRocketGuyCost_Value         , "BALANCE", "ROCKEYGUY_COST_OVERRIDE");
    GetSetting(ActiveCFEPatchConfig.ApplyMPBalanceToSP         , "BALANCE", "APPLY_BALANCE_TO_SINGLE_PLAYER");
    GetSetting(ActiveCFEPatchConfig.MPUnitsInSP         , "BALANCE", "MULTIPLAYER_UNITS_IN_SINGLE_PLAYER");
    
    // Multiplayer
    // This works if you use first 11 chars of SteamID for your name.
    // BUT, it's useless since the server is doing EVERYTHING and the clients' DLLs appear totally dormant.
    // So everyone ends up with whatever the server's INI file says.
    //GetString(ActiveCFEPatchConfig.RealMPlayerName   , "MULTIPLAYER", "MPLAYERNAME", MPLAYER_NAME_MAX);
    
	//Clamp Wall Length between 1 (build at all) and 10 (buffer limits)
	ActiveCFEPatchConfig.WallBuildLength = (std::max)(1, (std::min)(ActiveCFEPatchConfig.WallBuildLength, 10));
    
    // Clamp inverse preview mode to legal values (0-2)
    ActiveCFEPatchConfig.InverseBuildingPreviewMode = (std::max)(0, (std::min)(ActiveCFEPatchConfig.InverseBuildingPreviewMode, 2));
    
    // Clamp visceriod difficulty at 0-3
    ActiveCFEPatchConfig.MeanerVisceroids = (std::max)(0, (std::min)(ActiveCFEPatchConfig.MeanerVisceroids, 3));
    
    // Clamp safe sabotage at 0-2
    ActiveCFEPatchConfig.SafeSabotage = (std::max)(0, (std::min)(ActiveCFEPatchConfig.SafeSabotage, 2));
    
    // Clamp MP preplaced unit orders at 0-2
    ActiveCFEPatchConfig.MPPreplacedUnitOrderMode = (std::max)(0, (std::min)(ActiveCFEPatchConfig.MPPreplacedUnitOrderMode, 2));
    
    // Clamp building death announce mode at 0-4
    ActiveCFEPatchConfig.BuildingDeathAnnounceMode = (std::max)(0, (std::min)(ActiveCFEPatchConfig.BuildingDeathAnnounceMode, 4));
    
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
    
    // clamp balance patch values above 0
    ActiveCFEPatchConfig.OverrideWarFactoryHP_Value = (std::max)(1, ActiveCFEPatchConfig.OverrideWarFactoryHP_Value);
    ActiveCFEPatchConfig.OverrideStealthTankCost_Value = (std::max)(1, ActiveCFEPatchConfig.OverrideStealthTankCost_Value);
    ActiveCFEPatchConfig.OverrideArtyTurn_Value = (std::max)(1, ActiveCFEPatchConfig.OverrideArtyTurn_Value);
    ActiveCFEPatchConfig.OverrideMLRSHP_Value = (std::max)(1, ActiveCFEPatchConfig.OverrideMLRSHP_Value);
    ActiveCFEPatchConfig.OverrideMCVSight_Value = (std::max)(1, ActiveCFEPatchConfig.OverrideMCVSight_Value);
    ActiveCFEPatchConfig.OverrideConYardSight_Value = (std::max)(1, ActiveCFEPatchConfig.OverrideConYardSight_Value);
    ActiveCFEPatchConfig.OverrideRocketGuyHP_Value = (std::max)(1, ActiveCFEPatchConfig.OverrideRocketGuyHP_Value);
    ActiveCFEPatchConfig.OverrideRocketGuyCost_Value = (std::max)(1, ActiveCFEPatchConfig.OverrideRocketGuyCost_Value);
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
