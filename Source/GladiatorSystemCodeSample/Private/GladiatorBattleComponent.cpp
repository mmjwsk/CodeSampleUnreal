// Fill out your copyright notice in the Description page of Project Settings.


#include "LifeGoalGeneral/GladiatorBattleComponent.h"

void UGladiatorBattleComponent::Initialize(UGladiatorsSettings* settings)
{
	Settings = settings;
	WonRoundCounter = TTuple<int, int>();
	HealthCounter = TTuple<int, int>();
	ChargeCounter = TTuple<int, int>();
}

void UGladiatorBattleComponent::SimulateBattle(const FGladiatorData& firstCombatant, const FGladiatorData& secondCombatant,
	EArenaModifier arenaModifier)
{
	BattleData = FGladiatorBattleData(firstCombatant, secondCombatant);
	BattleData.ArenaModifier = arenaModifier;

	WonRoundCounter.Key = 0;
	WonRoundCounter.Value = 0;
	do
	{
		SimulateBattleRound();
		if (BattleData.Rounds.Last().bFirstCombatantWon)
		{
			WonRoundCounter.Key++;
		}
		else
		{
			WonRoundCounter.Value++;
		}
	}
	while (WonRoundCounter.Key < Settings->RoundsToWinBattle && WonRoundCounter.Value < Settings->RoundsToWinBattle);

	BattleData.bFirstCombatantWon = WonRoundCounter.Key == Settings->RoundsToWinBattle;
}

void UGladiatorBattleComponent::SimulateBattleRound()
{
	HealthCounter.Key = BattleData.FirstCombatantData.GetStat(EGladiatorStat::Health)->Value;
	HealthCounter.Value = BattleData.SecondCombatantData.GetStat(EGladiatorStat::Health)->Value;
	ChargeCounter.Key = 0;
	ChargeCounter.Value = 0;

	BattleData.Rounds.Add(FGladiatorBattleRound());
	do
	{
		SimulateBattleTurn();
	}
	while (AreCombatantsAlive());

	BattleData.Rounds.Last().bFirstCombatantWon = HealthCounter.Key > 0;
}

void UGladiatorBattleComponent::SimulateBattleTurn()
{
	FGladiatorBattleTurn turn = FGladiatorBattleTurn();

	ChargeCounter.Key += GetSpeed(BattleData.FirstCombatantData);
	ChargeCounter.Value += GetSpeed(BattleData.SecondCombatantData);
	turn.bFirstCombatantWentFirst = ChargeCounter.Key >= ChargeCounter.Value;

	if (turn.bFirstCombatantWentFirst)
	{
		MakeFirstCombatantAction(turn);
		if (AreCombatantsAlive())
		{
			MakeSecondCombatantAction(turn);
		}
	}
	else
	{
		MakeSecondCombatantAction(turn);
		if (AreCombatantsAlive())
		{
			MakeFirstCombatantAction(turn);
		}
	}

	BattleData.Rounds.Last().Turns.Add(turn);
}

int UGladiatorBattleComponent::GetSpeed(FGladiatorData& data) const
{
	float speed = data.GetStat(EGladiatorStat::Speed)->Value;
	if (data.ActiveMasteries.Contains(EGladiatorMastery::Immovable))
	{
		speed -= data.GetStat(EGladiatorStat::Speed)->Value * Settings->ImmovableStatDriftMultiplier;
	}
	if (data.ActiveMasteries.Contains(EGladiatorMastery::Unstoppable))
	{
		speed += data.GetStat(EGladiatorStat::Defense)->Value * Settings->UnstoppableStatDriftMultiplier;
	}

	if (BattleData.ArenaModifier == EArenaModifier::ProlongedFight)
	{
		speed *= Settings->ProlongedFightSpeedMultiplier;
	}
	else if (BattleData.ArenaModifier == EArenaModifier::QuickDuel)
	{
		speed *= Settings->QuickDuelSpeedMultiplier;
	}

	return FMath::Max(0, FMath::RoundToInt(speed));
}

