// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GladiatorBattleData.h"
#include "GladiatorLocalFightComponent.h"
#include "LifeGoalGeneralEnums.h"
#include "Utils/IntRange.h"
#include "GladiatorsSettings.generated.h"

USTRUCT(BlueprintType)
struct THEEMPIRE_API FGladiatorMasteriesArray
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<EGladiatorMastery> Masteries;
};


UCLASS()
class THEEMPIRE_API UGladiatorsSettings : public UDataAsset
{
	GENERATED_BODY()

public:
	// Duration of training in hours of a gladiator initiate. Can be reduced by assigned officer's leadership skill.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Training")
	int DefaultHoursToTrainInitiate = 72;

	// Leadership of assigned officer will reduce initiate training duration by this percentage
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Training", meta = (ClampMin = 0.f, ClampMax = 0.0099))
	float TrainingDurationReductionPercentPerLeadershipSkillPoint = 0.005;

	// During establishing initial parameters, the gladiator's stats will be determined based on relevant skill values divided by this
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Training")
	float GladiatorSkillPointDivisor = 10.f;

	// Multiplier to NPC Mental value that will produce gladiator's Tactics stat initial value
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Training")
	float GladiatorTacticsMentalMultiplier = 2.f;

	// Multiplier to NPC Physical value that will produce gladiator's Speed stat initial value
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Training")
	float GladiatorSpeedPhysicalMultiplier = 2.f;

	// Base value of initial health of all gladiators (before counting in stats)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Training")
	int GladiatorHealthBaseValue = 100;

	// Max value a given skill stat of gladiators can take (Attack, Defense etc.)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Stats")
	TMap<EGladiatorStat, int> GladiatorStatCaps;

	// Invested points into pardoned gladiator stats will be added into his proper Character stats upon pardon after dividing by THIS
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Stats")
	float PardonedGladiatorStatsDivisor = 2.f;

	// Minimal number of mastery slots each gladiator has / mastery slots count at Tactics 0
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Masteries")
	int BasicMasterySlotCount = 1;

	// Amount of Tactics skill value for gladiator to have additional active mastery slot, e.g. if THIS = 25,
	// gladiator will receive one additional slots on 25, 50, 75 and 100 Tactics
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Masteries")
	int TacticsPerAdditionalMasterySlot = 25;

	// Maps Masteries to their corresponding skills
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Masteries")
	TMap<EGladiatorStat, FGladiatorMasteriesArray> MasteriesPerSkill;

	// Maps Masteries to their descriptions
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Masteries")
	TMap<EGladiatorMastery, FString> MasteriesWithDescriptions;

	// Skill values at which mastery choice will be presented to a player.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Masteries")
	TArray<int> SkillValuesToGainMasteriesAt;

	// Multiplier to attacking gladiator's damage when he has Power mastery active
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Masteries|Effects|Attack")
	float PowerDamageMultiplier = 1.2f;

	// Chance that attack will deal double damage when attacking gladiator has Strike mastery active
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Masteries|Effects|Attack")
	float StrikeDoubleDamageChance = 0.2f;

	// Chance that attack will bypass damage reduction when attacking gladiator has Precision mastery active
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Masteries|Effects|Attack")
	float PrecisionDefenceIgnoringChance = 0.1f;
	
	// Chance that riposte effect will trigger if attacked Gladiator has Riposte mastery active
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Masteries|Effects|Attack")
	float RiposteTriggerChance = 0.5f;

	// Multiplier to the received damage that will be dealt back to attacker if the attacked gladiator has triggered Riposte
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Masteries|Effects|Attack")
	float RiposteDamageMultiplier = 0.25f;

	// Chance that protection effect will trigger if attacked Gladiator has Protection mastery active
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Masteries|Effects|Defense")
	float ProtectionTriggerChance = 0.3f;

	// Multiplier to the received damage if the attacked gladiator has triggered Protection
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Masteries|Effects|Defense")
	float ProtectionDamageMultiplier = 0.5f;

	// Chance that dodge effect will trigger if attacked Gladiator has Dodge mastery active
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Masteries|Effects|Defense")
	float DodgeTriggerChance = 0.05f;

	// Multiplier to gladiator's defense if he has Armored mastery active
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Masteries|Effects|Defense")
	float ArmoredDefenseMultiplier = 1.25f;

	// Multiplier to gladiator's Attack and Defense stats if Berserker mastery was triggered (is active and gladiator was hit in the previous turn)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Masteries|Effects|Defense")
	float BerserkerAttackDefenseMultiplier = 1.1f;

	// Chance that quick attack effect will trigger if attacking Gladiator has Quick Attack mastery active
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Masteries|Effects|Speed")
	float QuickAttackTriggerChance = 0.1f;

