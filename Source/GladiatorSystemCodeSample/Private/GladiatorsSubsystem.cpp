// Fill out your copyright notice in the Description page of Project Settings.


#include "LifeGoalGeneral/GladiatorsSubsystem.h"
#include "Core/EmpirePlayerController.h"
#include "Inventory/InventorySubsystem.h"
#include "LifeGoalGeneral/GladiatorBattleComponent.h"
#include "LifeGoalGeneral/GladiatorBattleSummaryGenerator.h"
#include "LifeGoalGeneral/RomeTournamentComponent.h"
#include "Advisors/AdvisorsSubsystem.h"
#include "NPCRelated/Family/FamiliesSubsystem.h"
#include "NPCRelated/NPCDataSubsystem.h"
#include "RpgStats/SkillsSubsystem.h"
#include "RpgStats/Dynasty/DynastyRelatedEnums.h"
#include "RpgStats/Dynasty/DynastySettings.h"
#include "RpgStats/Dynasty/DynastySubsystem.h"
#include "Utils/ArrayUtils.h"
#include "RpgStats/OutpostFeats/OutpostFeatsSubsystem.h"

void UGladiatorsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogTemp, Warning, TEXT("Gladiators Subsystem Initialized"));
	FWorldDelegates::OnStartGameInstance.AddUObject(this, &UGladiatorsSubsystem::OnGameInstanceStarted);
}

void UGladiatorsSubsystem::OnGameInstanceStarted(UGameInstance* gameInstance)
{
	gameInstance->GetSubsystem<UTimeFlowSubsystem>()->HourPassed.AddDynamic(this, &UGladiatorsSubsystem::OnHourPassed);
	gameInstance->GetSubsystem<UTimeFlowSubsystem>()->DayPassed.AddDynamic(this, &UGladiatorsSubsystem::OnDayPassed);
	NpcSubsystem = gameInstance->GetSubsystem<UNPCDataSubsystem>();
	SkillsSubsystem = gameInstance->GetSubsystem<USkillsSubsystem>();
	DynastySubsystem = gameInstance->GetSubsystem<UDynastySubsystem>();

	BattleComponent = NewObject<UGladiatorBattleComponent>();
	BattleComponent->Initialize(Settings);

	BattleSummaryGenerator = NewObject<UGladiatorBattleSummaryGenerator>();
	BattleSummaryGenerator->Initialize(Settings);

	TournamentComponent = NewObject<URomeTournamentComponent>();
	TournamentComponent->Initialize(this, gameInstance, BattleComponent, BattleSummaryGenerator, 
		gameInstance->GetSubsystem<UAdvisorsSubsystem>());
}

const FTournamentData& UGladiatorsSubsystem::GetTournamentData()
{
	return SaveLoadComponent->TournamentData;
}

void UGladiatorsSubsystem::ApplyTournamentResults()
{
	for (const auto& deadGladiatorId : TournamentComponent->GetDeadGladiators())
	{
		RemoveGladiator(deadGladiatorId);
	}

	for (const auto& pardonedGladiatorId : SaveLoadComponent->TournamentData.PardonedGladiators)
	{
		PardonGladiator(pardonedGladiatorId);
	}

	int goldGained = SaveLoadComponent->TournamentData.Gold;
	float goldMultiplier = DynastySubsystem->GetFloatValueBonus(
		DynastySubsystem->Settings->SurvivalTacticsGladiatorSurviveAndCoinsGainedMultipliers, EDynastyTalent::SurvivalTactics);
	GetGameInstance()->GetSubsystem<UInventorySubsystem>()->AddNpcItemsToOutpostInventory(ENpcItem::Coin, FMath::RoundToInt(goldGained * goldMultiplier));
	SaveLoadComponent->TournamentData = FTournamentData();
}

