//
// Copyright 2020 Electronic Arts Inc.
//
// TiberianDawn.DLL and RedAlert.dll and corresponding source code is free 
// software: you can redistribute it and/or modify it under the terms of 
// the GNU General Public License as published by the Free Software Foundation, 
// either version 3 of the License, or (at your option) any later version.

// TiberianDawn.DLL and RedAlert.dll and corresponding source code is distributed 
// in the hope that it will be useful, but with permitted supplemental terms 
// under Section 7 of the GPL. See the GNU General Public License in LICENSE.TXT 
// distributed with this program. You should have received a copy of the 
// GNU General Public License along with permitted supplemental terms 
// with this program. If not, see https://github.com/electronicarts/CnC_Remastered_Collection

/* $Header:   F:\projects\c&c\vcs\code\layer.cpv   2.18   16 Oct 1995 16:50:14   JOE_BOSTIC  $ */
/***********************************************************************************************
 ***             C O N F I D E N T I A L  ---  W E S T W O O D   S T U D I O S               *** 
 ***********************************************************************************************
 *                                                                                             *
 *                 Project Name : Command & Conquer                                            *
 *                                                                                             *
 *                    File Name : LAYER.CPP                                                    *
 *                                                                                             *
 *                   Programmer : Joe L. Bostic                                                *
 *                                                                                             *
 *                   Start Date : May 31, 1994                                                 *
 *                                                                                             *
 *                  Last Update : March 10, 1995 [JLB]                                         *
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*
 * Functions:                                                                                  *
 *   LayerClass::Submit -- Adds an object to a layer list.                                     *
 *   LayerClass::Sort -- Perform an incremental sort pass on the layer's objects.              *
 *   LayerClass::Sorted_Add -- Adds object in sorted order to layer.                           * 
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#include	"function.h"
#include	"layer.h"
#include "CFEDEBUG.H"


#include <windows.h>


/***********************************************************************************************
 * LayerClass::Submit -- Adds an object to a layer list.                                       *
 *                                                                                             *
 *    This routine is used to add an object to the layer list. If the list is full, then the   *
 *    object is not added.                                                                     *
 *                                                                                             *
 * INPUT:   object   -- Pointer to the object to add.                                          *
 *                                                                                             *
 * OUTPUT:  bool; Was the object added successfully?                                           *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   05/31/1994 JLB : Created.                                                                 *
 *   05/31/1994 JLB : Allows sorted insert.                                                    *
 *   01/02/1995 JLB : Fixed to work with EMSListOf template.                                   * 
 *=============================================================================================*/
bool LayerClass::Submit(ObjectClass const * object, bool sort)
{
	/*
	**	Add the object to the layer. Either at the end (if "sort" is false) or at the
	**	appropriately sorted position.
	*/
	if (sort) {
		return(Sorted_Add(object));
	}
	return(Add((ObjectClass *)object));
}


/***********************************************************************************************
 * LayerClass::Sort -- Handles sorting the objects in the layer.                               *
 *                                                                                             *
 *    This routine is used if the layer objects must be sorted and sorting is to occur now.    *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   Don't call this routine too often since it does take a bit of time to           *
 *             execute. It is a single pass binary sort and thus isn't horribly slow,          *
 *             but it does take some time.                                                     *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   10/17/1994 JLB : Created.                                                                 *
 *   03/10/1995 JLB : Uses comparison operator.                                                * 
 *=============================================================================================*/