	// Chance that heavy attack effect will trigger if attacking Gladiator has Heavy Attack mastery active
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Masteries|Effects|Speed")
	float HeavyAttackTriggerChance = 0.1f;

	// Multiplier of gladiator's Speed that will be subtracted then and added to his Defense if Immovable mastery is active
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Masteries|Effects|Speed")
	float ImmovableStatDriftMultiplier = 0.5f;

	// Multiplier of gladiator's Defense that will be subtracted and then added to his Speed if Unstoppable mastery is active
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Masteries|Effects|Speed")
	float UnstoppableStatDriftMultiplier = 0.5f;

	// Multiplier of original reward that a gladiator with Entertainer active will receive even if he loses the fight
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Masteries|Effects|Charisma")
	float EntertainerRewardMultiplier = 0.5f;

	// Multiplier of reward received after a won match if gladiator has Victor mastery active
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Masteries|Effects|Charisma")
	float VictorRewardMultiplier = 1.2f;

	// Multiplier of reward that will be added to the original reward multiplier by a number of defeated enemies, up to a cap
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Masteries|Effects|Charisma")
	float ReturningChampionRewardMultiplierBonus = 0.1f;

	// Max bonus reward multiplier that can be achieved by Returning Champion gladiator (1 means +100% reward)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Masteries|Effects|Charisma")
	float ReturningChampionRewardBonusMultiplierCap = 1.f;

	// Modifier to a chance of avoiding death for a gladiator that has Rome's Favourite mastery active
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Masteries|Effects|Charisma")
	float RomesFavouriteChanceToAvoidDeathModifier = 0.1f;

	// Number of rounds a gladiator must win to win the battle
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Battle")
	int RoundsToWinBattle = 2;

	// Default value of charge gladiators need to accumulate to attack
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Battle")
	int ChargeToAttack = 100;

	// Minimal value of damage upon a successful attack
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Battle")
	int MinDamage = 1;

	// Value of charge gladiators need to accumulate to attack when Small Crowd is active
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Battle|Arena Modifiers")
	int SmallCrowdChargeToAttack = 150;

	// Value added to weapons damage when Roman Weapons is active
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Battle|Arena Modifiers")
	int RomanWeaponsDamageModifier = 10;

	// Value added to weapons damage when Broken Weapons is active
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Battle|Arena Modifiers")
	int BrokenWeaponsDamageModifier = -10;

	// Multiplier to gladiator's speed stat when Prolonged Fight is active
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Battle|Arena Modifiers")
	float ProlongedFightSpeedMultiplier = 0.9f;

	// Multiplier to gladiator's speed stat when Quick Duel is active
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Battle|Arena Modifiers")
	float QuickDuelSpeedMultiplier = 0.9f;

	// Multiplier to gladiator's defense stat when Fresh Equipment is active
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Battle|Arena Modifiers")
	float FreshEquipmentDefenseMultiplier = 1.2f;

	// Multiplier to gladiator's defense stat when Lack of Equipment is active
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Battle|Arena Modifiers")
	float LackOfEquipmentDefenseMultiplier = 0.9f;

	// Multiplier to gold reward after victorious duel if High Stakes is active
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Battle|Arena Modifiers")
	float HighStakesGoldMultiplier = 1.2f;

	// Chance of gladiator dying when losing a battle if Deadly Circumstances is active
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Battle|Arena Modifiers")
	float DeadlyCircumstancesDeathChance = 1.f;

	// Line that appears at the very start of the summary (for entire duel) - format {0} = NAME, {1} = NAME
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Battle|Summary")
	FGladiatorBattleDescriptionLines SummaryOpening;

	// Line that appears at the very end of the summary, stating who won the battle - format {0} = WINNER
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Battle|Summary")
	FGladiatorBattleDescriptionLines SummaryEnding;

	// Line that appears at the start of every round - format {0} = ROUND COUNTER
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Battle|Summary")
	FGladiatorBattleDescriptionLines SummaryRoundStarted;

	// Line that appears at the end of a round, stating who won the round - format {0} = LOSER, {1} = WINNER
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Battle|Summary")
	FGladiatorBattleDescriptionLines SummaryRoundEnded;

	// Line that appears if neither gladiator attacked in the round
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Battle|Summary")
	FGladiatorBattleDescriptionLines SummaryNoAction;

	// Line that appears if a gladiator attacked - format {0} = NAME, {1} = DAMAGE
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Battle|Summary")
	FGladiatorBattleDescriptionLines SummaryAttack;

	// Lines that are added to the SummaryAttack per each triggered mastery - first for attacker, then for defender
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Battle|Summary")
	TMap<EGladiatorMastery, FGladiatorBattleDescriptionLines> SummaryMasteryLines;

