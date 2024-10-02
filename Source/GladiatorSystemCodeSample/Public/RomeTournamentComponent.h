// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GladiatorsSubsystem.h"
#include "TournamentData.h"
#include "Core/TimeRelatedEnums.h"
#include "Advisors/AdvisorsSubsystem.h"
#include "RomeTournamentComponent.generated.h"

UCLASS()
class THEEMPIRE_API URomeTournamentComponent : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(UGladiatorsSubsystem* gladiatorsSubsystem, UGameInstance* gameInstance,
		UGladiatorBattleComponent* battleComponent, UGladiatorBattleSummaryGenerator* summaryGenerator,
		UAdvisorsSubsystem* advisorsSubsystem);

private:
	TObjectPtr<UGladiatorsSubsystem> GladiatorsSystem;
	TObjectPtr<USkillsSubsystem> SkillsSubsystem;
	TObjectPtr<UDynastySubsystem> DynastySubsystem;
	TObjectPtr<UGladiatorsSettings> Settings;
	TObjectPtr<UGladiatorBattleComponent> BattleComponent;
	TObjectPtr<UGladiatorBattleSummaryGenerator> SummaryGenerator;
	TObjectPtr<UAdvisorsSubsystem> AdvisorsSubsystem;
	bool bShouldHandleTournamentEnd;

public:
	void InitializeTournament(FGuid officer, TArray<FGuid> gladiators, int startingGold);
	TArray<FGuid> GetDeadGladiators() const;
	
private:
	UFUNCTION()
	void OnHourPassed(int hour);
	UFUNCTION()
	void OnSeasonPassed(ESeason currentSeason);

	FTournamentData* GetTournamentData() const;
	void ProgressTournament();
	void RollUpcomingBattles();
	void FightBattles();
	void EndTournament();
	void GenerateOpponent(FTournamentGladiatorData& gladiadorData);
	EArenaModifier RollArenaModifier() const;
	int GetPointsValue(FGuid gladiatorId) const;
	void GenerateMasteries(FGladiatorData& data, int masteriesCount);
	bool ShouldLoserDie(const FGladiatorData& loserData, EArenaModifier modifier) const;
	int CalculateGoldReward(FGladiatorData& data, bool wasVictory, EArenaModifier modifier, int previousWins) const;
};
