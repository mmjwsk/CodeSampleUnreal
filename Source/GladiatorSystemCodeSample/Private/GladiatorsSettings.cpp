// Fill out your copyright notice in the Description page of Project Settings.


#include "LifeGoalGeneral/GladiatorsSettings.h"

int UGladiatorsSettings::RollActiveMasteriesCount(int points) const
{
	for (const auto& rangePair : ActiveMasteriesPerThreshold)
	{
		if (points >= rangePair.Key)
		{
			return FMath::RandRange(rangePair.Value.Min, rangePair.Value.Max);
		}
	}

	return 0;
}

FString UGladiatorsSettings::RollGladiatorName() const
{
	return EnemyGladiatorFirstNames.GetRandomLine() + " " + EnemyGladiatorSecondNames.GetRandomLine();
}