float UGladiatorBattleComponent::GetDamage(FGladiatorData& data) const
{
	float damage = data.GetStat(EGladiatorStat::Attack)->Value;
	if (data.ActiveMasteries.Contains(EGladiatorMastery::Power))
	{
		damage *= Settings->PowerDamageMultiplier;
	}
	if (BerserkerTriggeredLastTurn(data))
	{
		damage *= Settings->BerserkerAttackDefenseMultiplier;
	}

	if (BattleData.ArenaModifier == EArenaModifier::RomanWeapons)
	{
		damage += Settings->RomanWeaponsDamageModifier;
	}
	else if (BattleData.ArenaModifier == EArenaModifier::BrokenWeapons)
	{
		damage += Settings->BrokenWeaponsDamageModifier;
	}

	return FMath::Max(0.f, damage);
}

float UGladiatorBattleComponent::GetDefense(FGladiatorData& data) const
{
	float defense = data.GetStat(EGladiatorStat::Defense)->Value;
	if (data.ActiveMasteries.Contains(EGladiatorMastery::Armored))
	{
		defense *= Settings->ArmoredDefenseMultiplier;
	}
	if (BerserkerTriggeredLastTurn(data))
	{
		defense *= Settings->BerserkerAttackDefenseMultiplier;
	}
	if (data.ActiveMasteries.Contains(EGladiatorMastery::Immovable))
	{
		defense += data.GetStat(EGladiatorStat::Speed)->Value * Settings->ImmovableStatDriftMultiplier;
	}
	if (data.ActiveMasteries.Contains(EGladiatorMastery::Unstoppable))
	{
		defense -= data.GetStat(EGladiatorStat::Defense)->Value * Settings->UnstoppableStatDriftMultiplier;
	}

	if (BattleData.ArenaModifier == EArenaModifier::FreshEquipment)
	{
		defense *= Settings->FreshEquipmentDefenseMultiplier;
	}
	else if (BattleData.ArenaModifier == EArenaModifier::LackOfEquipment)
	{
		defense *= Settings->LackOfEquipmentDefenseMultiplier;
	}

	return FMath::Max(0.f, defense);
}

bool UGladiatorBattleComponent::BerserkerTriggeredLastTurn(const FGladiatorData& data) const
{
	bool triggered = false;
	if (!BattleData.Rounds.Last().Turns.IsEmpty())
	{
		if (BattleData.FirstCombatantData.Id == data.Id)
		{
			triggered = BattleData.Rounds.Last().Turns.Last().SecondCombatantAction.
				DefenderTriggeredMasteries.Contains(EGladiatorMastery::Berserker);
		}
		else
		{
			triggered = BattleData.Rounds.Last().Turns.Last().FirstCombatantAction.
				DefenderTriggeredMasteries.Contains(EGladiatorMastery::Berserker);
		}
	}
	return triggered;
}

