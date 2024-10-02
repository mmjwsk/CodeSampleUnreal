// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GladiatorData.h"
#include "TournamentData.h"
#include "Components/ActorComponent.h"
#include "GladiatorsSaveLoadComponent.generated.h"


UCLASS(Abstract, Blueprintable, ClassGroup = (SaveLoad), meta=(BlueprintSpawnableComponent))
class THEEMPIRE_API UGladiatorsSaveLoadComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGladiatorsSaveLoadComponent();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, SaveGame, Replicated, Category = "Artractive")
	TArray<FGladiatorData> Gladiators;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, SaveGame, Replicated, Category = "Artractive")
	TArray<FGladiatorInitiateData> Initiates;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, SaveGame, Replicated, Category = "Artractive")
	TArray<FGuid> Candidates;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, SaveGame, Replicated, Category = "Artractive")
	FTournamentData TournamentData;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, SaveGame, Replicated, Category = "Artractive")
	FString LastLocalFightLog;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, SaveGame, Replicated, Category = "Artractive")
	int LocalFightCooldown;

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
