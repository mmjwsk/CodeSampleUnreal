// Fill out your copyright notice in the Description page of Project Settings.


#include "LifeGoalGeneral/GladiatorsSaveLoadComponent.h"
#include "Net/UnrealNetwork.h"

UGladiatorsSaveLoadComponent::UGladiatorsSaveLoadComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UGladiatorsSaveLoadComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UGladiatorsSaveLoadComponent, Gladiators);
	DOREPLIFETIME(UGladiatorsSaveLoadComponent, Initiates);
	DOREPLIFETIME(UGladiatorsSaveLoadComponent, Candidates);
	DOREPLIFETIME(UGladiatorsSaveLoadComponent, TournamentData);
	DOREPLIFETIME(UGladiatorsSaveLoadComponent, LastLocalFightLog);
	DOREPLIFETIME(UGladiatorsSaveLoadComponent, LocalFightCooldown);
}


