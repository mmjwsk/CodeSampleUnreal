// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GladiatorLocalFightComponent.generated.h"

class UTimeFlowSubsystem;
class UGladiatorsSubsystem;

UCLASS(Abstract, BlueprintType, Blueprintable, ClassGroup=(LifeGoal), meta=(BlueprintSpawnableComponent) )
class THEEMPIRE_API UGladiatorLocalFightComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UGladiatorLocalFightComponent();

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Artractive")
	TObjectPtr<UGladiatorsSubsystem> GladiatorsSubsystem;

private:
	TObjectPtr<UTimeFlowSubsystem> TimeFlowSystem;
	int NpcFightTimer;
	TPair<FGuid, FGuid> NpcFightGladiatorIds;

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Artractive")
	void StartFightForPlayerAndGladiator(FGuid gladiatorId);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Artractive")
	void StartFightForTwoGladiators(FGuid firstGladiatorId, FGuid secondGladiatorId);

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Artractive")
	void ClearSpawnedNpcs();
	
	UFUNCTION(BlueprintCallable, Category = "Artractive")
	void SetupNpcFight(FGuid firstGladiatorId, FGuid secondGladiatorId);

private:
	UFUNCTION()
	void OnHourPassed(int hour);

	void HandleNpcFightEnded();
};
