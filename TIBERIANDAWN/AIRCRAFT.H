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

/* $Header:   F:\projects\c&c\vcs\code\aircraft.h_v   2.17   16 Oct 1995 16:47:48   JOE_BOSTIC  $ */
/***********************************************************************************************
 ***              C O N F I D E N T I A L  ---  W E S T W O O D  S T U D I O S               ***
 ***********************************************************************************************
 *                                                                                             *
 *                 Project Name : Command & Conquer                                            *
 *                                                                                             *
 *                    File Name : AIRCRAFT.H                                                   *
 *                                                                                             *
 *                   Programmer : Joe L. Bostic                                                *
 *                                                                                             *
 *                   Start Date : July 22, 1994                                                *
 *                                                                                             *
 *                  Last Update : November 28, 1994 [JLB]                                      *
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*
 * Functions:                                                                                  *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#ifndef AIRCRAFT_H
#define AIRCRAFT_H

#include	"radio.h"
#include	"fly.h"
#include	"target.h"


class AircraftClass : public FootClass, public FlyClass
{
	public:
		/*
		**	This is a pointer to the class control structure for the aircraft.
		*/
		AircraftTypeClass const * const Class;

		//-----------------------------------------------------------------------------
		void * operator new(size_t);
		void operator delete(void *);
		operator AircraftType(void) const {return Class->Type;};
		AircraftClass(void) : Class(0) {};
		AircraftClass(AircraftType classid, HousesType house);
		virtual ~AircraftClass(void);
		virtual RTTIType What_Am_I(void) const {return RTTI_AIRCRAFT;};

		static void Init(void);
		enum {FLIGHT_LEVEL=24};

		virtual int Mission_Attack(void);
		virtual int Mission_Unload(void);
		virtual int Mission_Hunt(void);
		virtual int Mission_Retreat(void);
		virtual int Mission_Move(void);
		virtual int Mission_Enter(void);
		virtual int Mission_Guard(void);
		virtual int Mission_Guard_Area(void);

		/*
		**	State machine support routines.
		*/
		bool  Process_Take_Off(void);
		bool  Process_Landing(void);
		int  Process_Fly_To(bool slowdown);

		/*
		**	Query functions.
		*/
		virtual int Threat_Range(int control) const;
		virtual int Rearm_Delay(bool second) const;
		virtual MoveType Can_Enter_Cell(CELL cell, FacingType facing=FACING_NONE) const;
		virtual LayerType In_Which_Layer(void) const;
		virtual ObjectTypeClass const & Class_Of(void) const {return *Class;};
		virtual ActionType What_Action(ObjectClass * target) const;
		virtual ActionType What_Action(CELL cell) const;
		virtual DirType Desired_Load_Dir(ObjectClass * passenger, CELL & moveto) const;
		virtual int Pip_Count(void) const;
		TARGET  Good_Fire_Location(TARGET target) const;
		bool  Cell_Seems_Ok(CELL cell, bool landing=false) const;
		DirType  Pose_Dir(void) const;
		TARGET  Good_LZ(void) const;
		virtual DirType Fire_Direction(void) const;

		/*
		**	Landing zone support functionality.
		*/
		bool  Is_LZ_Clear(TARGET target) const;
		TARGET  New_LZ(TARGET oldlz, bool stable = false) const;

		/*
		**	Coordinate inquiry functions. These are used for both display and
		**	combat purposes.
		*/
		virtual COORDINATE Sort_Y(void) const;
		virtual COORDINATE Fire_Coord(int which) const;
		virtual COORDINATE Target_Coord(void) const;

		/*
		**	Object entry and exit from the game system.
		*/
		virtual bool Unlimbo(COORDINATE , DirType facing = DIR_N);

		/*
		**	Display and rendering support functionality. Supports imagery and how
		**	object interacts with the map and thus indirectly controls rendering.
		*/
		virtual int Exit_Object(TechnoClass *);
		virtual bool Mark(MarkType mark=MARK_CHANGE);
		virtual short const * Overlap_List(void) const;
		virtual void Draw_It(int x, int y, WindowNumberType window);
		virtual void Set_Speed(int speed);

		/*
		**	User I/O.
		*/
		virtual void Active_Click_With(ActionType action, ObjectClass * object);
		virtual void Active_Click_With(ActionType action, CELL cell);
		virtual void Player_Assign_Mission(MissionType mission, TARGET target=TARGET_NONE, TARGET destination=TARGET_NONE);
		virtual void Response_Select(void);
		virtual void Response_Move(void);
		virtual void Response_Attack(void);

		/*
		**	Combat related.
		*/
