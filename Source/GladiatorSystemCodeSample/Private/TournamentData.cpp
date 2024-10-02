// Fill out your copyright notice in the Description page of Project Settings.


#include "LifeGoalGeneral/TournamentData.h"

FTournamentGladiatorData::FTournamentGladiatorData()
{
	GladiatorId = FGuid();
	OpponentData = FGladiatorData();
	ArenaModifier = EArenaModifier::None;
	bOpponentRevealed = false;
	bOpponentRolled = false;
	PastBattles = TArray<FTournamentBattleResult>();
}

FTournamentGladiatorData::FTournamentGladiatorData(FGuid gladiatorId) : FTournamentGladiatorData()
{
	GladiatorId = gladiatorId;
}

int FTournamentGladiatorData::GetWonBattles() const
{
	int wonBattles = 0;
	for (const auto& battle : PastBattles)
	{
		if (battle.Result == EGladiatorBattleResult::Victory)
		{
			wonBattles++;
		}
	}
	return wonBattles;
}

bool FTournamentGladiatorData::HasOnlyWins() const
{
	for (const auto& battle : PastBattles)
	{
		if (battle.Result != EGladiatorBattleResult::Victory)
		{
			return false;
		}
	}
	return true;
}

bool FTournamentGladiatorData::IsAlive() const
{
	if (PastBattles.IsEmpty()) return true;
	
	return PastBattles.Last().Result != EGladiatorBattleResult::Death;
}

FTournamentData::FTournamentData()
{
	State = ERomeTournamentState::Unavailable;
	DelegatedOfficer = FGuid();
	Gladiators = TArray<FTournamentGladiatorData>();
	Gold = 0;
	PardonedGladiators = TArray<FGuid>();
}

FTournamentData::FTournamentData(FGuid officerId, const TArray<FGuid>& sentGladiators, int sentGold)
{
	State = ERomeTournamentState::InProgress;
	DelegatedOfficer = officerId;
	
	Gladiators = TArray<FTournamentGladiatorData>();
	for (const auto& id : sentGladiators)
	{
		Gladiators.Add(FTournamentGladiatorData(id));
	}
	
	Gold = sentGold;
	PardonedGladiators = TArray<FGuid>();
}

bool FTournamentData::HasAliveGladiators() const
{
	for (const auto& gladiator : Gladiators)
	{
		if (gladiator.IsAlive())
		{
			return true;
		}
	}
	
	return false;
}
