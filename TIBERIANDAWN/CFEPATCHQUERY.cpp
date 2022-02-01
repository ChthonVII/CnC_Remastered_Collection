#include "FUNCTION.H"

extern bool DLL_Export_Get_Input_Key_State(KeyNumType key);
//extern void Logic_Switch_Player_Context(HouseClass *house);

extern void On_Sound_Effect(const HouseClass* player_ptr, int sound_effect_index, const char* extension, int variation, COORDINATE coord);

//Disable A10s in missions if the player has no buildings
bool CFE_Patch_A10_Override(const ObjectTypeClass& object, const HousesType house)
{
	if (ActiveCFEPatchConfig.DisableCommandoAirstrikes
		&& GameToPlay == GAME_NORMAL
		&& house != PlayerPtr->Class->House
		&& object.What_Am_I() == RTTI_AIRCRAFTTYPE
		&& ((const AircraftTypeClass&)(object)).Type == AIRCRAFT_A10)
	{
		return CFE_Patch_House_Has_Any_Buildings_In_Map(*PlayerPtr);
	}
	return true;
}

bool CFE_Patch_House_Has_Any_Buildings_In_Map(HouseClass& house)
{
	if (house.ActiveBScan)
	{
		/*cfehunter 12/06/2020
		*Certain missions give you buildings that are owned by you, but not yet discovered
		*We need to account for this here.
		*/
		if (house.IsHuman && GameToPlay == GAME_NORMAL)
		{
			for (const void* rawBuilding : Buildings.ActivePointers)
			{
				const BuildingClass& building = *reinterpret_cast<const BuildingClass*>(rawBuilding);
				if (building.House == &house && building.Is_Discovered_By_Player(&house))
					return true;
			}
		}
		else return true;
	}
	return false;
}

bool CFE_Patch_Is_Wall(const ObjectTypeClass& object)
{
	return object.What_Am_I() == RTTI_BUILDINGTYPE && static_cast<const BuildingTypeClass&>(object).IsWall;
}

bool CFE_Patch_Can_Have_Rally_Point(const ObjectClass& object)
{
	return object.What_Am_I() == RTTI_BUILDING && static_cast<const BuildingClass&>(object).Can_Have_Rally_Point();
}

bool CFE_Patch_Should_Extend_Walls()
{
	//#CFE TODO: This isn't multiplayer safe // Chthon CFE Note -- actually it is MP safe.
	return ActiveCFEPatchConfig.WallBuildLength > 1 && !DLL_Export_Get_Input_Key_State(KN_LCTRL);
}

bool CFE_Patch_Is_Cell_Friendly_To_House(const HousesType house, const CELL cell)
{
    // Chthon CFE Note -- changed so building off walls is toggleable
	if (Map[cell].Owner == house) {
        if (    ActiveCFEPatchConfig.AllowSandbagging || 
                (Map[cell].Overlay == OVERLAY_NONE) ||
                !(OverlayTypeClass::As_Reference(Map[cell].Overlay).IsWall)
        ){
            return(true);
        }
        else {
            return false;
        }
	}

	if (const BuildingClass* const base = Map[cell].Cell_Building()) {
        // Chthon CFE Note: check for active, potential crash bug if we don't
		if (base->IsActive && base->House->Class->House == house) {
			return true;
		}
	}
	return false;
}

bool CFE_Patch_Is_Cell_In_Radius_To_Friendly_House(const HousesType house, const CELL cell, const int radius)
{
	if (Map.In_Radar(cell))
	{
		const int cellX = Cell_X(cell);
		const int cellY = Cell_Y(cell);
		for (int y = cellY - radius, yEnd = cellY + radius; y <= yEnd; ++y) {
            // Chthon CFE Note -- don't wrap the map or call XY_Cell on a negative number (left shift on negative number is undefined)
            if ( (y < 0) || (y >= MAP_CELL_H) ){
                continue;
            }
			for (int x = cellX - radius, xEnd = cellX + radius; x <= xEnd; ++x) {
                // Chthon CFE Note -- again, don't wrap the map
                if ( (x < 0) || (x >= MAP_CELL_W) ){
                    continue;
                }
				const CELL nextCell = XY_Cell(x, y);

				if (Map.In_Radar(nextCell) && CFE_Patch_Is_Cell_Friendly_To_House(house, nextCell))
					return true;
			}
		}
	}
	return false;
}

