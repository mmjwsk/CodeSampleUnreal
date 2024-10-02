// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GladiatorBattleData.h"
#include "GladiatorsSettings.h"
#include "UObject/NoExportTypes.h"
#include "GladiatorBattleComponent.generated.h"

UCLASS()
class THEEMPIRE_API UGladiatorBattleComponent : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(UGladiatorsSettings* settings);

public:
	FGladiatorBattleData BattleData;

private:
	TObjectPtr<UGladiatorsSettings> Settings;
	TTuple<int, int> WonRoundCounter;
	TTuple<int, int> HealthCounter;
	TTuple<int, int> ChargeCounter;

public:
	void SimulateBattle(const FGladiatorData& firstCombatant, const FGladiatorData& secondCombatant,
		EArenaModifier arenaModifier);

private:
	void SimulateBattleRound();
	void SimulateBattleTurn();
	int GetSpeed(FGladiatorData& data) const;
	float GetDamage(FGladiatorData& data) const;
	float GetDefense(FGladiatorData& data) const;
	bool BerserkerTriggeredLastTurn(const FGladiatorData& data) const;
	FGladiatorBattleAction MakeBattleAction(int& attackerCharge, FGladiatorData& attacker, int& attackerHealth,
		int& defenderCharge, FGladiatorData& defender, int& defenderHealth);
	void MakeFirstCombatantAction(FGladiatorBattleTurn& turn);
	void MakeSecondCombatantAction(FGladiatorBattleTurn& turn);
	bool AreCombatantsAlive() const;
};