void UGladiatorsSubsystem::GetTournamentResultsData(TArray<FString>& deadGladiatorNames, TArray<FString>& pardonedGladiatorNames, int& gold)
{
	for (const auto& deadGladiatorId : TournamentComponent->GetDeadGladiators())
	{
		deadGladiatorNames.Add(GetGladiatorDataById(deadGladiatorId).Name);
	}

	for (const auto& pardonedGladiatorId : SaveLoadComponent->TournamentData.PardonedGladiators)
	{
		pardonedGladiatorNames.Add(GetGladiatorDataById(pardonedGladiatorId).Name);
	}

	gold = SaveLoadComponent->TournamentData.Gold;
}

void UGladiatorsSubsystem::StartTournament(FGuid officer, TArray<FGuid> sentGladiators, int startingGold)
{
	TournamentComponent->InitializeTournament(officer, sentGladiators, startingGold);
}

void UGladiatorsSubsystem::RevealOpponent(FGuid gladiatorId)
{
	const int revealCost = GetRevealCost();
	if (SaveLoadComponent->TournamentData.Gold < revealCost) return;
	
	SaveLoadComponent->TournamentData.Gold -= revealCost;
	SaveLoadComponent->TournamentData.Gladiators.FindByPredicate([&](const FTournamentGladiatorData& data)
	{
		return data.GladiatorId == gladiatorId;
	})
	->bOpponentRevealed = true;
}

int UGladiatorsSubsystem::GetRevealCost() const
{
	const float revealValueFromDynastyTalent = DynastySubsystem->GetFloatValueBonus(
		DynastySubsystem->Settings->LeadersInsightLearningEnemyGladiatorsChanceAndCostReductionPercentages, EDynastyTalent::LeadersInsight);
	const float reductionValue = revealValueFromDynastyTalent != 1.f ? 1.f - revealValueFromDynastyTalent : 1.f;
	return FMath::RoundToInt(Settings->GoldToRevealOpponent * reductionValue);
}

const FGladiatorData& UGladiatorsSubsystem::GetGladiatorDataById(FGuid gladiatorId) const
{
	return *UArrayUtils::FindItemById(SaveLoadComponent->Gladiators, gladiatorId);
}

TArray<FGladiatorData> UGladiatorsSubsystem::GetAvailableGladiators() const
{
	if (SaveLoadComponent->TournamentData.State != ERomeTournamentState::InProgress
		&& SaveLoadComponent->TournamentData.State != ERomeTournamentState::Finished)
	{
		return SaveLoadComponent->Gladiators;
	}

	auto availableGladiators = SaveLoadComponent->Gladiators;
	for (int i = availableGladiators.Num() - 1; i >= 0; i--)
	{
		for (const auto& tournamentGladiator : SaveLoadComponent->TournamentData.Gladiators)
		{
			if (availableGladiators[i].Id == tournamentGladiator.GladiatorId)
			{
				availableGladiators.RemoveAt(i);
				break;
			}
		}
	}

	return availableGladiators;
}

void UGladiatorsSubsystem::TurnCandidateIntoInitiate(FGuid candidateId, FGuid officerId)
{
	if (!SaveLoadComponent->Candidates.Contains(candidateId))
	{
		UE_LOG(LogArtractive, Warning, TEXT("Gladiator Initiate to be trained is not on Candidates list! Something went wrong!"));
		return;
	}

	SaveLoadComponent->Candidates.Remove(candidateId);
	SaveLoadComponent->Initiates.Add(FGladiatorInitiateData(candidateId, GetHoursToTrainInitiate(officerId), officerId));
}

void UGladiatorsSubsystem::StopInitiateTraining(FGuid initiateId)
{
	FGladiatorInitiateData initiateData;
	if (TryGetInitiateById(initiateData, initiateId))
	{
		SaveLoadComponent->Initiates.Remove(initiateData);
		SaveLoadComponent->Candidates.Add(initiateId);
	}
}

bool UGladiatorsSubsystem::TryGetInitiateById(FGladiatorInitiateData& initiateData, FGuid initiateId)
{
	return UArrayUtils::TryGetItemById(initiateData, SaveLoadComponent->Initiates, initiateId);
}

