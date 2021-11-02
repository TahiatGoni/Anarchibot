#pragma once

#include "sc2api/sc2_api.h"
#include "sc2api/sc2_args.h"
#include "sc2lib/sc2_lib.h"
#include "sc2utils/sc2_manage_process.h"
#include "sc2utils/sc2_arg_parser.h"

class anarchibot : public sc2::Agent {
public:

	virtual void OnGameStart();
	
	//On step, we try to build structures, instead of hardcoding the structure numbers
	//like the tutorial, we would adjust this value based on unit sizes
	virtual void OnStep();
	
	//For Barracks, they shall count number of marines, if 4 new marines have been made
	//since the last count, the Barracks shall create a medic, and then continue

	//for marines, they shall wait until a unit  size of 10 is formed with 2 medics and 10 marines
	//then these groups will scout to possible enemy locations
	//if enemy location is found on the map, the units will come back to base and wait until a 
	//sufficiently sized fleet is created and then attack all out. Medics similar behaviour

	//SCVs collect minerals (need to come up with Phosphene gas collection ability)
	virtual void OnUnitIdle(const Unit* unit);

private:

	size_t CountUnitType(UNIT_TYPEID unit_type) {
		//from the API tutorials, to find the number of a certain unit
		return Observation()->GetUnits(Unit::Alliance::Self, IsUnit(unit_type)).size();
	}


	//try to build a structure
	bool TryBuildStructure(ABILITY_ID ability_type_for_structure, UNIT_TYPEID unit_type = UNIT_TYPEID::TERRAN_SCV);

	//try to build supply depot
	bool TryBuildSupplyDepot();

	 bool TryBuildBarracks();

};

