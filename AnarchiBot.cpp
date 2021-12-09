#include "AnarchiBot.h"
#include <iostream>


void AnarchiBot::OnGameStart() {
	std::cout << "GO Anarchibot" << std::endl;
}

void AnarchiBot::OnStep() {

	scout();

	update_enemy_location();

	TryBuildSupplyDepot();

	TryBuildBarracks();

	TryBuildFactory();

	TryBuildGeyser();

	TryCollectGas();

	TryUpgradeBarracks();

	TryUpgradeFactory();

	TryBuildStarport();

	launch_attack();

	TryBuildSCV();

	need_reinceforcements();

	

}


void AnarchiBot::OnUnitIdle(const sc2::Unit* unit) {
	const sc2::ObservationInterface* observation = Observation();
	const sc2::GameInfo& game_info = Observation()->GetGameInfo();
	float rx = sc2::GetRandomScalar();
	float ry = sc2::GetRandomScalar();
	sc2::Point2D*  patrolPoint = nullptr;


	const sc2::Point2D* enemy_base;

	if (enemy_location) {
		enemy_base = enemy_location;
	}
	else {
		enemy_base = &game_info.enemy_start_locations.front();
	}
	const sc2::Unit* unit_patrol= nullptr;
	sc2::Units units = observation->GetUnits(sc2::Unit::Alliance::Self);
	for (const auto& unit : units) {


		if (unit->unit_type == sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER) {
			unit_patrol = unit;
		}
	}

	//get patrol coordinates
	if (unit_patrol) {
		patrolPoint = &(sc2::Point2D(unit_patrol->pos.x + rx * 20.0f, unit_patrol->pos.y + ry * 20.0f));
	

		if (patrol_enemy_flag) {
			patrolPoint = &(sc2::Point2D(enemy_base->x + rx * 45.0f, enemy_base->y + ry * 45.0f));
		}
	}

	switch (unit->unit_type.ToType()) {

	//command center builds SCVs
	case sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER: {


		if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_SCV) < unit->ideal_harvesters) {
			Actions()->UnitCommand(unit, sc2::ABILITY_ID::TRAIN_SCV);
		}
		break;
	}
	//Idle SCVs collect minerals
	case sc2::UNIT_TYPEID::TERRAN_SCV: {


		const sc2::Unit* mineral_target = FindNearestMineralPatch(unit->pos);
		if (!mineral_target) {
			break;
		}
		

		Actions()->UnitCommand(unit, sc2::ABILITY_ID::SMART, mineral_target);

		break;


	}
	//try upgrades and buld troops
	case sc2::UNIT_TYPEID::TERRAN_BARRACKS: {

		Actions()->UnitCommand(unit, sc2::ABILITY_ID::BUILD_TECHLAB);
		Actions()->UnitCommand(unit, sc2::ABILITY_ID::BUILD_TECHLAB);
		
		if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_MARAUDER) < 10) {
			Actions()->UnitCommand(unit, sc2::ABILITY_ID::TRAIN_MARAUDER);
		}
		if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_MARINE) < 16) {
			Actions()->UnitCommand(unit, sc2::ABILITY_ID::TRAIN_MARINE);
		}



		break;
	}

	//try upgrades and buld troops
	case sc2::UNIT_TYPEID::TERRAN_FACTORY: {

		Actions()->UnitCommand(unit, sc2::ABILITY_ID::BUILD_TECHLAB);
		Actions()->UnitCommand(unit, sc2::ABILITY_ID::BUILD_TECHLAB);
		if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_HELLION) < 4) {
			Actions()->UnitCommand(unit, sc2::ABILITY_ID::TRAIN_HELLION, true);
		}
		if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_THOR) < 2) {
			Actions()->UnitCommand(unit, sc2::ABILITY_ID::TRAIN_THOR, true);
		}
		if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_CYCLONE) < 2) {
			Actions()->UnitCommand(unit, sc2::ABILITY_ID::TRAIN_CYCLONE, true);
		}
		if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_SIEGETANK) < 2) {
			Actions()->UnitCommand(unit, sc2::ABILITY_ID::TRAIN_SIEGETANK, true);
		}




		break;
	}
	//idle units patrol
	case sc2::UNIT_TYPEID::TERRAN_MARINE: {
		const sc2::GameInfo& game_info = Observation()->GetGameInfo();


			sc2::Units units = observation->GetUnits(sc2::Unit::Alliance::Self, IsmyAttacker());


			if (patrolPoint) {
				Actions()->UnitCommand(unit, sc2::ABILITY_ID::GENERAL_PATROL, *patrolPoint);
			}
			break;

	}
	case sc2::UNIT_TYPEID::TERRAN_HELLION: {
		const sc2::GameInfo& game_info = Observation()->GetGameInfo();


			sc2::Units units = observation->GetUnits(sc2::Unit::Alliance::Self, IsmyAttacker());



			if (patrolPoint) {
				Actions()->UnitCommand(unit, sc2::ABILITY_ID::GENERAL_PATROL, *patrolPoint);
			}

	}
	case sc2::UNIT_TYPEID::TERRAN_MARAUDER: {
		const sc2::GameInfo& game_info = Observation()->GetGameInfo();



			if (patrolPoint) {
				Actions()->UnitCommand(unit, 17, *patrolPoint);
			}
			break;

	}
	case sc2::UNIT_TYPEID::TERRAN_THOR: {
		const sc2::GameInfo& game_info = Observation()->GetGameInfo();




			if (patrolPoint) {
				Actions()->UnitCommand(unit, sc2::ABILITY_ID::GENERAL_PATROL, *patrolPoint);
			}
			break;

	}
	case sc2::UNIT_TYPEID::TERRAN_CYCLONE: {
		const sc2::GameInfo& game_info = Observation()->GetGameInfo();



			if (patrolPoint) {
				Actions()->UnitCommand(unit, sc2::ABILITY_ID::GENERAL_PATROL, *patrolPoint);
			}
			break;


	}
	case sc2::UNIT_TYPEID::TERRAN_SIEGETANK: {
		const sc2::GameInfo& game_info = Observation()->GetGameInfo();




			if (patrolPoint) {
				Actions()->UnitCommand(unit, sc2::ABILITY_ID::GENERAL_PATROL, *patrolPoint);
			}
			break;
	

	}
	case sc2::UNIT_TYPEID::TERRAN_MEDIVAC: {
		const sc2::GameInfo& game_info = Observation()->GetGameInfo();



		if (patrolPoint) {
			Actions()->UnitCommand(unit, 17, *patrolPoint);
		}
		break;

	}

	//techlab does research
	case sc2::UNIT_TYPEID::TERRAN_BARRACKSTECHLAB: {
		const sc2::GameInfo& game_info = Observation()->GetGameInfo();
		Actions()->UnitCommand(unit, sc2::ABILITY_ID::RESEARCH_COMBATSHIELD, true);
	}

	//starport builds Medivac
	case sc2::UNIT_TYPEID::TERRAN_STARPORT: {
		const sc2::GameInfo& game_info = Observation()->GetGameInfo();
		if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_MEDIVAC) < 4) {
			Actions()->UnitCommand(unit, sc2::ABILITY_ID::TRAIN_MEDIVAC, true);
		}
	}
	default: {
		break;

	}
	}
}




