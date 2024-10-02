// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GladiatorData.h"
#include "GladiatorBattleData.generated.h"

USTRUCT(BlueprintType)
struct THEEMPIRE_API FGladiatorBattleAction
{
	GENERATED_BODY()

public:
	FGladiatorBattleAction();
	FGladiatorBattleAction(int damage, TArray<EGladiatorMastery> attackerTriggeredMasteries,
		TArray<EGladiatorMastery> defenderTriggeredMasteries);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<EGladiatorMastery> AttackerTriggeredMasteries;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<EGladiatorMastery> DefenderTriggeredMasteries;
};

USTRUCT(BlueprintType)
struct THEEMPIRE_API FGladiatorBattleTurn
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGladiatorBattleAction FirstCombatantAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGladiatorBattleAction SecondCombatantAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bFirstCombatantWentFirst;
};

USTRUCT(BlueprintType)
struct THEEMPIRE_API FGladiatorBattleRound
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FGladiatorBattleTurn> Turns;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bFirstCombatantWon;
};


USTRUCT(BlueprintType)
struct THEEMPIRE_API FGladiatorBattleData
{
	GENERATED_BODY()

public:
	FGladiatorBattleData();
	FGladiatorBattleData(const FGladiatorData& firstCombatant, const FGladiatorData& secondCombatant);

public:
	// In player gladiator vs AI gladiator, player gladiator should always be considered first combatant
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGladiatorData FirstCombatantData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGladiatorData SecondCombatantData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EArenaModifier ArenaModifier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FGladiatorBattleRound> Rounds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bFirstCombatantWon;
};

USTRUCT(BlueprintType)
struct THEEMPIRE_API FGladiatorBattleDescriptionLines
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> Lines;

public:
	FString GetRandomLine() const;
};
