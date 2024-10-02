// Fill out your copyright notice in the Description page of Project Settings.


#include "LifeGoalGeneral/GladiatorLocalFightComponent.h"
#include "Core/TimeFlowSubsystem.h"
#include "LifeGoalGeneral/GladiatorsSubsystem.h"

UGladiatorLocalFightComponent::UGladiatorLocalFightComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UGladiatorLocalFightComponent::BeginPlay()
{
	Super::BeginPlay();

	GladiatorsSubsystem = GetOwner()->GetGameInstance()->GetSubsystem<UGladiatorsSubsystem>();
	GladiatorsSubsystem->RegisterLocalFightComponent(this);
	
	TimeFlowSystem = GetOwner()->GetGameInstance()->GetSubsystem<UTimeFlowSubsystem>();
}

void UGladiatorLocalFightComponent::SetupNpcFight(FGuid firstGladiatorId, FGuid secondGladiatorId)
{
	TimeFlowSystem->HourPassed.AddDynamic(this, &UGladiatorLocalFightComponent::OnHourPassed);
	NpcFightTimer = GladiatorsSubsystem->Settings->GladiatorFightDuration;
	NpcFightGladiatorIds.Key = firstGladiatorId;
	NpcFightGladiatorIds.Value = secondGladiatorId;
}

void UGladiatorLocalFightComponent::OnHourPassed(int hour)
{
	NpcFightTimer--;
	if (NpcFightTimer <= 0)
	{
		HandleNpcFightEnded();
	}
}

void UGladiatorLocalFightComponent::HandleNpcFightEnded()
{
	TimeFlowSystem->HourPassed.RemoveDynamic(this, &UGladiatorLocalFightComponent::OnHourPassed);
	GladiatorsSubsystem->PerformLocalGladiatorBattle(NpcFightGladiatorIds.Key, NpcFightGladiatorIds.Value);
	ClearSpawnedNpcs();
}