int UGladiatorsSubsystem::GetHoursToTrainInitiate(FGuid trainingOfficerId) const
{
	const float dynastyTalentValue = DynastySubsystem->GetFloatValueBonus(
		DynastySubsystem->Settings->AdvancedTrainingTimeToTrainGladiatorsAndExperiencePercentages, EDynastyTalent::AdvancedTraining);
	const float dynastyMultiplier = dynastyTalentValue != 1.f ? 1 - dynastyTalentValue : 1.f;
	return FMath::RoundToInt(Settings->DefaultHoursToTrainInitiate * dynastyMultiplier
		* (1.f - SkillsSubsystem->GetSkillLevel(trainingOfficerId, ESkill::Leadership)
			* Settings->TrainingDurationReductionPercentPerLeadershipSkillPoint));
}

void UGladiatorsSubsystem::ApplyGladiatorModifications(FGuid gladiatorId, FText newName, TMap<EGladiatorStat, int> statDeltas,
	TArray<EGladiatorMastery> newMasteries, TArray<EGladiatorMastery> newActiveMasteries, int remainingXp)
{
	auto* gladiatorData = GetGladiatorData(gladiatorId);
	gladiatorData->Name = newName.ToString();

	for (const auto& statPair : statDeltas)
	{
		*gladiatorData->GetStat(statPair.Key) += statPair.Value;
	}

	gladiatorData->KnownMasteries = newMasteries;
	gladiatorData->ActiveMasteries = newActiveMasteries;
	gladiatorData->UnusedXp = remainingXp;
}

void UGladiatorsSubsystem::GrantXp(FGuid gladiatorId, int amount)
{
	const float dynastyTalentValue = DynastySubsystem->GetFloatValueBonus(
		DynastySubsystem->Settings->AdvancedTrainingTimeToTrainGladiatorsAndExperiencePercentages, EDynastyTalent::AdvancedTraining);
	const float dynastyMultiplier = dynastyTalentValue != 1.f ? 1 + dynastyTalentValue : 1.f;
	amount = FMath::RoundToInt(amount * dynastyMultiplier);
	GetGladiatorData(gladiatorId)->UnusedXp += amount;
}

int UGladiatorsSubsystem::GetMasterySlotCount(FGuid gladiatorId)
{
	return Settings->BasicMasterySlotCount
		+ GetGladiatorStatValue(gladiatorId, EGladiatorStat::Tactics) / Settings->TacticsPerAdditionalMasterySlot;
}

int UGladiatorsSubsystem::GetGladiatorStatValue(FGuid gladiatorId, EGladiatorStat stat)
{
	return GetGladiatorData(gladiatorId)->GetStat(stat)->Value;
}

TArray<EGladiatorMastery> UGladiatorsSubsystem::GetLearnableMasteriesForStat(FGuid gladiatorId, EGladiatorStat stat)
{
	auto masteries = Settings->MasteriesPerSkill[stat].Masteries;
	auto* data = GetGladiatorData(gladiatorId);

	for (int i = masteries.Num() - 1; i >= 0; i--)
	{
		if (data->KnownMasteries.Contains(masteries[i]))
		{
			masteries.RemoveAt(i);
		}
	}

	return masteries;
}

void UGladiatorsSubsystem::DEBUG_GenerateNewCandidate()
{
	// TODO #58 - should be added as slave and not as enemy when obtaining a gladiator
	SaveLoadComponent->Candidates.Add(NpcSubsystem->CreateEnemyNPC());
}

void UGladiatorsSubsystem::OnHourPassed(int hour)
{
	for (int i = SaveLoadComponent->Initiates.Num() - 1; i >= 0; i--)
	{
		SaveLoadComponent->Initiates[i].HoursTillInitiation--;
		if (SaveLoadComponent->Initiates[i].HoursTillInitiation == 0)
		{
			TurnInitiateIntoGladiator(SaveLoadComponent->Initiates[i]);
			SaveLoadComponent->Initiates.RemoveAt(i);
		}
	}
}

