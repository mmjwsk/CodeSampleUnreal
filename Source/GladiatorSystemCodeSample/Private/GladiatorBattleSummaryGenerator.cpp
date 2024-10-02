// Fill out your copyright notice in the Description page of Project Settings.

#include "LifeGoalGeneral/GladiatorBattleSummaryGenerator.h"

void UGladiatorBattleSummaryGenerator::Initialize(UGladiatorsSettings* settings)
{
	Settings = settings;
}

void UGladiatorBattleSummaryGenerator::GenerateSummary(const FGladiatorBattleData& battle)
{
	Summary = "";
	FirstCombatantName = battle.FirstCombatantData.Name;
	SecondCombatantName = battle.SecondCombatantData.Name;
	AddLine(FString::Format(*Settings->SummaryOpening.GetRandomLine(),{FirstCombatantName, SecondCombatantName}));

	for (int i = 0; i < battle.Rounds.Num(); i++)
	{
		AddLine(Settings->SummaryRoundDivisor);
		AddLine(FString::Format(*Settings->SummaryRoundStarted.GetRandomLine(), {i}));
		
		GenerateRound(battle.Rounds[i]);
	}

	AddLine(Settings->SummaryRoundDivisor);
	FString winner = battle.bFirstCombatantWon ? FirstCombatantName : SecondCombatantName;
	AddLine(FString::Format(*Settings->SummaryEnding.GetRandomLine(), {winner}));
}

void UGladiatorBattleSummaryGenerator::GenerateRound(const FGladiatorBattleRound& round)
{
	for (const auto& turn : round.Turns)
	{
		GenerateTurn(turn);
	}
	
	FString winner = round.bFirstCombatantWon ? FirstCombatantName : SecondCombatantName;
	FString loser = round.bFirstCombatantWon ? SecondCombatantName : FirstCombatantName;
	AddLine(FString::Format(*Settings->SummaryRoundEnded.GetRandomLine(), {loser, winner}));
}

void UGladiatorBattleSummaryGenerator::GenerateTurn(const FGladiatorBattleTurn& turn)
{
	AddLine(Settings->SummaryTurnDivisor);
	if (turn.FirstCombatantAction.bAttack || turn.SecondCombatantAction.bAttack)
	{
		if (turn.bFirstCombatantWentFirst)
		{
			GenerateAction(turn.FirstCombatantAction, FirstCombatantName);
			GenerateAction(turn.SecondCombatantAction, SecondCombatantName);
		}
		else
		{
			GenerateAction(turn.SecondCombatantAction, SecondCombatantName);
			GenerateAction(turn.FirstCombatantAction, FirstCombatantName);
		}
	}
	else
	{
		AddLine(Settings->SummaryNoAction.GetRandomLine());
	}
}

void UGladiatorBattleSummaryGenerator::GenerateAction(const FGladiatorBattleAction& action, const FString& attackerName)
{
	if (!action.bAttack) return;

	AddLine(FString::Format(*Settings->SummaryAttack.GetRandomLine(), {attackerName, action.Damage}));
	GenerateMasteryLines(action.AttackerTriggeredMasteries);
	GenerateMasteryLines(action.DefenderTriggeredMasteries);
}

void UGladiatorBattleSummaryGenerator::GenerateMasteryLines(const TArray<EGladiatorMastery>& triggeredMasteries)
{
	for (const auto mastery : triggeredMasteries)
	{
		AddPhrase(Settings->SummaryMasteryLines[mastery].GetRandomLine());
	}
}

void UGladiatorBattleSummaryGenerator::AddLine(const FString& line)
{
	Summary.Append(LINE_TERMINATOR);
	Summary.Append(line);
}

void UGladiatorBattleSummaryGenerator::AddPhrase(const FString& phrase)
{
	Summary.Append(" ");
	Summary.Append(phrase);
}
