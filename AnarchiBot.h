#ifndef ANARCHIBOT_H_
#define ANARCHIBOT_H_

#include "sc2api/sc2_api.h"
#include "sc2api/sc2_args.h"
#include "sc2lib/sc2_lib.h"
#include "sc2utils/sc2_manage_process.h"
#include "sc2utils/sc2_arg_parser.h"




class AnarchiBot : public sc2::Agent {
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
	virtual void OnUnitIdle(const sc2::Unit* unit);

private:

	size_t CountUnitType(sc2::UNIT_TYPEID unit_type) {
		//from the API tutorials, to find the number of a certain unit
		return Observation()->GetUnits(sc2::Unit::Alliance::Self, IsUnit(unit_type)).size();
	}

	//! Determines if the unit matches the unit type.
	struct IsUnit {
		IsUnit(sc2::UNIT_TYPEID type) : type_(type) {};
		sc2::UNIT_TYPEID type_;
		bool operator()(const sc2::Unit& unit) { return unit.unit_type == type_; };
	};

	//try to build a structure
	bool TryBuildStructure(sc2::ABILITY_ID ability_type_for_structure, sc2::UNIT_TYPEID unit_type = sc2::UNIT_TYPEID::TERRAN_SCV);

	//try to build supply depot
	bool TryBuildSupplyDepot();

	//try to build 
	bool TryBuildBarracks();

	bool TryBuildGeyser();

	bool TryBuildFactory();


	//for finding nearest area to mine or build refinery
	const sc2::Unit* FindNearestMineralPatch(const sc2::Point2D& start);

	const sc2::Unit* FindNearestGasPatch(const sc2::Point2D& start);


	int marine_count = 0;
	int gas_count = 0;
	int collectcounter = 0;
	
	
	bool TryCollectGas();
	bool TryBuildStarport();
	bool TryUpgradeBarracks();
	bool TryUpgradeFactory();

	bool scout();

	const sc2::Unit* scouter;

	void update_enemy_location();
	void launch_attack();
	bool TryBuildSCV();
	void need_reinceforcements();
	void finish_stragglers();

	//for patrolling enemy base after winning to kill stragglers
	bool patrol_enemy_flag = false;
	//collected from examples in sc2api
	bool scouting = false;
	bool attack_time = false;
	bool already_finishing = false;

	const sc2::Point2D* enemy_location = nullptr;
	//identify units
	struct IsVespeneGeyser {
		bool operator()(const sc2::Unit& unit) {
			switch (unit.unit_type.ToType()) {
			case sc2::UNIT_TYPEID::NEUTRAL_VESPENEGEYSER: return true;
			case sc2::UNIT_TYPEID::NEUTRAL_SPACEPLATFORMGEYSER: return true;
			case sc2::UNIT_TYPEID::NEUTRAL_PROTOSSVESPENEGEYSER: return true;
			case sc2::UNIT_TYPEID::NEUTRAL_RICHVESPENEGEYSER: return true;
			case sc2::UNIT_TYPEID::NEUTRAL_PURIFIERVESPENEGEYSER: return true;
			case sc2::UNIT_TYPEID::NEUTRAL_SHAKURASVESPENEGEYSER: return true;
			default: return false;
			}
		}
	};

	struct IsRefinery {
		bool operator()(const sc2::Unit& unit) {
			switch (unit.unit_type.ToType()) {
			case sc2::UNIT_TYPEID::TERRAN_REFINERY: return true;
			default: return false;
			}
		}
	};


	struct IsMarine {
		bool operator()(const sc2::Unit& unit) {
			switch (unit.unit_type.ToType()) {
			case sc2::UNIT_TYPEID::TERRAN_MARINE: return true;
			default: return false;
			}
		}
	};

	struct IsmyAttacker {
		bool operator()(const sc2::Unit& unit) {
			switch (unit.unit_type.ToType()) {
			case sc2::UNIT_TYPEID::TERRAN_MARINE: return true;
			case sc2::UNIT_TYPEID::TERRAN_HELLION: return true;
			case sc2::UNIT_TYPEID::TERRAN_MARAUDER: return true;
			case sc2::UNIT_TYPEID::TERRAN_THOR: return true;
			case sc2::UNIT_TYPEID::TERRAN_CYCLONE: return true;
			case sc2::UNIT_TYPEID::TERRAN_SIEGETANK: return true;
			case sc2::UNIT_TYPEID::TERRAN_MEDIVAC: return true;
			default: return false;
			}
		}
	};


	struct IsCommandCenter {
		bool operator()(const sc2::Unit& unit) {
			switch (unit.unit_type.ToType()) {
			case sc2::UNIT_TYPEID::TERRAN_MARINE: return true;
			case sc2::UNIT_TYPEID::TERRAN_HELLION: return true;
			case sc2::UNIT_TYPEID::TERRAN_MARAUDER: return true;
			case sc2::UNIT_TYPEID::TERRAN_THOR: return true;
			case sc2::UNIT_TYPEID::TERRAN_CYCLONE: return true;
			case sc2::UNIT_TYPEID::TERRAN_SIEGETANK: return true;
			case sc2::UNIT_TYPEID::TERRAN_MEDIVAC: return true;
			default: return false;
			}
		}
	};


};

#endif