//		virtual bool Target_Something_Nearby(ThreatType threat=THREAT_NORMAL);
		virtual ResultType Take_Damage(int & damage, int distance, WarheadType warhead, TechnoClass * source, ArmorPenType specialarmorpen = ARMORPEN_NONE, bool givesxp = true);
		virtual BulletClass * Fire_At(TARGET target, int which);
		virtual TARGET As_Target(void) const;

		/*
		**	AI.
		*/
		virtual void AI(void);
		virtual void Enter_Idle_Mode(bool initial = false);
		virtual RadioMessageType Receive_Message(RadioClass * from, RadioMessageType message, long & param);
		virtual void Scatter(COORDINATE threat, bool forced=false, bool nokidding=false);
		void Set_Reinforcement_Delay(long delay);

		/*
		**	Scenario and debug support.
		*/
		#ifdef CHEAT_KEYS
		virtual void Debug_Dump(MonoClass *mono) const;
		#endif

		/*
		**	File I/O.
		*/
		static void  Read_INI(char *buffer);
		static void  Write_INI(char *buffer);
		static char * INI_Name(void) {return "AIRCRAFT";};
		bool  Load(FileClass & file);
		bool  Save(FileClass & file);
		virtual void Code_Pointers(void);
		virtual void Decode_Pointers(void);

		/*
		**	Dee-buggin' support.
		*/
		int Validate(void) const;
        
        // Chthon CFE Note: logic for exiting repair bay
        bool DoSmarterRunAway();

	public:

		/*
		**	This is the facing used for the body of the aircraft. Typically, this is the same
		**	as the PrimaryFacing, but in the case of helicopters, it can be different.
		*/
		FacingClass SecondaryFacing;
        
        // Chthon CFE Note: Bind the bitfields inside a Union so they pack correctly.
        // Compatibility note: MSVC is happy with anonymous union & struct, but apparently gcc needs extensions for it.
        union {
            unsigned char AircraftClassFlags;
			struct{
				unsigned IsLanding:1;
                unsigned IsTakingOff:1;
                unsigned IsHoming:1;
                unsigned IsHovering:1;
				unsigned AircraftClassFlagsPadding : 4; //round out to 1 byte, we can use this space later.
			};
		};
        

		//private: // Chthon CFE Note: these need to be public so we can check them from elsewhere 

		/*
		**	Aircraft can be in either state of landing, taking off, or in steady altitude.
		**	These flags are used to control transition between flying and landing. It is
		**	necessary to handle the transition in this manner so that it occurs smoothly
		**	during the graphic processing section.
		*/
		//unsigned IsLanding:1;
		//unsigned IsTakingOff:1;
    private:

		/*
		**	It is very common for aircraft to be homing in on a target. When this flag is
		**	true, the aircraft will constantly adjust its facing toward the TarCom. When the
		**	target is very close (one cell away or less), then this flag is automatically cleared.
		**	This is because the homing algorithm is designed to get the aircraft to the destination
		**	but no more. Checking when this flag is cleared is a way of flagging transition into
		**	a new mode. Example: Transport helicopters go into a hovering into correct position
		**	mode when the target is reached.
		*/
		//unsigned IsHoming:1;

		/*
		**	Helicopters that are about to land must hover into a position exactly above the landing
		**	zone. When this flag is true, the aircraft will be adjusted so that it is exactly over
		**	the TarCom. The facing of the aircraft is not altered by this movement. The affect
		**	like the helicopter is hovering and shifting sideways to position over the landing
		**	zone. When the position is over the landing zone, then this flag is set to false.
		*/
		//unsigned IsHovering:1;

		/*
		**	This is the jitter tracker to be used when the aircraft is a helicopter and
		**	is flying. It is most noticable when the helicopter is hovering.
		*/
		unsigned char Jitter;

	public:
		/*
		**	This is the altitude of the aircraft. It is expressed in pixels that
		**	the shadow is offset to the south. If the altitude reaches zero, then
		**	the aircraft has landed. The altitude for normal aircraft is at
		**	Flight_Level().
		*/
		int Altitude;

	private:

		/*
		**	This timer controls when the aircraft will reveal the terrain around itself.
		**	When this timer expires and this aircraft has a sight range, then the
		**	look around process will occur.
		*/
		TCountDownTimerClass SightTimer;

		/*
		**	Most attack aircraft can make several attack runs. This value contains the
		**	number of attack runs the aircraft has left. When this value reaches
		**	zero then the aircraft is technically out of ammo.
		*/
		char AttacksRemaining;
        
        unsigned char AircraftClassPadding1[3]; // to align the next long

		/*
		**	Cargo planes will wait a certain number of ticks before flying in.
		*/
		long ReinforcementStart;

        // Chthon CFE Note: Counter for helicopters to recheck target's location
    public:
        unsigned short int TargetRecheckCounter = 0;
    private:
        
		/*
		** Some additional padding in case we need to add data to the class and maintain backwards compatibility for save/load
		*/
        // Chthon CFE note: original padding was 28 bytes
        // TargetRecheckCounter eats 2 bytes
        // RESET PADDING in v1.8 b/c we're breaking save compatibility anyway
		unsigned char SaveLoadPadding[6];

        // Chthon CFE Note:
        /*  Memory map is:
         *  2 bytes     SecondaryFacing
         *  1 byte      AircraftClassFlags
         *  1 byte      Jitter
         *  4 bytes     Altitude
         *  8 bytes     SightTimer
         *  1 byte         AttacksRemaining
         * 3 bytes      AircraftClassPadding1
         * 4 bytes      ReinforcementStart
         * 2 bytes      TargetRecheckCounter
         *  6 bytes     Padding
         * 
         *  (VTable isn't part of the memory layout because it's static)
         * -----
         * Total 32 bytes
         * 
         */
        
		/*
		** This contains the value of the Virtual Function Table Pointer
		*/
		static void * VTable;
};

#endif