bool AnarchiBot::TryBuildSupplyDepot() {

	const sc2::ObservationInterface* observation = Observation();

	// If we are not supply capped, don't build a supply depot
	if (observation->GetFoodUsed() <= observation->GetFoodCap() - 4)
		return false;
	// Try and build a depot. Find a random SCV and give it the order.
	return TryBuildStructure(sc2::ABILITY_ID::BUILD_SUPPLYDEPOT);

}

bool AnarchiBot::TryBuildStructure(sc2::ABILITY_ID ability_type_for_structure, sc2::UNIT_TYPEID unit_type) {
	const sc2::ObservationInterface* observation = Observation();

	// If a unit already is building a supply structure of this type, do nothing.
	// Also get an scv to build the structure.
	const sc2::Unit* unit_to_build = nullptr;
	sc2::Units units = observation->GetUnits(sc2::Unit::Alliance::Self);
	for (const auto& unit : units) {
		for (const auto& order : unit->orders) {
			if (order.ability_id == ability_type_for_structure) {
				return false;
			}
		}

		if (unit->unit_type == unit_type) {
			unit_to_build = unit;
		}
	}

	float rx = sc2::GetRandomScalar();
	float ry = sc2::GetRandomScalar();
	if (unit_to_build) {
		Actions()->UnitCommand(unit_to_build,
			ability_type_for_structure,
			sc2::Point2D(unit_to_build->pos.x + rx * 15.0f, unit_to_build->pos.y + ry * 15.0f));
	}
	return true;
}

