// Fill out your copyright notice in the Description page of Project Settings.


#include "LifeGoalGeneral/GladiatorBattleData.h"

FGladiatorBattleAction::FGladiatorBattleAction()
{
	bAttack = false;
	Damage = 0;
	AttackerTriggeredMasteries = TArray<EGladiatorMastery>();
	DefenderTriggeredMasteries = TArray<EGladiatorMastery>();
}

FGladiatorBattleAction::FGladiatorBattleAction(int damage, TArray<EGladiatorMastery> attackerTriggeredMasteries,
	TArray<EGladiatorMastery> defenderTriggeredMasteries)
{
	bAttack = true;
	Damage = damage;
	AttackerTriggeredMasteries = attackerTriggeredMasteries;
	DefenderTriggeredMasteries = defenderTriggeredMasteries;
}

FGladiatorBattleData::FGladiatorBattleData()
{
	FirstCombatantData = FGladiatorData();
	SecondCombatantData = FGladiatorData();
	ArenaModifier = EArenaModifier::None;
	Rounds = TArray<FGladiatorBattleRound>();
	bFirstCombatantWon = false;
}

FGladiatorBattleData::FGladiatorBattleData(const FGladiatorData& firstCombatant, const FGladiatorData& secondCombatant)
{
	FirstCombatantData = firstCombatant;
	SecondCombatantData = secondCombatant;
	ArenaModifier = EArenaModifier::None;
	Rounds = TArray<FGladiatorBattleRound>();
	bFirstCombatantWon = false;
}

FString FGladiatorBattleDescriptionLines::GetRandomLine() const
{
	return Lines[FMath::RandHelper(Lines.Num())];
}