bool CFE_Patch_Unit_Should_Rally(const TechnoClass& object)
{
	switch (object.What_Am_I())
	{
	case RTTI_UNIT:
		return !static_cast<const UnitClass&>(object).Class->IsToHarvest;
	default:
		return true;
	}
}

// Chthon CFE note. We need a full-width version of Random_Pick(), since RAND_MAX is compiler-dependent and can be as small as 15 bits (max 32767, 0x7fff)
// In fact, it sounds like it is that small for MSVC 
// input: non-negative integers min and max (unsigned ints would make more sense, but then we'd have unsigned->signed casts all over the place)
// output: integer between min and max, inclusive
// should be multiplayer safe since rand() is supposed to be synced to the same seed on each PC
// In TD (but not RA), vanilla Random_Pick() calls IRandom(), which calls rand(), so we'll use rand() here
int CFE_Patch_FullRange_Random_Pick(int min, int max){
    if (min > max){
        int temp = min;
        min = max;
        max= temp;
    }
    if (min < 0){
        min = 0;
    }
    if (max < 0){
        max = 0;
    }
    if (max == 0){
        return 0;
    }
    int adjustedmax = (max - min) + 1;
    int ceiling = INT_MAX/adjustedmax; // int division involves implicit floor() 
    ceiling *= adjustedmax;
    int fullrangerandom = 0;
    do {
        int random_one = rand() & 0x7fff;
        int random_two = rand() & 0x7fff;
        fullrangerandom = (random_one << 16) | random_two;
        if (random_one > random_two){
            fullrangerandom |= 0x8000; // this isn't great randomness, but I don't want to spend more cycles grabbing another random for just one bit.
        }
    } while (fullrangerandom > ceiling);
    return (fullrangerandom % adjustedmax) + min;
}

// Play the same sound for all human players
// used for stuff like commando voice lines that were audible to everyone in the original game,
// but which are broken in vanilla remastered b/c of PlayerPtr shenanigans
// skiphouse is a house to not player the sound for (b/c it was already played by other means)
// other parameters are the same as Sound_Effect()
void CFE_Patch_Sound_Effect_For_All_Humans(VocType voc, COORDINATE coord, int variation, HouseClass* skiphouse){

    // just play the sound normally in SP
    if (GameToPlay == GAME_NORMAL){
        if (!skiphouse || (PlayerPtr != skiphouse)){
            Sound_Effect(voc, coord, variation);
        }
        return;
    }
    // multiplayer:
    for (int i=0 ; i<MPlayerCount; i++) {
        HouseClass* someplayer = HouseClass::As_Pointer(MPlayerHouses[i]);
        if (someplayer && (someplayer->IsHuman) && (!skiphouse || (someplayer != skiphouse))) {
            On_Sound_Effect(someplayer, (int)voc, "", variation, coord);
        }
    }
    return;
}