void AnarchiBot::need_reinceforcements() {

	const sc2::ObservationInterface* observation = Observation();
	const sc2::GameInfo& game_info = Observation()->GetGameInfo();

	const sc2::Point2D* enemy_base;

	if (enemy_location) {
		enemy_base = enemy_location;
	}
	else {
		enemy_base = &game_info.enemy_start_locations.front();
	}

	int attacking_units = 0;

	sc2::Units units = observation->GetUnits(sc2::Unit::Alliance::Self, IsmyAttacker());

	//if more than 3 soldiers breach the enemy lines, time to send in the reinforcements to finish it all
	for (const auto& unit : units) {

		if ((((unit->pos.x - enemy_base->x) * (unit->pos.x - enemy_base->x)) + ((unit->pos.y - enemy_base->y) * (unit->pos.x - enemy_base->y))) <= 200.0f) {
			attacking_units++;
		}

	}


	if (patrol_enemy_flag) {

		finish_stragglers();
		return;
	}

	//if sufficient troops already there, no need to send more
	if (attacking_units > 15) {
		return;
	}


	if (attacking_units > 3) {
		Actions()->UnitCommand(units, sc2::ABILITY_ID::ATTACK_ATTACK, *enemy_base);
	}


}



void AnarchiBot::launch_attack() {


	const sc2::ObservationInterface* observation = Observation();
	const sc2::GameInfo& game_info = Observation()->GetGameInfo();

	const sc2::Point2D* enemy_base;

	sc2::Units units = observation->GetUnits(sc2::Unit::Alliance::Self, IsmyAttacker());
	if (enemy_location) {
		enemy_base = enemy_location;
	}
	else {
		enemy_base = &game_info.enemy_start_locations.front();
	}

	sc2::Units enemyunits = observation->GetUnits(sc2::Unit::Alliance::Enemy);

	int attacking_units = 0;

	//check if many units make it to the center of the base
	//if so, most structures destroyed so time to hunt stragglers
	for (const auto& unit : units) {

		if ((((unit->pos.x - enemy_base->x) * (unit->pos.x - enemy_base->x)) + ((unit->pos.y - enemy_base->y) * (unit->pos.x - enemy_base->y))) < 100.0f) {
			attacking_units++;
		}

	}


	if (attacking_units > 15) {

		patrol_enemy_flag = true;
		return;
	}

	if (patrol_enemy_flag) {
	
		finish_stragglers();
		return;
	}


	//if not hunting stragglers
	//once there is enough in the fleet, launch an attack with all units
	if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_MARINE) > 5 && CountUnitType(sc2::UNIT_TYPEID::TERRAN_THOR) >= 0 && CountUnitType(sc2::UNIT_TYPEID::TERRAN_MARAUDER) > 2 &&
		CountUnitType(sc2::UNIT_TYPEID::TERRAN_SIEGETANK) >= 1 && CountUnitType(sc2::UNIT_TYPEID::TERRAN_HELLION) > 2 &&
		CountUnitType(sc2::UNIT_TYPEID::TERRAN_CYCLONE) > 1) {

		Actions()->UnitCommand(units, sc2::ABILITY_ID::ATTACK_ATTACK, *enemy_base);
		

	}



	


}
bool AnarchiBot::scout() {

	if (scouting) {
		return true;
	}

	scouting = true;

	const sc2::ObservationInterface* observation = Observation();
	const sc2::GameInfo& game_info = Observation()->GetGameInfo();
	const std::vector<sc2::Point2D> enemy_bases = game_info.enemy_start_locations;

	const sc2::Unit* unit_to_build = nullptr;
	sc2::Units units = observation->GetUnits(sc2::Unit::Alliance::Self);
	for (const auto& unit : units) {

		if (unit->unit_type == sc2::UNIT_TYPEID::TERRAN_SCV) {
			unit_to_build = unit;
			scouter = unit;
		}
	}


	for (const auto& enemy_base : enemy_bases) {
		Actions()->UnitCommand(unit_to_build, sc2::ABILITY_ID::MOVE_MOVE, enemy_base, true);

	}

	return false;
}


void  AnarchiBot::update_enemy_location() {

	const sc2::ObservationInterface* observation = Observation();
	const sc2::GameInfo& game_info = Observation()->GetGameInfo();

	if (enemy_location) {
		return;
	}

	if (game_info.enemy_start_locations.size() == 1) {
		enemy_location = &game_info.enemy_start_locations.front();
		return;
	}

	
	sc2::Units enemyunits = observation->GetUnits(sc2::Unit::Alliance::Enemy);

	if (enemyunits.size() > 0) {

		enemy_location = new sc2::Point2D(enemyunits.front()->pos.x, enemyunits.front()->pos.y);


	}
	
}


