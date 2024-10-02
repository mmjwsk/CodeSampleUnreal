// Fill out your copyright notice in the Description page of Project Settings.


#include "LifeGoalGeneral/RomeTournamentComponent.h"
#include "Core/TimeFlowSubsystem.h"
#include "LifeGoalGeneral/GladiatorBattleComponent.h"
#include "LifeGoalGeneral/GladiatorBattleSummaryGenerator.h"
#include "RpgStats/SkillsSubsystem.h"
#include "RpgStats/Dynasty/DynastySettings.h"
#include "RpgStats/Dynasty/DynastySubsystem.h"

void URomeTournamentComponent::Initialize(UGladiatorsSubsystem* gladiatorsSubsystem, UGameInstance* gameInstance,
	UGladiatorBattleComponent* battleComponent, UGladiatorBattleSummaryGenerator* summaryGenerator,
	UAdvisorsSubsystem* advisorSubsystem)
{
	GladiatorsSystem = gladiatorsSubsystem;
	Settings = GladiatorsSystem->Settings;
	BattleComponent = battleComponent;
	SummaryGenerator = summaryGenerator;
	AdvisorsSubsystem = advisorSubsystem;
	SkillsSubsystem = gameInstance->GetSubsystem<USkillsSubsystem>();
	DynastySubsystem = gameInstance->GetSubsystem<UDynastySubsystem>();

	auto* timeFlowSystem = gameInstance->GetSubsystem<UTimeFlowSubsystem>();
	timeFlowSystem->HourPassed.AddDynamic(this, &URomeTournamentComponent::OnHourPassed);
	timeFlowSystem->SeasonPassed.AddDynamic(this, &URomeTournamentComponent::OnSeasonPassed);
}

void URomeTournamentComponent::InitializeTournament(FGuid officer, TArray<FGuid> gladiators, int startingGold)
{
	GladiatorsSystem->SaveLoadComponent->TournamentData = FTournamentData(officer, gladiators, startingGold);
}

TArray<FGuid> URomeTournamentComponent::GetDeadGladiators() const
{
	TArray<FGuid> deadGladiators = TArray<FGuid>();
	for (const auto& gladiator : GetTournamentData()->Gladiators)
	{
		if (!gladiator.IsAlive())
		{
			deadGladiators.Add(gladiator.GladiatorId);
		}
	}
	
	return deadGladiators;
}

void URomeTournamentComponent::OnHourPassed(int hour)
{
	if (GetTournamentData()->State == ERomeTournamentState::InProgress && hour == Settings->HourToShowTournamentReports)
	{
		if (bShouldHandleTournamentEnd)
		{
			FightBattles();
			EndTournament();
		}
		else
		{
			ProgressTournament();
		}

		GladiatorsSystem->ShowTournamentEvent();
	}
}

void URomeTournamentComponent::OnSeasonPassed(ESeason currentSeason)
{
	if (GetTournamentData()->State == ERomeTournamentState::InProgress)
	{
		bShouldHandleTournamentEnd = true;
	}
	else if (currentSeason == ESeason::Summer)
	{
		GetTournamentData()->State = ERomeTournamentState::Available;
	}
}

FTournamentData* URomeTournamentComponent::GetTournamentData() const
{
	return &GladiatorsSystem->SaveLoadComponent->TournamentData;
}

void URomeTournamentComponent::ProgressTournament()
{
	FightBattles();

	if (GetTournamentData()->HasAliveGladiators())
	{
		RollUpcomingBattles();
	}
	else
	{
		EndTournament();
	}
}

void URomeTournamentComponent::RollUpcomingBattles()
{
	for (auto& gladiator : GetTournamentData()->Gladiators)
	{
		if (!gladiator.IsAlive()) continue;

		GenerateOpponent(gladiator);
		// TODO #17 - add chance from TournamentData.DelegatedOfficer's leadership skill value (as percent)
		const float chanceToReveal = DynastySubsystem->GetFloatValueBonus(
			DynastySubsystem->Settings->LeadersInsightLearningEnemyGladiatorsChanceAndCostReductionPercentages, EDynastyTalent::LeadersInsight)
			+ (AdvisorsSubsystem->GetAdvisorWorkerSkill(GetTournamentData()->DelegatedOfficer, ESkill::Leadership) / 100);
 		gladiator.bOpponentRevealed = FMath::FRand() <= chanceToReveal;
		gladiator.bOpponentRolled = true;
		gladiator.ArenaModifier = RollArenaModifier();
	}
}

