// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseEquipment.h"
#include "Net/UnrealNetwork.h"


ABaseEquipment::ABaseEquipment()
{
	bReplicates = true;
	MyType = EEquipmentType::EWearable;

}

void ABaseEquipment::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABaseEquipment, AttachedPawn);
	DOREPLIFETIME(ABaseEquipment, MyType);
	DOREPLIFETIME(ABaseEquipment, bIsActive);
	DOREPLIFETIME(ABaseEquipment, LifeTimeTimer);
}

void ABaseEquipment::DestroySelf()
{
	Destroy();
}

void ABaseEquipment::Thrown(FVector ThrowDirection, float ThrowStrength)
{
}

void ABaseEquipment::Planted(FVector Location)
{
}

void ABaseEquipment::Applied()
{
}

void ABaseEquipment::PickedUpBy(APawn * Pawn)
{
	if (Role == ROLE_Authority)
	{
		if (AFPSCharacter* Character = Cast<AFPSCharacter>(Pawn))
		{
			AttachedPawn = Character;
		}
	}
}
