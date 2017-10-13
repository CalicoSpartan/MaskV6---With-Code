// Fill out your copyright notice in the Description page of Project Settings.

#include "EQ_Invis.h"
#include "Net/UnrealNetwork.h"

AEQ_Invis::AEQ_Invis()
{
	MyType = EEquipmentType::EWearable;


}

void AEQ_Invis::Applied()
{
	GetStaticMeshComponent()->bGenerateOverlapEvents = false;
	GetStaticMeshComponent()->SetSimulatePhysics(false);
	GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (AttachedPawn != NULL)
	{
		AttachToComponent(AttachedPawn->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName(TEXT("AbilityLocation1")));
	}
	BP_Applied();
	GetWorld()->GetTimerManager().SetTimer(LifeTimeTimer, this, &AEQ_Invis::DestroySelf, LifeTimeDelay, false);
}


void AEQ_Invis::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AEQ_Invis, EffectTransitionSpeed);
	DOREPLIFETIME(AEQ_Invis, PlayerMinInvisSpeed);
}

void AEQ_Invis::PickedUpBy(APawn* Pawn)
{
	Super::PickedUpBy(Pawn);
	Applied();

}
void AEQ_Invis::DestroySelf()
{
	if (Role == ROLE_Authority)
	{

		if (AttachedPawn->MyEquipment.Find(this) != -1)
		{
			AttachedPawn->MyEquipment.RemoveAt(AttachedPawn->MyEquipment.Find(this));
		}
		BP_Destroyed();
	}
}