void LayerClass::Sort(void)
{
    CFE_Debug_Printf("Enter LayerClass::Sort()");
    CFE_Debug_Printf("LayerClass::Sort() count is %i", Count());
    CFE_Debug_Printf("LayerClass::Sort() self pointer check 0x%X", this);
    CFE_Debug_Printf("LayerClass::Sort() pre-checking pointers...");
    for (int index = 0; index < Count(); index++) {
        //CFE_Debug_Printf("LayerClass::Sort() index is %i and pointer is 0x%X", index, (*this)[index]);
        if (!(*this)[index]){
            //CFE_Debug_Printf("HEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEY THERE! This is a null pointer.");
            CFE_Debug_Printf("LayerClass::Sort() index is %i and pointer is 0x%X HEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEY THERE! This is a null pointer.", index, (*this)[index]);
        }
    }
    //CFE_Debug_Printf("LayerClass::Sort() sleeping...");
    //Sleep(5000);
    CFE_Debug_Printf("LayerClass::Sort() pre-checking objects...");
    for (int index = 0; index < Count(); index++) {
        ObjectClass* obby = (*this)[index];
        if (!obby){
            CFE_Debug_Printf("LayerClass::Sort() object pre-check skipping null pointer at index %i", index);
            continue;
        }
        //CFE_Debug_Printf("LayerClass::Sort() object pre-check for %i: IsActive=%i, IsInLimbo=%i, IsDown=%i", index, obby->IsActive, obby->IsInLimbo, obby->IsDown);
        if (!obby->IsActive || obby->IsInLimbo || !obby->IsDown){
            //CFE_Debug_Printf("HEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEY THERE! This object probably shouldn't be here!");
            CFE_Debug_Printf("LayerClass::Sort() object pre-check for %i: IsActive=%i, IsInLimbo=%i, IsDown=%i HEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEY THERE! This object probably shouldn't be here!", index, obby->IsActive, obby->IsInLimbo, obby->IsDown);
            bool knowntype = false;
            void* vobby = reinterpret_cast<void*>(obby); 		
            for (int i = 0; i<Units.Length(); i++){
                //CFE_Debug_Printf("checking units 0x%X vs 0x%X", obby, Units.Raw_Ptr(i));
                if (vobby == reinterpret_cast<void*>(Units.Raw_Ptr(i))){
                    knowntype = true;
                    CFE_Debug_Printf("This object is Unit");
                    break;
                }
            }
            if (!knowntype){
                for (int i = 0; i<Factories.Length(); i++){
                    //CFE_Debug_Printf("checking Factories 0x%X vs 0x%X", obby, Factories.Raw_Ptr(i));
                    if (vobby == reinterpret_cast<void*>(Factories.Raw_Ptr(i))){
                        knowntype = true;
                        CFE_Debug_Printf("This object is Factory");
                        break;
                    }
                }
            }
            if (!knowntype){
                for (int i = 0; i<Terrains.Length(); i++){
                    //CFE_Debug_Printf("checking terrains 0x%X vs 0x%X", obby, Terrains.Raw_Ptr(i));
                    if (vobby == reinterpret_cast<void*>(Terrains.Raw_Ptr(i))){
                        knowntype = true;
                        CFE_Debug_Printf("This object is Terrain");
                        break;
                    }
                }
            }
            if (!knowntype){
                for (int i = 0; i<Templates.Length(); i++){
                    //CFE_Debug_Printf("checking templates 0x%X vs 0x%X", obby, Templates.Raw_Ptr(i));
                    if (vobby == reinterpret_cast<void*>(Templates.Raw_Ptr(i))){
                        knowntype = true;
                        CFE_Debug_Printf("This object is Template");
                        break;
                    }
                }
            }
            if (!knowntype){
                for (int i = 0; i<Smudges.Length(); i++){
                    //CFE_Debug_Printf("checking smudges 0x%X vs 0x%X", obby, Smudges.Raw_Ptr(i));
                    if (vobby == reinterpret_cast<void*>(Smudges.Raw_Ptr(i))){
                        knowntype = true;
                        CFE_Debug_Printf("This object is Smudge");
                        break;
                    }
                }
            }
            if (!knowntype){
                for (int i = 0; i<Overlays.Length(); i++){
                    CFE_Debug_Printf("checking overlays 0x%X vs 0x%X", obby, Overlays.Raw_Ptr(i));
                    if (vobby == reinterpret_cast<void*>(Overlays.Raw_Ptr(i))){
                        knowntype = true;
                        CFE_Debug_Printf("This object is Overlay");
                        break;
                    }
                }
            }
            if (!knowntype){
                for (int i = 0; i<Infantry.Length(); i++){
                    //CFE_Debug_Printf("checking infantry 0x%X vs 0x%X", obby, Infantry.Raw_Ptr(i));
                    if (vobby == reinterpret_cast<void*>(Infantry.Raw_Ptr(i))){
                        knowntype = true;
                        CFE_Debug_Printf("This object is Infantry");
                        break;
                    }
                }
            }
            if (!knowntype){
                for (int i = 0; i<Bullets.Length(); i++){
                    //CFE_Debug_Printf("checking Bullets 0x%X vs 0x%X", obby, Bullets.Raw_Ptr(i));
                    if (vobby == reinterpret_cast<void*>(Bullets.Raw_Ptr(i))){
                        knowntype = true;
                        CFE_Debug_Printf("This object is Bullet");
                        break;
                    }
                }
            }
            if (!knowntype){
                for (int i = 0; i<Buildings.Length(); i++){
                    //CFE_Debug_Printf("checking Buildings 0x%X vs 0x%X", obby, Buildings.Raw_Ptr(i));
                    if (vobby == reinterpret_cast<void*>(Buildings.Raw_Ptr(i))){
                        knowntype = true;
                        CFE_Debug_Printf("This object is Buildings");
                        break;
                    }
                }
            }
            if (!knowntype){
                for (int i = 0; i<Anims.Length(); i++){
                    //CFE_Debug_Printf("checking Anims 0x%X vs 0x%X", obby, Anims.Raw_Ptr(i));
                    if (vobby == reinterpret_cast<void*>(Anims.Raw_Ptr(i))){
                        knowntype = true;
                        CFE_Debug_Printf("This object is Anim");
                        break;
                    }
                }
            }
            if (!knowntype){
                for (int i = 0; i<Aircraft.Length(); i++){
                    //CFE_Debug_Printf("checking Aircraft 0x%X vs 0x%X", obby, Aircrafts.Raw_Ptr(i));
                    if (vobby == reinterpret_cast<void*>(Aircraft.Raw_Ptr(i))){
                        knowntype = true;
                        CFE_Debug_Printf("This object is Aircraft");
                        break;
                    }
                }
            }
            if (!knowntype){
                for (int i = 0; i<Triggers.Length(); i++){
                    //CFE_Debug_Printf("checking Triggers 0x%X vs 0x%X", obby, Triggers.Raw_Ptr(i));
                    if (vobby == reinterpret_cast<void*>(Triggers.Raw_Ptr(i))){
                        knowntype = true;
                        CFE_Debug_Printf("This object is Trigger");
                        break;
                    }
                }
            }
            if (!knowntype){
                for (int i = 0; i<TeamTypes.Length(); i++){
                    //CFE_Debug_Printf("checking TeamTypes 0x%X vs 0x%X", obby, TeamTypes.Raw_Ptr(i));
                    if (vobby == reinterpret_cast<void*>(TeamTypes.Raw_Ptr(i))){
                        knowntype = true;
                        CFE_Debug_Printf("This object is TeamType");
                        break;
                    }
                }
            }
            if (!knowntype){
                for (int i = 0; i<Teams.Length(); i++){
                    //CFE_Debug_Printf("checking Teams 0x%X vs 0x%X", obby, Teams.Raw_Ptr(i));
                    if (vobby == reinterpret_cast<void*>(Teams.Raw_Ptr(i))){
                        knowntype = true;
                        CFE_Debug_Printf("This object is Team");
                        break;
                    }
                }
            }
            if (!knowntype){
                for (int i = 0; i<Houses.Length(); i++){
                    //CFE_Debug_Printf("checking Houses 0x%X vs 0x%X", obby, Houses.Raw_Ptr(i));
                    if (vobby == reinterpret_cast<void*>(Houses.Raw_Ptr(i))){
                        knowntype = true;
                        CFE_Debug_Printf("This object is House");
                        break;
                    }
                }
            }
            if (!knowntype){
                CFE_Debug_Printf("Unexpected... Maybe a wild pointer");
            }
        }
    }
    //CFE_Debug_Printf("LayerClass::Sort() sleeping...");
    //Sleep(5000);
    /*
    CFE_Debug_Printf("LayerClass::Sort() pre-checking Y Sorts...");
    for (int index = 0; index < Count(); index++) {
        ObjectClass* obby = (*this)[index];
        if (!obby->IsActive || obby->IsInLimbo || !obby->IsDown){
            CFE_Debug_Printf("Skipping bad object!!!");
        }
        else{
            CFE_Debug_Printf("LayerClass::Sort() index is %i and Sort_Y is 0x%X and what am i? %i", index, obby->Sort_Y(), obby->What_Am_I());
        }
    }
    */
    //CFE_Debug_Printf("LayerClass::Sort() sleeping...");
    //Sleep(5000);
    
    CFE_Debug_Printf("LayerClass::Sort() fixing problem...");
    bool happy = false;
    while (!happy){
        happy = true;
        for (int index = 0; index < Count()-1; index++) {
            ObjectClass* obby = (*this)[index];
            if (!obby->IsActive || obby->IsInLimbo || !obby->IsDown){
                Delete(index);
                happy = false;
                break;
            }
        }
    }
    
    CFE_Debug_Printf("LayerClass::Sort() starting loop...");
	for (int index = 0; index < Count()-1; index++) {
        //CFE_Debug_Printf("LayerClass::Sort() index is %i", index);
        //CFE_Debug_Printf("LayerClass::Sort() first pointer is 0x%X", (*this)[index+1]);
        //CFE_Debug_Printf("LayerClass::Sort() second pointer is 0x%X", (*this)[index]);
        //CFE_Debug_Printf("LayerClass::Sort() dereferencing first pointer...");
        //ObjectClass* obby1 = (*this)[index+1];
        //CFE_Debug_Printf("LayerClass::Sort() first object type is %i", obby1->What_Am_I());
        //CFE_Debug_Printf("LayerClass::Sort() first object active? %i", obby1->IsActive);
        //CFE_Debug_Printf("LayerClass::Sort() first object Sort_Y() is %i", obby1->Sort_Y());
        //CFE_Debug_Printf("LayerClass::Sort() dereferencing second pointer...");
        //ObjectClass* obby2 = (*this)[index];
        //CFE_Debug_Printf("LayerClass::Sort() second object type is %i", obby2->What_Am_I());
        //CFE_Debug_Printf("LayerClass::Sort() second object active? %i", obby2->IsActive);
        //CFE_Debug_Printf("LayerClass::Sort() second object Sort_Y() is %i", obby2->Sort_Y());
		if (*(*this)[index+1] < *(*this)[index]) {
			ObjectClass * temp;

			temp = (*this)[index+1];
			(*this)[index+1] = (*this)[index];
			(*this)[index] = temp;
		}
	}
	CFE_Debug_Printf("Exit LayerClass::Sort()");
}


