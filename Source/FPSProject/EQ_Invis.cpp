// Fill out your copyright notice in the Description page of Project Settings.

#include "EQ_Invis.h"
#include "Net/UnrealNetwork.h"

AEQ_Invis::AEQ_Invis()
{
	MyType = EEquipmentType::EWearable;


}

void AEQ_Invis::Applied()
{

	BP_Applied();

}


void AEQ_Invis::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AEQ_Invis, EffectTransitionSpeed);
}

void AEQ_Invis::PickedUpBy(APawn* Pawn)
{
	Super::PickedUpBy(Pawn);
	Applied();

}