// Play a random sound from a list for all human players
// used for stuff that was audible to everyone in original game, and used non-syncronized random pick (which meant that different players might hear different sounds)
// voclist is an array of sounds to choose from; listlength is the number of elements in voclist (use ARRAY_SIZE macro when calling)
// other parameters are the same as CFE_Patch_Sound_Effect_For_All_Humans
void CFE_Patch_Random_Sound_Effect_For_All_Humans(VocType voclist[], int listlength, COORDINATE coord, int variation, HouseClass* skiphouse){
    
    if (listlength < 1) return;
    
    if (GameToPlay == GAME_NORMAL){
        if (!skiphouse || (PlayerPtr != skiphouse)){
            Sound_Effect(voclist[Random_Pick(0, listlength -1)], coord, variation);
        }
        return;
    }
    // multiplayer:
    for (int i=0 ; i<MPlayerCount; i++) {
        HouseClass* someplayer = HouseClass::As_Pointer(MPlayerHouses[i]);
        if (someplayer && (someplayer->IsHuman) && (!skiphouse || (someplayer != skiphouse))) {
            On_Sound_Effect(someplayer, (int)voclist[Random_Pick(0, listlength -1)], "", variation, coord);
        }
    }
    return;
}

// Play the same sound for all human players who can see a unit
// used for constructing/selling sounds.
// The original game only played these for the owner.
// When not buggy, remastered plays them positionally for everyone
// This has a better feel, but it allows you to hear things under the shroud.
// So we need this function to play positionally, but only for people who can see the unit
void CFE_Patch_Sound_Effect_For_Seers(VocType voc, TechnoClass* thingy, int variation){

    if (!thingy){
        return;
    }
    
    // just play the sound normally in SP
    if (GameToPlay == GAME_NORMAL){
        if ((thingy->House == PlayerPtr) || thingy->IsDiscoveredByPlayer || Map[Coord_Cell(thingy->Center_Coord())].IsMapped){
            Sound_Effect(voc, thingy->Center_Coord(), variation);
        }
        return;
    }
    // multiplayer:
    for (int i=0 ; i<MPlayerCount; i++) {
        HouseClass* someplayer = HouseClass::As_Pointer(MPlayerHouses[i]);
        if (someplayer && (someplayer->IsHuman) && ((thingy->House == someplayer) || thingy->Is_Discovered_By_Player(someplayer) || Map[Coord_Cell(thingy->Center_Coord())].Is_Mapped(someplayer))) {
            On_Sound_Effect(someplayer, (int)voc, "", variation, thingy->Center_Coord());
        }
    }
    return;
}

void CFE_Patch_Sound_Effect_For_Seers(VocType voc, CELL cell, int variation){
    
    // just play the sound normally in SP
    if (GameToPlay == GAME_NORMAL){
        if (Map[cell].IsMapped){
            Sound_Effect(voc, Cell_Coord(cell), variation);
        }
        return;
    }
    // multiplayer:
    for (int i=0 ; i<MPlayerCount; i++) {
        HouseClass* someplayer = HouseClass::As_Pointer(MPlayerHouses[i]);
        if (someplayer && (someplayer->IsHuman) && Map[cell].Is_Mapped(someplayer)) {
            On_Sound_Effect(someplayer, (int)voc, "", variation, Cell_Coord(cell));
        }
    }
    return;
}

// Play EVA speech for all human players
// used for stuff that was audible to everyone in original game, such as incoming nuke warning
// parameters are the same as Speak()
void  CFE_Patch_Speak_For_All_Humans(VoxType voice, COORDINATE coord, HouseClass* skiphouse){

    if (GameToPlay == GAME_NORMAL){
        if (!skiphouse || (PlayerPtr != skiphouse)){
            Speak(voice, NULL, coord);
        }
        return;
    }
    // multiplayer
    for (int i=0 ; i<MPlayerCount; i++) {
        HouseClass* someplayer = HouseClass::As_Pointer(MPlayerHouses[i]);
        if (someplayer && (someplayer->IsHuman) && (!skiphouse || (someplayer != skiphouse))) {
            Speak(voice, someplayer, coord);
        }
    }

    return;
}