void AnarchiBot::finish_stragglers() {
	sc2::Units units_at_enemy;
	const sc2::ObservationInterface* observation = Observation();
	const sc2::GameInfo& game_info = Observation()->GetGameInfo();

	sc2::Units units = observation->GetUnits(sc2::Unit::Alliance::Self, IsmyAttacker());
	const sc2::Point2D* enemy_base;

	if (enemy_location) {
		enemy_base = enemy_location;
	}
	else {
		enemy_base = &game_info.enemy_start_locations.front();
	}
	bool not_already_patrolling = true;


	//check units, if time to find stragglers, then randomly patrol and search
	for (const auto& unit : units) {

		for (const auto& order : unit->orders) {
			if (order.ability_id == 17 || order.ability_id == sc2::ABILITY_ID::ATTACK_ATTACK) {
				not_already_patrolling = false;
			}
		}


		if (not_already_patrolling) {
			units_at_enemy.push_back(unit);
		}
	}


	float rx = sc2::GetRandomScalar();
	float ry = sc2::GetRandomScalar();
	sc2::Point2D* patrolPoint = nullptr;


	patrolPoint = &(sc2::Point2D(enemy_base->x + rx * 45.0f, enemy_base->y + ry * 45.0f));

	Actions()->UnitCommand(units_at_enemy, 16, patrolPoint);


	sc2::Units enemyunits = observation->GetUnits(sc2::Unit::Alliance::Enemy);

	if (enemyunits.size() > 0) {

		if (!already_finishing) {
			already_finishing = true;
			for (const auto& target : enemyunits) {

				for (const auto& unit : units) {
					for (const auto& order : unit->orders) {
						if (!(order.ability_id == sc2::ABILITY_ID::ATTACK_ATTACK)) {
							Actions()->UnitCommand(unit, sc2::ABILITY_ID::ATTACK_ATTACK, target, true);
						}

					}
				}



			}
		}
	}
	else {
		already_finishing = false;
	}

}







bool AnarchiBot::TryBuildBarracks() {
	const sc2::ObservationInterface* observation = Observation();

	if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT) < 1) {
		return false;
	}

	//build 2 barrack for faster unit creation
	if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKS) > 1) {
		return false;
	}

	return TryBuildStructure(sc2::ABILITY_ID::BUILD_BARRACKS);
}

bool AnarchiBot::TryBuildStarport() {
	const sc2::ObservationInterface* observation = Observation();

	if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT) < 1) {
		return false;
	}

	if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_FACTORY) < 1) {
		return false;
	}


	//do not build if already have it
	if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_STARPORT) > 0) {
		return false;
	}

	return TryBuildStructure(sc2::ABILITY_ID::BUILD_STARPORT);
}

bool AnarchiBot::TryUpgradeBarracks() {
	const sc2::ObservationInterface* observation = Observation();

	if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT) < 1) {
		return false;
	}

	//do not updrade if already have it
	if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKSTECHLAB) > 2) {
		return false;
	}

	const sc2::Unit* build = nullptr;
	sc2::Units units = observation->GetUnits(sc2::Unit::Alliance::Self);
	for (const auto& unit : units) {


		if (unit->unit_type == sc2::UNIT_TYPEID::TERRAN_BARRACKS) {
			build = unit;
		}

	}

	if (build) {
		Actions()->UnitCommand(build, sc2::ABILITY_ID::BUILD_TECHLAB, build);
	}

	return true;
}


bool AnarchiBot::TryBuildFactory() {
	const sc2::ObservationInterface* observation = Observation();

	if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT) < 1) {
		return false;
	}

	//do not build if already have it
	if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_FACTORY) > 1) {
		return false;
	}

	return TryBuildStructure(sc2::ABILITY_ID::BUILD_FACTORY);
}

bool AnarchiBot::TryUpgradeFactory() {
	const sc2::ObservationInterface* observation = Observation();

	if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT) < 1) {
		return false;
	}

	//do not updrade if already have it
	if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_FACTORYTECHLAB) > 2) {
		return false;
	}

	const sc2::Unit* build = nullptr;
	sc2::Units units = observation->GetUnits(sc2::Unit::Alliance::Self);
	for (const auto& unit : units) {

		if (unit->unit_type == sc2::UNIT_TYPEID::TERRAN_FACTORY) {
			build = unit;
		}

	}

	if (build) {
		Actions()->UnitCommand(build, sc2::ABILITY_ID::BUILD_TECHLAB, build);
	}

	return true;
}