FGladiatorBattleAction UGladiatorBattleComponent::MakeBattleAction(int& attackerCharge, FGladiatorData& attacker, int& attackerHealth,
	int& defenderCharge, FGladiatorData& defender, int& defenderHealth)
{
	int chargeThreshold = BattleData.ArenaModifier == EArenaModifier::SmallCrowd
		? Settings->SmallCrowdChargeToAttack
		: Settings->ChargeToAttack;

	if (attackerCharge < chargeThreshold)
	{
		return FGladiatorBattleAction();
	}

	TArray<EGladiatorMastery> attackerTriggeredMasteries = TArray<EGladiatorMastery>();
	TArray<EGladiatorMastery> defenderTriggeredMasteries = TArray<EGladiatorMastery>();

	// HANDLE QUICK ATTACK ROLL
	if (attacker.ActiveMasteries.Contains(EGladiatorMastery::QuickAttack) && FMath::FRand() <= Settings->QuickAttackTriggerChance)
	{
		attackerTriggeredMasteries.Add(EGladiatorMastery::QuickAttack);
	}
	else
	{
		attackerCharge -= chargeThreshold;
	}
	
	// HANDLE DODGE ROLL
	if (defender.ActiveMasteries.Contains(EGladiatorMastery::Dodge) && FMath::FRand() <= Settings->DodgeTriggerChance)
	{
		defenderTriggeredMasteries.Add(EGladiatorMastery::Dodge);
		return FGladiatorBattleAction(0, attackerTriggeredMasteries, defenderTriggeredMasteries);
	}

	// HANDLE HEAVY ATTACK ROLL
	if (attacker.ActiveMasteries.Contains(EGladiatorMastery::HeavyAttack) && FMath::FRand() <= Settings->HeavyAttackTriggerChance)
	{
		attackerTriggeredMasteries.Add(EGladiatorMastery::HeavyAttack);
		defenderCharge = 0;
	}

	float damage = GetDamage(attacker);
	float defense = GetDefense(defender);

	// HANDLE PRECISION ROLL
	if (attacker.ActiveMasteries.Contains(EGladiatorMastery::Precision) && FMath::FRand() <= Settings->PrecisionDefenceIgnoringChance)
	{
		attackerTriggeredMasteries.Add(EGladiatorMastery::Precision);
		defense = 0.f;
	}

	// HANDLE PROTECTION ROLL
	if (defender.ActiveMasteries.Contains(EGladiatorMastery::Protection) && FMath::FRand() <= Settings->ProtectionTriggerChance)
	{
		defenderTriggeredMasteries.Add(EGladiatorMastery::Protection);
		damage *= Settings->ProtectionDamageMultiplier;
	}

	// HANDLE STRIKE ROLL
	if (attacker.ActiveMasteries.Contains(EGladiatorMastery::Strike) && FMath::FRand() <= Settings->StrikeDoubleDamageChance)
	{
		attackerTriggeredMasteries.Add(EGladiatorMastery::Strike);
		damage *= 2.f;
	}

	int damageReceived = FMath::Max(Settings->MinDamage, FMath::RoundToInt(damage - defense));
	defenderHealth -= damageReceived;
	if (defenderHealth > 0)
	{
		// TRIGGER BERSERKER
		if (defender.ActiveMasteries.Contains(EGladiatorMastery::Berserker))
		{
			defenderTriggeredMasteries.Add(EGladiatorMastery::Berserker);
		}
		
		// HANDLE RIPOSTE ROLL
		if (defender.ActiveMasteries.Contains(EGladiatorMastery::Riposte) && FMath::FRand() <= Settings->RiposteTriggerChance)
		{
			defenderTriggeredMasteries.Add(EGladiatorMastery::Riposte);
			attackerHealth -= FMath::Max(Settings->MinDamage, FMath::RoundToInt(damageReceived * Settings->RiposteDamageMultiplier));
		}
	}

	return FGladiatorBattleAction(damageReceived, attackerTriggeredMasteries, defenderTriggeredMasteries);
}

void UGladiatorBattleComponent::MakeFirstCombatantAction(FGladiatorBattleTurn& turn)
{
	turn.FirstCombatantAction = MakeBattleAction(ChargeCounter.Key, BattleData.FirstCombatantData, HealthCounter.Key,
		ChargeCounter.Value, BattleData.SecondCombatantData, HealthCounter.Value);
}

void UGladiatorBattleComponent::MakeSecondCombatantAction(FGladiatorBattleTurn& turn)
{
	turn.SecondCombatantAction = MakeBattleAction(ChargeCounter.Value, BattleData.SecondCombatantData, HealthCounter.Value,
		ChargeCounter.Key, BattleData.FirstCombatantData, HealthCounter.Key);
}

bool UGladiatorBattleComponent::AreCombatantsAlive() const
{
	return HealthCounter.Key > 0 && HealthCounter.Value > 0;
}