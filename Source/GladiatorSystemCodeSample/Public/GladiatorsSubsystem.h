// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GladiatorData.h"
#include "GladiatorsSaveLoadComponent.h"
#include "GladiatorsSettings.h"
#include "TournamentData.h"
#include "RpgStats/RpgStatEnums.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GladiatorsSubsystem.generated.h"

class UDynastySubsystem;
class UGladiatorLocalFightComponent;
class UGladiatorBattleSummaryGenerator;
class UGladiatorBattleComponent;
class URomeTournamentComponent;
class USkillsSubsystem;
class UNPCDataSubsystem;

UCLASS(Abstract, Blueprintable)
class THEEMPIRE_API UGladiatorsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UGladiatorsSubsystem() {}

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	void OnGameInstanceStarted(UGameInstance* gameInstance);

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Artractive")
	TObjectPtr<UGladiatorsSaveLoadComponent> SaveLoadComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive")
	TObjectPtr<UGladiatorsSettings> Settings;

	UPROPERTY(BlueprintReadOnly, Category = "Artractive")
	TObjectPtr<UGladiatorLocalFightComponent> LocalFightComponent;

private:
	TObjectPtr<UNPCDataSubsystem> NpcSubsystem;
	TObjectPtr<USkillsSubsystem> SkillsSubsystem;
	TObjectPtr<UDynastySubsystem> DynastySubsystem;

	UPROPERTY()
	TObjectPtr<UGladiatorBattleComponent> BattleComponent;

	UPROPERTY()
	TObjectPtr<UGladiatorBattleSummaryGenerator> BattleSummaryGenerator;

	UPROPERTY()
	TObjectPtr<URomeTournamentComponent> TournamentComponent;
	
public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Artractive|Tournament")
	void ShowTournamentEvent();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Artractive|Tournament")
	const FTournamentData& GetTournamentData();

	// Kill gladiators that died, pardon those that got pardoned, earn the gold, make officer available again
	UFUNCTION(BlueprintCallable, Category = "Artractive|Tournament")
	void ApplyTournamentResults();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Artractive|Tournament")
	void GetTournamentResultsData(TArray<FString>& deadGladiatorNames, TArray<FString>& pardonedGladiatorNames, int& gold);
	
	UFUNCTION(BlueprintCallable, Category = "Artractive|Tournament")
	void StartTournament(FGuid officer, TArray<FGuid> sentGladiators, int startingGold);
	
	UFUNCTION(BlueprintCallable, Category = "Artractive|Tournament")
	void RevealOpponent(FGuid gladiatorId);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Artractive|Tournament")
	int GetRevealCost() const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Artractive")
	const FGladiatorData& GetGladiatorDataById(FGuid gladiatorId) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Artractive")
	TArray<FGladiatorData> GetAvailableGladiators() const;
	
	UFUNCTION(BlueprintCallable, Category = "Artractive|Training")
	void TurnCandidateIntoInitiate(FGuid candidateId, FGuid officerId);

	UFUNCTION(BlueprintCallable, Category = "Artractive|Training")
	void StopInitiateTraining(FGuid initiateId);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Artractive|Training")
	bool TryGetInitiateById(FGladiatorInitiateData& initiateData, FGuid initiateId);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Artractive|Training")
	int GetHoursToTrainInitiate(FGuid trainingOfficerId) const;

	UFUNCTION(BlueprintCallable, Category = "Artractive|Stats")
	void ApplyGladiatorModifications(FGuid gladiatorId, FText newName, TMap<EGladiatorStat, int> statDeltas,
		TArray<EGladiatorMastery> newMasteries, TArray<EGladiatorMastery> newActiveMasteries, int remainingXp);

	UFUNCTION(BlueprintCallable, Category = "Artractive|Stats")
	void GrantXp(FGuid gladiatorId, int amount);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Artractive|Stats")
	int GetMasterySlotCount(FGuid gladiatorId);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Artractive|Stats")
	int GetGladiatorStatValue(FGuid gladiatorId, EGladiatorStat stat);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Artractive|Stats")
	TArray<EGladiatorMastery> GetLearnableMasteriesForStat(FGuid gladiatorId, EGladiatorStat stat);

	UFUNCTION(BlueprintCallable, Category = "Artractive|Debug")
	void DEBUG_GenerateNewCandidate();

	FGladiatorData* GetGladiatorData(FGuid gladiatorId);
	void RegisterLocalFightComponent(UGladiatorLocalFightComponent* component);
	void PerformLocalGladiatorBattle(FGuid firstGladiatorId, FGuid secondGladiatorId);
	
private:
	UFUNCTION()
	void OnHourPassed(int hour);

	UFUNCTION()
	void OnDayPassed(int day);

	void TurnInitiateIntoGladiator(const FGladiatorInitiateData& initiateData);
	float GetGladiatorInitialSkillPoints(FGuid npcId, ESkill skill) const;
	void PardonGladiator(FGuid gladiatorId);
	void GrantPardonSkills(FGuid gladiatorId);
	void RemoveGladiator(FGuid gladiatorId);
	void AddRandomMasteryForStat(FGladiatorData& data, EGladiatorStat stat, int statThreshold);
};
