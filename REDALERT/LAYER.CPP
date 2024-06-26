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

/* $Header: /CounterStrike/LAYER.CPP 1     3/03/97 10:25a Joe_bostic $ */
/***********************************************************************************************
 ***              C O N F I D E N T I A L  ---  W E S T W O O D  S T U D I O S               ***
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
 *   LayerClass::Sort -- Perform an incremental sort pass on the layer's objects.              *
 *   LayerClass::Sorted_Add -- Adds object in sorted order to layer.                           *
 *   LayerClass::Submit -- Adds an object to a layer list.                                     *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#include	"function.h"
#include	"layer.h"
#include "CFEDEBUG.H"


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
    // Chthon CFE Note: This... does not result in a fully sorted list.
    // Presumably for speed reasons.
    // It's essentially a bubble sort that takes several frames to catch up... assuming that the objects hold still long enough
    /*
	for (int index = 0; index < Count()-1; index++) {
		if (*(*this)[index+1] < *(*this)[index]) {
			ObjectClass * temp;

			temp = (*this)[index+1];
			(*this)[index+1] = (*this)[index];
			(*this)[index] = temp;
		}
	}
	*/
    
    // replace with a real bubble sort (should be efficient because input list is likely to be nearly sorted already)
	bool happy = false;
    int endpt = Count() - 1;
    while (!happy){
        happy = true;
        for (int index = 0; index < endpt; index++) {
            if (*(*this)[index+1] < *(*this)[index]) {
                happy = false;
                ObjectClass * temp;
                temp = (*this)[index+1];
                (*this)[index+1] = (*this)[index];
                (*this)[index] = temp;
            }
        }
        endpt--; // after N iterations of the outer loop, the back N items will be correct
    }
    
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
	if ((unsigned)ActiveCount >= Length()) {
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


// Chthon CFE Note: Detect bad pointers and remove them from the Layer.
// This is intended as a last resort against bad pointers in the Layers causes crashes when loading saved games.
// This function is slow as ass, and so should not be used elsewhere.
// returns true if the Layer was clean; false if corrections were made
bool LayerClass::SanitizeLayer(){
    
    bool retval = true;
    
    // check for null and wild pointers
    // There's a faster way to do this, but I don't trust WW's DIY vector implementation to iterate correctly if a removal is made mid-loop.
    bool happy = false;
    while (!happy){
        happy = true;
        for (int index = 0; index < Count(); index++) {
            ObjectClass* obby = (*this)[index];
            if (!obby){
                CFE_Debug_Printf("SanitizeLayer() found a null pointer at index %i! (Please report this.)", index);
                retval = false;
                happy = false;
                Delete(index);
                break;
            }
            RTTIType thisrtti = RTTI_NONE;
            unsigned char thisextrartti = 0;
            if (!CFE_WildPointerCheck(reinterpret_cast<void*>(obby), thisrtti, thisextrartti)){
                CFE_Debug_Printf("SanitizeLayer() found a wild pointer (0x%X) at index %i! (Please report this.)", obby, index);
                retval = false;
                happy = false;
                Delete(index);
                break;
            }
            if (thisextrartti > 0){
                CFE_Debug_Printf("SanitizeLayer() found a pointer (0x%X, RTTI %i, extraRTTI %i) to something that isn't an ObjectClass at index %i! (Please report this.)", obby, thisrtti, thisextrartti, index);
                retval = false;
                happy = false;
                Delete(index);
                break;
            }
            if (!obby->IsActive){
                CFE_Debug_Printf("SanitizeLayer() found a dangling pointer (0x%X, RTTI %i, extraRTTI %i) at index %i! (Please report this.)", obby, thisrtti, thisextrartti, index);
                retval = false;
                happy = false;
                Delete(index);
                break;
            }
            if (obby->IsInLimbo || !obby->IsDown){
                CFE_Debug_Printf("SanitizeLayer() found an object (0x%X, RTTI %i, extraRTTI %i) that's not on the map (IsInLimbo %i, IsDown %i) at index %i! (Please report this.)", obby, thisrtti, thisextrartti, obby->IsInLimbo, obby->IsDown, index);
                retval = false;
                happy = false;
                Delete(index);
                break;
            }
        }
    }
    
    // check for duplicates
    for (int index = 0; index < Count()-1; index++) {
        ObjectClass* obby = (*this)[index];
        bool happy = false;
        while (!happy){
            happy = true;
            for (int j = index+1; j < Count(); j++){
                ObjectClass* jobby = (*this)[j];
                if (obby == jobby){
                    CFE_Debug_Printf("SanitizeLayer() found a duplicate (0x%X) at indices %i and %i! (Please report this.)", obby, index, j);
                    retval = false;
                    happy = false;
                    Delete(j);
                    break;
                }
            }
        }
    }
    
    
    return retval;
}
