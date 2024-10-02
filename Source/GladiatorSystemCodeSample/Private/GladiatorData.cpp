

#include "LifeGoalGeneral/GladiatorData.h"
#include "Core/EmpirePlayerController.h"

FGladiatorStat::FGladiatorStat()
{
	Type = EGladiatorStat::Attack;
	Value = 0;
	Investment = 0;
}

FGladiatorStat::FGladiatorStat(EGladiatorStat type)
{
	Type = type;
	Value = 0;
	Investment = 0;
}

FGladiatorStat FGladiatorStat::operator++(int)
{
	Value++;
	Investment++;
	
	return *this;
}

FGladiatorStat FGladiatorStat::operator+=(int amount)
{
	Value += amount;
	Investment += amount;

	return *this;
}

FGladiatorStat FGladiatorStat::operator--(int)
{
	if (Investment == 0)
	{
		UE_LOG(LogArtractive, Warning, TEXT("Trying to decrement Gladiator Stat that has 0 investment points. Aborted."))
	}
	else
	{
		Value--;
		Investment--;
	}
	return *this;
}

FGladiatorStat FGladiatorStat::operator-=(int amount)
{
	if (Investment < amount)
	{
		UE_LOG(LogArtractive, Warning, TEXT("Trying to decrement Gladiator Stat by more points that were invested. Aborted."))
	}
	else
	{
		Value -= amount;
		Investment -= amount;
	}
	return *this;
}

FGladiatorData::FGladiatorData() : FDataWithIdBase()
{
	Name = "DefaultName";
	
	Stats = TArray<FGladiatorStat>();
	for (const EGladiatorStat stat : TEnumRange<EGladiatorStat>())
	{
		Stats.Add(FGladiatorStat(stat));
	}
	UnusedXp = 0;
	
	KnownMasteries = TArray<EGladiatorMastery>();
	ActiveMasteries = TArray<EGladiatorMastery>();
}

FGladiatorStat* FGladiatorData::GetStat(EGladiatorStat statType)
{
	return Stats.FindByPredicate([statType](const FGladiatorStat& stat)
	{
		return stat.Type == statType;
	});
}

FGladiatorInitiateData::FGladiatorInitiateData() : FDataWithIdBase()
{
	HoursTillInitiation = -1;
	OfficerId = FGuid();
}

FGladiatorInitiateData::FGladiatorInitiateData(FGuid npcId, int hoursTillInitiation, FGuid officerId) : FDataWithIdBase(npcId)
{
	HoursTillInitiation = hoursTillInitiation;
	OfficerId = officerId;
}
