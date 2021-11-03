#include "BasicSc2Bot.h"



void BasicSc2Bot::OnGameStart() {
	std::cout << "GO Anarchibot" << std::endl;
}

void BasicSc2Bot::OnStep() {

	TryBuildSupplyDepot();

	TryBuildBarracks();

	TryBuildFactory();

	TryBuildGeyser();

	TryCollectGas();

}


void BasicSc2Bot::OnUnitIdle(const sc2::Unit* unit) {
	const sc2::ObservationInterface* observation = Observation();
	
	switch (unit->unit_type.ToType()) {
	case sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER: {

		if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_SCV) < 15) {
			Actions()->UnitCommand(unit, sc2::ABILITY_ID::TRAIN_SCV);
		}
		break;
	}
	case sc2::UNIT_TYPEID::TERRAN_SCV: {
		const sc2::Unit* mineral_target = FindNearestMineralPatch(unit->pos);
		if (!mineral_target) {
			break;
		}
		
		Actions()->UnitCommand(unit, sc2::ABILITY_ID::SMART, mineral_target);
		break;


	}
	case sc2::UNIT_TYPEID::TERRAN_BARRACKS: {


		Actions()->UnitCommand(unit, sc2::ABILITY_ID::TRAIN_MARAUDER);
		Actions()->UnitCommand(unit, sc2::ABILITY_ID::TRAIN_MARINE);
	



		break;
	}
	case sc2::UNIT_TYPEID::TERRAN_FACTORY: {


		Actions()->UnitCommand(unit, sc2::ABILITY_ID::TRAIN_HELLION);




		break;
	}
	case sc2::UNIT_TYPEID::TERRAN_MARINE: {
		const sc2::GameInfo& game_info = Observation()->GetGameInfo();
		if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_MARINE) > 9 && CountUnitType(sc2::UNIT_TYPEID::TERRAN_HELLION) > 3) {

			sc2::Units units = observation->GetUnits(sc2::Unit::Alliance::Self, IsAttacker());

			Actions()->UnitCommand(units, sc2::ABILITY_ID::ATTACK_ATTACK, game_info.enemy_start_locations.front());
			break;
		}
	}
	case sc2::UNIT_TYPEID::TERRAN_HELLION: {
		const sc2::GameInfo& game_info = Observation()->GetGameInfo();
		if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_MARINE) > 9 && CountUnitType(sc2::UNIT_TYPEID::TERRAN_HELLION) > 3) {

			sc2::Units units = observation->GetUnits(sc2::Unit::Alliance::Self, IsAttacker());

			Actions()->UnitCommand(units, sc2::ABILITY_ID::ATTACK_ATTACK, game_info.enemy_start_locations.front());
			break;
		}
	}
	case sc2::UNIT_TYPEID::TERRAN_MARAUDER: {
		const sc2::GameInfo& game_info = Observation()->GetGameInfo();
		if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_MARINE) > 7 && CountUnitType(sc2::UNIT_TYPEID::TERRAN_MARAUDER) > 1) {
			Actions()->UnitCommand(unit, sc2::ABILITY_ID::ATTACK_ATTACK, game_info.enemy_start_locations.front());
			break;
		}
	}
	default: {
		break;

	}
	}
}

bool BasicSc2Bot::TryBuildSupplyDepot() {

	const sc2::ObservationInterface* observation = Observation();

	// If we are not supply capped, don't build a supply depot
	if (observation->GetFoodUsed() <= observation->GetFoodCap() - 2)
		return false;
	// Try and build a depot. Find a random SCV and give it the order.
	return TryBuildStructure(sc2::ABILITY_ID::BUILD_SUPPLYDEPOT);

}

bool BasicSc2Bot::TryBuildStructure(sc2::ABILITY_ID ability_type_for_structure, sc2::UNIT_TYPEID unit_type) {
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

	Actions()->UnitCommand(unit_to_build,
		ability_type_for_structure,
		sc2::Point2D(unit_to_build->pos.x + rx * 15.0f, unit_to_build->pos.y + ry * 15.0f));

	return true;
}




bool BasicSc2Bot::TryBuildBarracks() {
	const sc2::ObservationInterface* observation = Observation();

	if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT) < 1) {
		return false;
	}

	//build 1 barrack for faster unit creation
	if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKS) > 1) {
		return false;
	}

	return TryBuildStructure(sc2::ABILITY_ID::BUILD_BARRACKS);
}


bool BasicSc2Bot::TryBuildFactory() {
	const sc2::ObservationInterface* observation = Observation();

	if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT) < 1) {
		return false;
	}

	//build 1 barrack for faster unit creation
	if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_FACTORY) > 1) {
		return false;
	}

	return TryBuildStructure(sc2::ABILITY_ID::BUILD_FACTORY);
}


bool BasicSc2Bot::TryBuildGeyser() {
	const sc2::ObservationInterface* observation = Observation();

	if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT) < 1) {
		return false;
	}

	//build 1 barrack for faster unit creation
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

		if (unit->unit_type == sc2::UNIT_TYPEID::TERRAN_SCV) {
			unit_to_build = unit;
		}
	}

	const sc2::Unit* gas_unit = FindNearestGasPatch((unit_to_build->pos));
	
	

	if (Query()->Placement(sc2::ABILITY_ID::BUILD_REFINERY, gas_unit->pos)) {
		Actions()->UnitCommand(unit_to_build,
			sc2::ABILITY_ID::BUILD_REFINERY,
			gas_unit);
	}
	return true;


}


const sc2::Unit* BasicSc2Bot::FindNearestMineralPatch(const sc2::Point2D& start) {
	sc2::Units units = Observation()->GetUnits(sc2::Unit::Alliance::Neutral);
	float distance = std::numeric_limits<float>::max();
	const sc2::Unit* target = nullptr;
	for (const auto& u : units) {
		if (u->unit_type == sc2::UNIT_TYPEID::NEUTRAL_MINERALFIELD) {
			float d = DistanceSquared2D(u->pos, start);
			if (d < distance) {
				distance = d;
				target = u;
			}
		}
	}
	return target;
}

const sc2::Unit* BasicSc2Bot::FindNearestGasPatch(const sc2::Point2D& start) {
	sc2::Units units = Observation()->GetUnits(sc2::Unit::Alliance::Neutral, IsVespeneGeyser());
	float distance = std::numeric_limits<float>::max();
	const sc2::Unit* target = nullptr;
	for (const auto& u : units) {
		
		

			float d = DistanceSquared2D(u->pos, start);

			if (d < distance) {
				distance = d;
				target = u;
			}



	}

	
	return target;
}

void BasicSc2Bot::TryCollectGas() {
	
	if (gas_count >= 3) {
		return;
	}

	const sc2::ObservationInterface* observation = Observation();
	sc2::Units units = observation->GetUnits(sc2::Unit::Alliance::Self);

	sc2::Units collectors;

	
	const sc2::Unit* target = nullptr;


	for (const auto& unit : units) {

		if (unit->unit_type == sc2::UNIT_TYPEID::TERRAN_SCV && gas_count < 3) {
			collectors.push_back(unit);
			gas_count++;
		}
		if (unit->unit_type == sc2::UNIT_TYPEID::TERRAN_REFINERY) {
			target = unit;
		}
	}

	for (const auto& unit : collectors) {

		Actions()->UnitCommand(unit,
			sc2::ABILITY_ID::SMART,
			target);
	}



}