/* // Unfortunately, this works as intended (if you give it the first 11 characters of SteamID as your name) but doesn't solve the problem.
 * // Turns out the server is doing ALL the display work for the clients.
 * // In fact, clients' DLLs are never called to do anything at all, and probably aren't even loaded.
 * // So all this accomplishes is identifying who the server is. Which is useless to what we wanted to do.

// Chthon CFE Note: Switch player context to or from your real local player ID
// input: true to switch TO real local player ID, false to switch FROM real local player ID back to whatever it was before
void CFE_Patch_MP_Switch_Player_Context(const bool toreal){
  
    /* 
     * Generally speaking, there are 3 ways we could do this:
     * 1. Find some function that we can be absolutely certain will always be called for its first run with your real local PlayerPtr.
     * This would be ideal since it would be both reliable and fully automatic.
     * Unfortunately, it's really hard to track one down given the unreliability of the log functions.
     * 2. What we do here -- make the user give us the real player name via the ini file, then match that to a PlayerPtr
     * This is 100% accurate, but not fully automatic :(
     * 3. Find the starting location with the shortest distance to the mouse location on the first frame, and grab the associated PlayerPtr
     * This is fully automatic, but might get the wrong answer on some maps.  
     */
    
    /* continue commenting out
    
    //CFE_Debug_Printf("CFE_Patch_MP_Switch_Player_Context called");
    
    // do nothing if not multiplayer 
    if (GameToPlay != GAME_GLYPHX_MULTIPLAYER){
        return;
    }
    
    //CFE_Debug_Printf("break1");
    
    // if we don't know who we are yet, figure it out
    if (!ActiveCFEPatchConfig.IKnowWhoIAm){
        //CFE_Debug_Printf("break2");
        // give up if we didn't get a string from the ini to work with
        if (strlen(ActiveCFEPatchConfig.RealMPlayerName) == 0){
            return;
        }
        //CFE_Debug_Printf("break3");
        for (int i=0 ; i<MPlayerCount; i++) {
            HouseClass* candidate_player_ptr = HouseClass::As_Pointer(MPlayerHouses[i]);
            //CFE_Debug_Printf("i is %i, From INI |%s| versus from pointer |%s|", i, ActiveCFEPatchConfig.RealMPlayerName, candidate_player_ptr->Name);
            if (strcmp(ActiveCFEPatchConfig.RealMPlayerName, candidate_player_ptr->Name) == 0){
                ActiveCFEPatchConfig.RealPlayerPtr = candidate_player_ptr;
                ActiveCFEPatchConfig.RealLocalPlayerIndex = i;
                ActiveCFEPatchConfig.IKnowWhoIAm = true;
                //CFE_Debug_Printf("From INI |%s| MATCHED from pointer |%s|", ActiveCFEPatchConfig.RealMPlayerName, candidate_player_ptr->Name);
                break;
            }
        }
        //CFE_Debug_Printf("break4");
        // give up if nothing matched
        if (!ActiveCFEPatchConfig.IKnowWhoIAm){
            //CFE_Debug_Printf("fail after break4");
            return;
        }
    }
    //CFE_Debug_Printf("break 5?!");
    
    // change to real local player ID
    if (toreal){
        // do nothing if we have no real PlayerPtr to change to
        if (!ActiveCFEPatchConfig.RealPlayerPtr){
            return;
        }
        // save current ID
        ActiveCFEPatchConfig.SavedPlayerPtr = PlayerPtr;
        // do the switch (seems like Logic_Switch_Player_Context() and Set_Player_Context() have the same effect, differing only in how you specify the house you want in the input)
        Logic_Switch_Player_Context(ActiveCFEPatchConfig.RealPlayerPtr);
    }
    // change from real local ID back to what it was before
    else {
        // do nothing if we don't have a saved PlayerPtr to go back to
        if (!ActiveCFEPatchConfig.SavedPlayerPtr){
            return;
        }
        Logic_Switch_Player_Context(ActiveCFEPatchConfig.SavedPlayerPtr);
        ActiveCFEPatchConfig.SavedPlayerPtr = NULL;
    }
    
    return;
}
*/