	// Set of symbols that appears when next round starts as a line
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Battle|Summary")
	FString SummaryRoundDivisor = "==========";

	// Set of symbols that appears when next turn starts as a line
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Battle|Summary")
	FString SummaryTurnDivisor = "----------";

	// Hour at which the tournament will progress and the report will be generated
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Tournament", meta = (ClampMin = 0, ClampMax = 23))
	int HourToShowTournamentReports = 12;

	// Base chance of death upon losing a battle in Rome
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Tournament")
	int BaseChanceOfDeath = 0.5f;

	// Amount of gold it costs to discover an opponent
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Tournament")
	int OpponentDiscoveryCost = 25;

	// Amount of XP gladiator earns when winning a tournament battle
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Tournament")
	int XpEarnedForWonTournamentBattle = 5;

	// Base chance that a gladiator who won all his battles in the tournament will receive pardon
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Tournament")
	float BaseChanceForPardon = 0.02f;

	// Value that gets added to the base pardon chance per every won battle. That is - if this value is non-zero,
	// gladiators with 7 wins and 0 losses are more likely to receive pardon than those with 3 wins and 0 losses
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Tournament")
	float CumulativeChanceForPardon = 0.01f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Tournament")
	int ExtraGladiatorPerOfficerLeadershipPoints = 25;

	// Base amount of gold received for a won battle
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Tournament|Gold")
	int BaseGoldReward = 50;

	// Amount of extra gold earned per a point of Gladiator's charisma stat
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Tournament|Gold")
	float ExtraGoldPerGladiatorCharismaPoint = 0.1f;

	// Amount of extra gold earned per a point of leadership of the sent officer
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Tournament|Gold")
	float ExtraGoldPerOfficerLeadershipPoint = 0.5f;
	
	// Cost in gold to reveal the next opponent if not revealed already
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Tournament|Gold")
	int GoldToRevealOpponent = 25;

	// Names of enemy gladiators will consist of two randomized words - this is the list of all possible first words
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Tournament|Enemy Generation")
	FGladiatorBattleDescriptionLines EnemyGladiatorFirstNames;

	// Names of enemy gladiators will consist of two randomized words - this is the list of all possible second words
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Tournament|Enemy Generation")
	FGladiatorBattleDescriptionLines EnemyGladiatorSecondNames;

	// Additional points an enemy generation receives for each mastery known by the gladiator the enemy will fight against
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Tournament|Enemy Generation")
	int PointValuePerKnownMastery = 10;

	// How many points will one active mastery on the generated opponent subtract from the pool.
	// Note that when calculating points we take KNOWN masteries into account for bonus while here there are ACTIVE ones for reduction
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Tournament|Enemy Generation")
	int PointCostPerOpponentActiveMastery = 15;

	// Defines ranges of how many active masteries the generated opponent will have given how many points he has available.
	// First value is the point threshold, second value is the min and max number of masteries a gladiator worth that many points can have.
	// NOTE: Please keep the values ordered from highest to lowest
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Tournament|Enemy Generation")
	TMap<int, FIntRange> ActiveMasteriesPerThreshold;

	// The MAX amount of points that will be added OR subtracted at random from the pool with each generation
	// (If pool is 100 and this value is 10, then generation can roll anything between 90 and 110)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Tournament|Enemy Generation")
	int PointValueMaxRandomDrift = 15;

	// Amount of points that is added to the generation per each won tournament round by that generation's opponent.
	// This value controls how much subsequent fights in a single tournament will grow in difficulty
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Tournament|Enemy Generation")
	int PointsPerTournamentProgress = 5;

	// Amount of hours the gladiators will be seen fighting in the arena (as in - spawned and fighting).
	// After that time the fight ends
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Local Fight")
	int GladiatorFightDuration = 5;

	// Amount of days that needs to pass since last local fight in order for the local fight to be available again
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Local Fight")
	int LocalFightCooldownDays = 7;

	// Offsets from the arena center for first gladiator spawnpoint
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Local Fight")
	FVector SpawnLocationOffsetPositionOne;

	// Offsets from the arena center for second gladiator spawnpoint
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Local Fight")
	FVector SpawnLocationOffsetPositionTwo;

	// Component to spawn on the Arena
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Local Fight")
	TSubclassOf<UGladiatorLocalFightComponent> LocalFightComponentClass;

	// Amount of xp gladiator receives when he wins local gladiator vs gladiator fight
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artractive|Local Fight")
	int XpRewardForWinningLocalFight = 5;

public:
	int RollActiveMasteriesCount(int points) const;
	FString RollGladiatorName() const;
};
