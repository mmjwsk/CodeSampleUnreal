// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GladiatorBattleData.h"
#include "GladiatorsSettings.h"
#include "UObject/NoExportTypes.h"
#include "GladiatorBattleSummaryGenerator.generated.h"

UCLASS()
class THEEMPIRE_API UGladiatorBattleSummaryGenerator : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(UGladiatorsSettings* settings);

public:
	FString Summary;

private:
	TObjectPtr<UGladiatorsSettings> Settings;
	FString FirstCombatantName;
	FString SecondCombatantName;

public:
	void GenerateSummary(const FGladiatorBattleData& battle);

private:
	void GenerateRound(const FGladiatorBattleRound& round);
	void GenerateTurn(const FGladiatorBattleTurn& turn);
	void GenerateAction(const FGladiatorBattleAction& action, const FString& attackerName);
	void GenerateMasteryLines(const TArray<EGladiatorMastery>& triggeredMasteries);
	void AddLine(const FString& line);
	void AddPhrase(const FString& phrase);
};