/*********************************************************************************************** 
 * DynamicVectorClass<T>::Sorted_Add -- Adds object in sorted order to vector.                 * 
 *                                                                                             * 
 *    Use this routine to add an object to the vector but it will be inserted in sorted        * 
 *    order. This depends on the ">" operator being defined for the vector object.             * 
 *                                                                                             * 
 * INPUT:   object   -- Reference to the object that will be added to the vector.              * 
 *                                                                                             * 
 * OUTPUT:  bool; Was the object added to the vector successfully?                             * 
 *                                                                                             * 
 * WARNINGS:   none                                                                            * 
 *                                                                                             * 
 * HISTORY:                                                                                    * 
 *   03/10/1995 JLB : Created.                                                                 * 
 *=============================================================================================*/
int LayerClass::Sorted_Add(ObjectClass const * const object)
{
	if (ActiveCount >= (int)Length()) {
		if ((IsAllocated || !VectorMax) && GrowthStep > 0) {
			if (!Resize(Length() + GrowthStep)) {

				/*
				**	Failure to increase the size of the vector is an error condition.
				**	Return with the error flag.
				*/
				return(false);
			}
		} else {

			/*
			**	Increasing the size of this vector is not allowed! Bail this
			**	routine with the error code.
			*/
			return(false);
		}
	}

	/*
	**	There is room for the new object now. Add it to the right sorted position.
	*/
	int index;
	for (index = 0; index < ActiveCount; index++) {
		if ((*(*this)[index]) > (*object)) {
			break;
		}
	}

	/*
	**	Make room if the insertion spot is not at the end of the vector.
	*/
	for (int i = ActiveCount-1; i >= index; i--) {
		(*this)[i+1] = (*this)[i];
	}
	(*this)[index] = (ObjectClass *)object;
	ActiveCount++;
	return(true);
}	


