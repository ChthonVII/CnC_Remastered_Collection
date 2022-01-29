#include "FUNCTION.H"

extern bool DLL_Export_Get_Input_Key_State(KeyNumType key);

extern void On_Sound_Effect(const HouseClass* player_ptr, int sound_effect_index, const char* extension, int variation, COORDINATE coord);

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
	//#CFE TODO: This isn't multiplayer safe  // Chthon CFE Note -- actually it is MP safe.
	return ActiveCFEPatchConfig.WallBuildLength > 1 && !DLL_Export_Get_Input_Key_State(KN_LCTRL);
}


bool CFE_Patch_Is_Cell_Friendly_To_House(const HousesType house, const CELL cell, const bool isawall)
{
	
	if (   (Map[cell].Owner == house) &&
            (   isawall ||
                (   (Map[cell].Smudge != SMUDGE_NONE) &&
                    SmudgeTypeClass::As_Reference(Map[cell].Smudge).IsBib
                )
            )
    ){
        return true;
    }

	if (const BuildingClass* const base = Map[cell].Cell_Building()) {
        // Chthon CFE Note: check for active, potential crash bug if we don't
		if (base->IsActive && (base->House->Class->House == house) && base->Class->IsBase) {
			return true;
		}
	}
	return false;
}

bool CFE_Patch_Is_Cell_In_Radius_To_Friendly_House(const HousesType house, const CELL cell, const int radius, const bool isawall)
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

				if (Map.In_Radar(nextCell) && CFE_Patch_Is_Cell_Friendly_To_House(house, nextCell, isawall))
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


// Chthon CFE note. We need a full-width version of Random_Pick(), since WW's PRNG only returns 15 bit numbers (max 32767, 0x7fff)
// For that matter, it's a janky roll-your-own PRNG that's probably biased as hell. Should maybe replace it with something better someday.
// input: non-negative integers min and max (unsigned ints would make more sense, but then we'd have unsigned->signed casts all over the place)
// output: integer between min and max, inclusive
// should be multiplayer safe since Scen.RandomNumber is supposed to be synced
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
        int random_one = Scen.RandomNumber & 0x7fff; //weird, weird overload in RandomClass means this is actually a function call
        int random_two = Scen.RandomNumber & 0x7fff;
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
    if (Session.Type == GAME_NORMAL){
        if (!skiphouse || (PlayerPtr != skiphouse)){
            Sound_Effect(voc, coord, variation);
        }
        return;
    }
    // multiplayer:
    for (int i=0 ; i<Session.Players.Count() ; i++) {
        HouseClass* someplayer = HouseClass::As_Pointer(Session.Players[i]->Player.ID);
        if (someplayer && (someplayer->IsHuman) && (!skiphouse || (someplayer != skiphouse))) {
            On_Sound_Effect(someplayer, (int)voc, "", variation, coord);
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
    if (Session.Type == GAME_NORMAL){
        if ((thingy->House == PlayerPtr) || thingy->IsDiscoveredByPlayer || Map[Coord_Cell(thingy->Center_Coord())].IsMapped){
            Sound_Effect(voc, thingy->Center_Coord(), variation);
        }
        return;
    }
    // multiplayer:
    for (int i=0 ; i<Session.Players.Count() ; i++) {
        HouseClass* someplayer = HouseClass::As_Pointer(Session.Players[i]->Player.ID);
        if (someplayer && (someplayer->IsHuman) && ((thingy->House == someplayer) || thingy->Is_Discovered_By_Player(someplayer) || Map[Coord_Cell(thingy->Center_Coord())].Is_Mapped(someplayer))) {
            On_Sound_Effect(someplayer, (int)voc, "", variation, thingy->Center_Coord());
        }
    }
    return;
}

void CFE_Patch_Sound_Effect_For_Seers(VocType voc, CELL cell, int variation){
    
    // just play the sound normally in SP
    if (Session.Type == GAME_NORMAL){
        if (Map[cell].IsMapped){
            Sound_Effect(voc, Cell_Coord(cell), variation);
        }
        return;
    }
    // multiplayer:
    for (int i=0 ; i<Session.Players.Count() ; i++) {
        HouseClass* someplayer = HouseClass::As_Pointer(Session.Players[i]->Player.ID);
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

    if (Session.Type == GAME_NORMAL){
        if (!skiphouse || (PlayerPtr != skiphouse)){
            Speak(voice, NULL, coord);
        }
        return;
    }
    // multiplayer
    for (int i=0 ; i<Session.Players.Count() ; i++) {
        HouseClass* someplayer = HouseClass::As_Pointer(Session.Players[i]->Player.ID);
        if (someplayer && (someplayer->IsHuman) && (!skiphouse || (someplayer != skiphouse))) {
            Speak(voice, someplayer, coord);
        }
    }

    return;
}