void UGladiatorsSubsystem::OnDayPassed(int day)
{
	if (SaveLoadComponent->LocalFightCooldown > 0)
	{
		SaveLoadComponent->LocalFightCooldown--;
	}
}

void UGladiatorsSubsystem::TurnInitiateIntoGladiator(const FGladiatorInitiateData& initiateData)
{
	const FGuid npcId = initiateData.Id;
	FGladiatorData gladiator = FGladiatorData();

	gladiator.Id = npcId;
	gladiator.Name = NpcSubsystem->GetNPCName(npcId).ToString();

	gladiator.GetStat(EGladiatorStat::Attack)->Value = FMath::Min(Settings->GladiatorStatCaps[EGladiatorStat::Attack],
		FMath::RoundToInt(
			NpcSubsystem->GetNPCStatValue(npcId, EStat::Physical)
			+ GetGladiatorInitialSkillPoints(npcId, ESkill::MeleeWeapons)
			+ GetGladiatorInitialSkillPoints(npcId, ESkill::RangedWeapons)));

	gladiator.GetStat(EGladiatorStat::Defense)->Value = FMath::Min(Settings->GladiatorStatCaps[EGladiatorStat::Defense],
		FMath::RoundToInt(
			NpcSubsystem->GetNPCStatValue(npcId, EStat::Physical)
			+ GetGladiatorInitialSkillPoints(npcId, ESkill::Defense)));

	gladiator.GetStat(EGladiatorStat::Speed)->Value = FMath::Min(Settings->GladiatorStatCaps[EGladiatorStat::Speed],
		FMath::RoundToInt(
			NpcSubsystem->GetNPCStatValue(npcId, EStat::Physical)
			* Settings->GladiatorSpeedPhysicalMultiplier));

	gladiator.GetStat(EGladiatorStat::Charisma)->Value = FMath::Min(Settings->GladiatorStatCaps[EGladiatorStat::Charisma],
		FMath::RoundToInt(
			NpcSubsystem->GetNPCStatValue(npcId, EStat::Social)
			+ GetGladiatorInitialSkillPoints(npcId, ESkill::Leadership)
			+ GetGladiatorInitialSkillPoints(npcId, ESkill::Rhetorics)));

	gladiator.GetStat(EGladiatorStat::Tactics)->Value = FMath::Min(Settings->GladiatorStatCaps[EGladiatorStat::Tactics],
		FMath::RoundToInt(
			NpcSubsystem->GetNPCStatValue(npcId, EStat::Mental)
			* Settings->GladiatorTacticsMentalMultiplier));

	gladiator.GetStat(EGladiatorStat::Health)->Value = FMath::Min(Settings->GladiatorStatCaps[EGladiatorStat::Health],
		NpcSubsystem->GetNPCStatValue(npcId, EStat::Physical)
		+ Settings->GladiatorHealthBaseValue);

	for (int threshold : Settings->SkillValuesToGainMasteriesAt)
	{
		AddRandomMasteryForStat(gladiator, EGladiatorStat::Attack, threshold);
		AddRandomMasteryForStat(gladiator, EGladiatorStat::Defense, threshold);
		AddRandomMasteryForStat(gladiator, EGladiatorStat::Speed, threshold);
		AddRandomMasteryForStat(gladiator, EGladiatorStat::Charisma, threshold);
	}

	SaveLoadComponent->Gladiators.Add(gladiator);
}

float UGladiatorsSubsystem::GetGladiatorInitialSkillPoints(FGuid npcId, ESkill skill) const
{
	return SkillsSubsystem->GetSkillLevel(npcId, skill) / Settings->GladiatorSkillPointDivisor;
}

FGladiatorData* UGladiatorsSubsystem::GetGladiatorData(FGuid gladiatorId)
{
	return UArrayUtils::FindItemById(SaveLoadComponent->Gladiators, gladiatorId);
}

void UGladiatorsSubsystem::RegisterLocalFightComponent(UGladiatorLocalFightComponent* component)
{
	LocalFightComponent = component;
}