bool AnarchiBot::TryBuildGeyser() {


	const sc2::ObservationInterface* observation = Observation();

	//if  no supply depot, do not build
	//if already have 2, do not build
	//else get nearest gas patch and build on it
	const sc2::Unit* gas_unit = nullptr;
	if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT) < 1) {
		return false;
	}

	if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_REFINERY) > 1) {
		return false;
	}

	const sc2::Unit* unit_to_build = nullptr;
	const sc2::Unit* base_build = nullptr;
	sc2::Units units = observation->GetUnits(sc2::Unit::Alliance::Self);
	for (const auto& unit : units) {
		for (const auto& order : unit->orders) {
			if (order.ability_id == sc2::ABILITY_ID::BUILD_REFINERY) {
				return false;
			}
		}

		if (unit->unit_type == sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER) {
			base_build = unit;
		}

		if (unit->unit_type == sc2::UNIT_TYPEID::TERRAN_SCV && unit!= scouter) {
			
			unit_to_build = unit;
		}
	}
	if (unit_to_build) {
		gas_unit = FindNearestGasPatch((unit_to_build->pos));
	}
	
	if (gas_unit) {
		if (Query()->Placement(sc2::ABILITY_ID::BUILD_REFINERY, gas_unit->pos)) {
			Actions()->UnitCommand(unit_to_build,
				sc2::ABILITY_ID::BUILD_REFINERY,
				gas_unit);
		}
	}
	return true;


}


bool AnarchiBot::TryBuildSCV() {
	const sc2::ObservationInterface* observation = Observation();

	//try to build scv

	const sc2::Unit* unit_to_build = nullptr;
	sc2::Units units = observation->GetUnits(sc2::Unit::Alliance::Self);
	for (const auto& unit : units) {

		if (unit->unit_type == sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER) {

			unit_to_build = unit;
		}
	}

	if (unit_to_build) {
		if (unit_to_build->assigned_harvesters < unit_to_build->ideal_harvesters) {
			Actions()->UnitCommand(unit_to_build, sc2::ABILITY_ID::TRAIN_SCV);
		}
	}

	return true;


}

const sc2::Unit* AnarchiBot::FindNearestMineralPatch(const sc2::Point2D& start) {
	
	//get vector
	sc2::Units units = Observation()->GetUnits(sc2::Unit::Alliance::Neutral);
	float distance = std::numeric_limits<float>::max();
	const sc2::Unit* target = nullptr;
	for (const auto& u : units) {
		if (u->unit_type == sc2::UNIT_TYPEID::NEUTRAL_MINERALFIELD) {
			float d = DistanceSquared2D(u->pos, start);
			
			//if dustance is less than previous, update
			if (d < distance) {
				distance = d;
				target = u;
			}
		}
	}
	return target;
}

const sc2::Unit* AnarchiBot::FindNearestGasPatch(const sc2::Point2D& start) {

	//get vector of vespene geysers
	sc2::Units units = Observation()->GetUnits(sc2::Unit::Alliance::Neutral, IsVespeneGeyser());
	float distance = std::numeric_limits<float>::max();
	const sc2::Unit* target = nullptr;
	for (const auto& u : units) {
		
		
			
			float d = DistanceSquared2D(u->pos, start);

			//if dustance is less than previous, update
			if (d < distance) {
				distance = d;
				target = u;
			}



	}

	
	return target;
}

bool AnarchiBot::TryCollectGas() {
	


	const sc2::ObservationInterface* observation = Observation();
	sc2::Units units = observation->GetUnits(sc2::Unit::Alliance::Self, IsRefinery());
	sc2::Units units_SCV = observation->GetUnits(sc2::Unit::Alliance::Self);
	sc2::Units collectors;


	const sc2::Unit* target = nullptr;

	int order_count = 0;
	for (const auto& unit : units) {


		//if the refinery does not have the max amount of collectors,
		//then assign more SCVs
		target = unit;
		if (target->assigned_harvesters < target->ideal_harvesters) {
			for (const auto& unit : units_SCV) {


				if (unit->unit_type == sc2::UNIT_TYPEID::TERRAN_SCV && collectors.size() < target->ideal_harvesters - target->assigned_harvesters && unit != scouter) {
					collectors.push_back(unit);
				}
			}
			
			for (const auto& unit : collectors) {

				Actions()->UnitCommand(unit,
					sc2::ABILITY_ID::SMART,
					target);
			}
			
		}

	}


	return true;

}