void URomeTournamentComponent::FightBattles()
{
	for (auto& gladiator : GetTournamentData()->Gladiators)
	{
		if (!gladiator.bOpponentRolled || !gladiator.IsAlive()) continue;

		FGladiatorData* gladiatorData = GladiatorsSystem->GetGladiatorData(gladiator.GladiatorId);
		BattleComponent->SimulateBattle(*gladiatorData, gladiator.OpponentData, gladiator.ArenaModifier);
		SummaryGenerator->GenerateSummary(BattleComponent->BattleData);
		bool wasVictory = BattleComponent->BattleData.bFirstCombatantWon;

		FTournamentBattleResult result = FTournamentBattleResult();
		result.OpponentName = BattleComponent->BattleData.SecondCombatantData.Name;
		result.Result = wasVictory
			? EGladiatorBattleResult::Victory
			: ShouldLoserDie(*gladiatorData, gladiator.ArenaModifier)
			? EGladiatorBattleResult::Death
			: EGladiatorBattleResult::Loss;

		result.Description = SummaryGenerator->Summary;

		int goldEarned = CalculateGoldReward(*gladiatorData, wasVictory,
			gladiator.ArenaModifier, gladiator.GetWonBattles());
		result.GoldEarned = goldEarned;
		GetTournamentData()->Gold += goldEarned;

		if (wasVictory)
		{
			GladiatorsSystem->GrantXp(gladiator.GladiatorId, Settings->XpEarnedForWonTournamentBattle);
		}
		else if (result.Result == EGladiatorBattleResult::Death)
		{
			gladiator.bOpponentRolled = false;
		}
		gladiator.PastBattles.Add(result);
	}
}

void URomeTournamentComponent::EndTournament()
{
	GetTournamentData()->State = ERomeTournamentState::Finished;

	for (const auto& gladiator : GetTournamentData()->Gladiators)
	{
		if (!gladiator.HasOnlyWins()) continue;

		float pardonChance = Settings->BaseChanceForPardon + Settings->CumulativeChanceForPardon * gladiator.GetWonBattles();
		if (FMath::FRand() <= pardonChance)
		{
			GetTournamentData()->PardonedGladiators.Add(gladiator.GladiatorId);
		}
	}
}

void URomeTournamentComponent::GenerateOpponent(FTournamentGladiatorData& gladiadorData)
{
	int points = GetPointsValue(gladiadorData.GladiatorId);
	points += FMath::RandRange(-Settings->PointValueMaxRandomDrift, Settings->PointValueMaxRandomDrift);
	points += gladiadorData.GetWonBattles() * Settings->PointsPerTournamentProgress;

	FGladiatorData opponent = FGladiatorData();
	opponent.Name = Settings->RollGladiatorName();

	const int masteryCount = Settings->RollActiveMasteriesCount(points);
	points -= masteryCount * Settings->PointCostPerOpponentActiveMastery;
	GenerateMasteries(opponent, masteryCount);

	// Rolling tactics the special way because it's tied to active masteries and we want enemy gladiators to work by the rules too
	int minTactics = FMath::Max(0, (masteryCount - Settings->BasicMasterySlotCount) * Settings->TacticsPerAdditionalMasterySlot);
	int tacticsValue = FMath::RandRange(minTactics, Settings->GladiatorStatCaps[EGladiatorStat::Tactics]);
	opponent.GetStat(EGladiatorStat::Tactics)->Value = tacticsValue;
	points -= tacticsValue;

	TMap<EGladiatorStat, int> statValues = TMap<EGladiatorStat, int>();
	float statSum = 0;
	for (const EGladiatorStat stat : TEnumRange<EGladiatorStat>())
	{
		if (stat == EGladiatorStat::Tactics) continue;

		int minStatValue = stat == EGladiatorStat::Health ? Settings->GladiatorHealthBaseValue : 0;
		int value = FMath::RandRange(minStatValue, Settings->GladiatorStatCaps[stat]);
		statValues.Add(stat, value);
		statSum += value;
	}

	for (const auto& statPair : statValues)
	{
		int scaledValue = FMath::RoundToInt(statPair.Value / statSum * points);
		opponent.GetStat(statPair.Key)->Value = FMath::Min(scaledValue, Settings->GladiatorStatCaps[statPair.Key]);
	}

	gladiadorData.OpponentData = opponent;
}

