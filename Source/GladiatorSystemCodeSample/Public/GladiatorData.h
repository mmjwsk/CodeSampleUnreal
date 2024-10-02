// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LifeGoalGeneralEnums.h"
#include "UObject/NoExportTypes.h"
#include "Utils/DataWithIdBase.h"
#include "GladiatorData.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(GladiatorWarning, Warning, Warning);

USTRUCT(BlueprintType)
struct THEEMPIRE_API FGladiatorStat
{
	GENERATED_BODY()

public:
	FGladiatorStat();
	FGladiatorStat(EGladiatorStat type);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGladiatorStat Type;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Value;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Investment;

public:
	FGladiatorStat operator++(int);
	FGladiatorStat operator+=(int amount);
	FGladiatorStat operator--(int);
	FGladiatorStat operator-=(int amount);
};

USTRUCT(BlueprintType)
struct THEEMPIRE_API FGladiatorData : public FDataWithIdBase
{
	GENERATED_BODY()

public:
	FGladiatorData();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Artractive")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Artractive|Stats")
	TArray<FGladiatorStat> Stats;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Artractive|Stats")
	int UnusedXp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Artractive|Masteries")
	TArray<EGladiatorMastery> KnownMasteries;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Artractive|Masteries")
	TArray<EGladiatorMastery> ActiveMasteries;

public:
	FGladiatorStat* GetStat(EGladiatorStat statType);
};

USTRUCT(BlueprintType)
struct THEEMPIRE_API FGladiatorInitiateData : public FDataWithIdBase
{
	GENERATED_BODY()

public:
	FGladiatorInitiateData();
	FGladiatorInitiateData(FGuid npcId, int hoursTillInitiation, FGuid officerId);

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int HoursTillInitiation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGuid OfficerId;
};