void UGladiatorsSubsystem::PerformLocalGladiatorBattle(FGuid firstGladiatorId, FGuid secondGladiatorId)
{
	BattleComponent->SimulateBattle(GetGladiatorDataById(firstGladiatorId), GetGladiatorDataById(secondGladiatorId), EArenaModifier::None);
	BattleSummaryGenerator->GenerateSummary(BattleComponent->BattleData);
	FGuid& winner = BattleComponent->BattleData.bFirstCombatantWon ? firstGladiatorId : secondGladiatorId;
	GrantXp(winner, Settings->XpRewardForWinningLocalFight);
	SaveLoadComponent->LastLocalFightLog = BattleSummaryGenerator->Summary;
	SaveLoadComponent->LocalFightCooldown = Settings->LocalFightCooldownDays;
	GetGameInstance()->GetSubsystem<UOutpostFeatsSubsystem>()->RegisterOutpostFeat(EOutpostFeat::ArenaBattle);
}

void UGladiatorsSubsystem::PardonGladiator(FGuid gladiatorId)
{
	GrantPardonSkills(gladiatorId);

	// TODO #58 - make sure this method interacts with the Slaves npc list and not Enemies list as Gladiators should be considered Slaves
	FNPCData* data = NpcSubsystem->GetNPCDataReference(gladiatorId);
	data->NpcFeats.Add(ENpcFeat::OldWarrior);

	FAllyData allyData = FAllyData(*data);
	NpcSubsystem->RemoveNPC(gladiatorId);
	NpcSubsystem->AddNPCToOutpost(allyData);
	NpcSubsystem->SpawnNPC(gladiatorId, allyData.TransformInWorld);
	GetGameInstance()->GetSubsystem<UFamiliesSubsystem>()->RegisterFamily(TArray<FAllyData>() = {allyData});

	RemoveGladiator(gladiatorId);
}

void UGladiatorsSubsystem::GrantPardonSkills(FGuid gladiatorId)
{
	auto* gladiatorData = GetGladiatorData(gladiatorId);

	for (int i = 0; i < gladiatorData->GetStat(EGladiatorStat::Attack)->Investment / Settings->PardonedGladiatorStatsDivisor; i++)
	{
		ESkill skillToReward = FMath::RandBool()
			? ESkill::MeleeWeapons
			: ESkill::RangedWeapons;
		SkillsSubsystem->GiveSkillPoints(gladiatorId, skillToReward, 1);
	}

	SkillsSubsystem->GiveSkillPoints(gladiatorId, ESkill::Defense,
		gladiatorData->GetStat(EGladiatorStat::Defense)->Investment / Settings->PardonedGladiatorStatsDivisor);

	for (int i = 0; i < gladiatorData->GetStat(EGladiatorStat::Charisma)->Investment / Settings->PardonedGladiatorStatsDivisor; i++)
	{
		ESkill skillToReward = FMath::RandBool()
			? ESkill::Leadership
			: ESkill::Rhetorics;
		SkillsSubsystem->GiveSkillPoints(gladiatorId, skillToReward, 1);
	}
}

void UGladiatorsSubsystem::RemoveGladiator(FGuid gladiatorId)
{
	SaveLoadComponent->Gladiators.RemoveAt(SaveLoadComponent->Gladiators.IndexOfByPredicate(
	[gladiatorId](const FGladiatorData& gladiator)
	{
		return gladiator.Id == gladiatorId;
	}));
}

void UGladiatorsSubsystem::AddRandomMasteryForStat(FGladiatorData& data, EGladiatorStat stat, int statThreshold)
{
	if (data.GetStat(stat)->Value >= statThreshold)
	{
		TArray<EGladiatorMastery> availableMasteries = Settings->MasteriesPerSkill[stat].Masteries;
		for (auto mastery : data.KnownMasteries)
		{
			if (availableMasteries.Contains(mastery))
			{
				availableMasteries.Remove(mastery);
			}
		}

		data.KnownMasteries.Add(availableMasteries[FMath::RandHelper(availableMasteries.Num())]);
	}
}