EArenaModifier URomeTournamentComponent::RollArenaModifier() const
{
	return static_cast<EArenaModifier>(FMath::RandRange(0, static_cast<int>(EArenaModifier::BrutalFight)));
}

int URomeTournamentComponent::GetPointsValue(FGuid gladiatorId) const
{
	int pointsValue = 0;
	const auto* gladiatorData = GladiatorsSystem->GetGladiatorData(gladiatorId);

	for (const auto& stat : gladiatorData->Stats)
	{
		pointsValue += stat.Value;
	}

	pointsValue += gladiatorData->KnownMasteries.Num() * Settings->PointValuePerKnownMastery;
	return pointsValue;
}

void URomeTournamentComponent::GenerateMasteries(FGladiatorData& data, int masteriesCount)
{
	for (int i = 0; i < masteriesCount; i++)
	{
		EGladiatorMastery masteryToAdd;
		do
		{
			masteryToAdd = static_cast<EGladiatorMastery>(FMath::RandRange(0, static_cast<int>(EGladiatorMastery::RomesFavourite)));
		}
		while (data.ActiveMasteries.Contains(masteryToAdd));

		data.KnownMasteries.Add(masteryToAdd);
		data.ActiveMasteries.Add(masteryToAdd);
	}
}

bool URomeTournamentComponent::ShouldLoserDie(const FGladiatorData& loserData, EArenaModifier modifier) const
{
	const float deathChanceReduction = DynastySubsystem->GetFloatValueBonus(
		DynastySubsystem->Settings->SurvivalTacticsGladiatorSurviveAndCoinsGainedMultipliers, EDynastyTalent::SurvivalTactics) - 1.f;
	if (modifier == EArenaModifier::DeadlyCircumstances)
	{
		return FMath::FRand() <= Settings->DeadlyCircumstancesDeathChance - deathChanceReduction;
	}
	else
	{
		float deathChance = Settings->BaseChanceOfDeath - deathChanceReduction;
		if (loserData.ActiveMasteries.Contains(EGladiatorMastery::RomesFavourite))
		{
			deathChance -= Settings->RomesFavouriteChanceToAvoidDeathModifier;
		}

		return FMath::FRand() <= deathChance;
	}
}

int URomeTournamentComponent::CalculateGoldReward(FGladiatorData& data, bool wasVictory, EArenaModifier modifier, int previousWins) const
{
	if (!wasVictory && !data.ActiveMasteries.Contains(EGladiatorMastery::Entertainer))
	{
		return 0;
	}

	float reward = Settings->BaseGoldReward;
	reward += data.GetStat(EGladiatorStat::Charisma)->Value * Settings->ExtraGoldPerGladiatorCharismaPoint;
	reward += AdvisorsSubsystem->GetAdvisorWorkerSkill(GetTournamentData()->DelegatedOfficer, ESkill::Leadership)
		* Settings->ExtraGoldPerOfficerLeadershipPoint;

	if (wasVictory && data.ActiveMasteries.Contains(EGladiatorMastery::Victor))
	{
		reward *= Settings->VictorRewardMultiplier;
	}
	if (data.ActiveMasteries.Contains(EGladiatorMastery::ReturningChampion))
	{
		reward += reward * FMath::Min(Settings->ReturningChampionRewardBonusMultiplierCap,
			previousWins * Settings->ReturningChampionRewardMultiplierBonus);
	}
	if (modifier == EArenaModifier::HighStakes)
	{
		reward *= Settings->HighStakesGoldMultiplier;
	}
	if (!wasVictory)
	{
		reward *= Settings->EntertainerRewardMultiplier;
	}

	return FMath::RoundToInt(reward);
}
