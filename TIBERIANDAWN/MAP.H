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

/* $Header:   F:\projects\c&c\vcs\code\map.h_v   2.19   16 Oct 1995 16:46:12   JOE_BOSTIC  $ */
/***********************************************************************************************
 ***              C O N F I D E N T I A L  ---  W E S T W O O D  S T U D I O S               ***
 ***********************************************************************************************
 *                                                                                             *
 *                 Project Name : Command & Conquer                                            *
 *                                                                                             *
 *                    File Name : MAP.H                                                        *
 *                                                                                             *
 *                   Programmer : Joe L. Bostic                                                *
 *                                                                                             *
 *                   Start Date : April 29, 1994                                               *
 *                                                                                             *
 *                  Last Update : April 29, 1994   [JLB]                                       *
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*
 * Functions:                                                                                  *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#ifndef MAP_H
#define MAP_H

#include	"gscreen.h"

// megamaps
// #define	BIGMAP	0


class MapClass: public GScreenClass
{
	public:

        // Chthon CFE Note: needs destructor and it should be virtual, as per https://github.com/TheAssemblyArmada/Vanilla-Conquer/commit/9210c42e5eaca6cd1bd321451fa6c95e925b3887
        // also, where is the constructor?....
        virtual ~MapClass(){}
        
		/*
		** Initialization
		*/
		virtual void One_Time(void);						// Theater-specific inits
		virtual void Init_Clear(void);						// Clears all to known state
		virtual void Alloc_Cells(void);						// Allocates buffers
		virtual void Free_Cells(void);							// Frees buffers
		virtual void Init_Cells(void);							// Frees buffers

		// Added this function to allow the editor to setup the map without setting up the entire system. - 06/18/2019 JAS
		void One_Time_Editor(void);

		/*--------------------------------------------------------
		** Main functions that deal with groupings of cells within the map or deals with the cell
		** as it relates to the map - not what the cell contains.
		*/
		ObjectClass *  Close_Object(COORDINATE coord) const;
		virtual void Detach(ObjectClass * ) {};
		int  Cell_Region(CELL cell);
		int  Cell_Threat(CELL cell, HousesType house);
		int  Cell_Distance(CELL cell1, CELL cell2);
		bool  In_Radar(CELL cell) const;
		void  Sight_From(HouseClass *house, CELL cell, int sightrange, bool incremental=false);		// Added house pointer parameter. ST - 3/6/2019 10:23AM
		void  Place_Down(CELL cell, ObjectClass * object);
		void  Pick_Up(CELL cell, ObjectClass * object);
		void  Overlap_Down(CELL cell, ObjectClass * object);
		void  Overlap_Up(CELL cell, ObjectClass * object);
		bool  Read_Binary(char const *root, unsigned long *crc);
		bool  Write_Binary(char const *root);
		bool  Place_Random_Crate(void);

		//Added for loading custom maps - 2019/10/28 JAS
		bool Read_Binary_File(char const *fname, unsigned long *crc);

		long  Overpass(void);

		virtual void Logic(void);
        virtual void CFELogic(void); // Chthon CFE Note: Alternative logic for better tiberium growth.
		virtual void Set_Map_Dimensions(int x, int y, int w, int h);

		/*
		**	File I/O.
		*/
		virtual void Code_Pointers(void);
		virtual void Decode_Pointers(void);

		/*
		** Debug routine
		*/
		int Validate(void);

		/*
		** Catch-all for bad overlappers
		*/
		void Clean(void);

#ifdef USE_RA_AI
		/*
		** Pulled in from RA for AI. ST - 7/24/2019 5:53PM
		*/
		CELL Nearby_Location(CELL cell) const;	//, SpeedType speed, int zone, MZoneType check) const
#endif //USE_RA_AI

        // Chthon CFE Note -- changed this to enable searching for only walls that belong to a given house
		//int Scan_For_Overlay(const CELL origin, const FacingType dir, const OverlayType overlay, const int maxLength) const;
        int Scan_For_Overlay(const CELL origin, const FacingType dir, const OverlayType overlay, const int maxLength, const bool limittohouse = false, const HousesType whichhouse = HOUSE_NONE) const;
        
        // Chthon CFE Note: For advanced wall ownership        
        void FlipWalls(CELL centercell);
        
		/*
		**	This is the dimensions and position of the sub section of the global map.
		**	It is this region that appears on the radar map and constrains normal
		**	movement.
		*/
		int MapCellX;
		int MapCellY;
		int MapCellWidth;
		int MapCellHeight;

		/*
		**	This is the total value of all harvestable Tiberium on the map.
		*/
		long TotalValue;
        
        // megamaps
        /*
        **	To enable big maps we need to load the normal format a little differently.
        **  This value should be set when reading the map ini before the binary is loaded.
        */
        int MapBinaryVersion;
        float MegaMapTibScaleFactor = 1.0f;
        // don't need to offset this against padding since we've already hopelessly broken save compatibility

        // // not implementing vanilla-conquer's "unsigned long -> unit32_t" change b/c it's a pain and remastered doesn't need it (we're only ever compiling with MSVC)
        //bool Read_Binary_Big(char const* fname, uint32_t* crc);
        bool Read_Binary_Big(char const* fname, unsigned long* crc);
        bool Write_Binary_Big(char const* root);
        
	protected:

		/*
		**	These are the size dimensions of the underlying array of cell objects.
		**	This is the dimensions of the "map" that the tactical view is
		**	restricted to.
		*/
		int	XSize;
		int	YSize;
		int	Size;

		static int const RadiusCount[11];
		static int const RadiusOffset[];

	private:
		friend class CellClass;

		/*
		**	Tiberium growth potiential cells are recorded here.
		*/
        // megamaps
        // vanilla-conquer changes these to MAP_CELL_W / 2 without explanation
        // In order for megamaps to maintain the vanilla bias for cells at the start/end of the array getting picked for grow/spread,
        // we need to preserve the ratio of buffer :: MAP_CELL_TOTAL.
        // (And in order to preserve the original bias for non-megamaps, we need to pretend the buffer is still only 50)
		//CELL TiberiumGrowth[50];
		CELL TiberiumGrowth[200];
		int TiberiumGrowthCount;

		/*
		**	List of cells that are full enough strength that they could spread
		**	Tiberium to adjacent cells.
		*/
        // megamaps -- ditto comment above
		//CELL TiberiumSpread[50];
		CELL TiberiumSpread[200];
		int TiberiumSpreadCount;

		/*
		**	This is the current cell number in the incremental map scan process.
		*/
		CELL TiberiumScan;

		/*
		**	If the Tiberium map scan is processing forward, then this flag
		**	will be true. It alternates between forward and backward scanning
		**	in order to avoid the "Tiberium Creep".
		*/
		unsigned IsForwardScan:1;

		enum MapEnum {SCAN_AMOUNT=MAP_CELL_TOTAL};

        // ChthonVII TS-style visceroid spawning
    public:
        // generate visceroids if infantry died in interesting ways
        bool ProcessTiberiumDeath(CELL where, COORDINATE specifically_where, unsigned char how);
		
		// Chthon CFE Note: Delayed handling of crate bombs
		void ProcessCrateBombs();
        
        unsigned char VisceroidOffalCount = 0;
        unsigned char VisceroidTreeFrustrationCount = 0;
    private:
        
		/*
		** Some additional padding in case we need to add data to the class and maintain backwards compatibility for save/load
		*/
        // Chthon CFE Note: original padding was 1024
        // amazingly, we have enough padding to get back to original size despite megamaps
		unsigned char SaveLoadPadding[414];
};

#endif
