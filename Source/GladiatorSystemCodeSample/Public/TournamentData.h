// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GladiatorData.h"
#include "TournamentData.generated.h"

USTRUCT(BlueprintType)
struct THEEMPIRE_API FTournamentBattleResult
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString OpponentName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGladiatorBattleResult Result;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int GoldEarned;
};

USTRUCT(BlueprintType)
struct THEEMPIRE_API FTournamentGladiatorData
{
	GENERATED_BODY()

	FTournamentGladiatorData();
	FTournamentGladiatorData(FGuid gladiatorId);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGuid GladiatorId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGladiatorData OpponentData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EArenaModifier ArenaModifier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bOpponentRevealed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bOpponentRolled;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FTournamentBattleResult> PastBattles;

public:
	int GetWonBattles() const;
	bool HasOnlyWins() const;
	bool IsAlive() const;
};

USTRUCT(BlueprintType)
struct THEEMPIRE_API FTournamentData
{
	GENERATED_BODY()

	FTournamentData();
	FTournamentData(FGuid officerId, const TArray<FGuid>& sentGladiators, int sentGold);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ERomeTournamentState State;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGuid DelegatedOfficer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FTournamentGladiatorData> Gladiators;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Gold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FGuid> PardonedGladiators;

public:
	bool HasAliveGladiators() const;
};
