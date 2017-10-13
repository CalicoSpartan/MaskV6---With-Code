// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "FPSCharacter.h"
#include "BaseEquipment.generated.h"

UENUM(BlueprintType)
enum EEquipmentType
{
	EWearable,
	EThrowable,
	EPlantable

};


/**
 * 
 */
UCLASS()
class FPSPROJECT_API ABaseEquipment : public AStaticMeshActor
{
	GENERATED_BODY()
	
public:
	ABaseEquipment();

	UPROPERTY(Replicated,EditAnywhere,Category = "Type")
		TEnumAsByte<enum EEquipmentType> MyType;
	UPROPERTY(Replicated,EditAnywhere,BlueprintReadWrite, Category = "Lifetime")
		bool bIsActive = false;
	UPROPERTY(Replicated,EditAnywhere,BlueprintReadWrite,Category = "Lifetime")
		float LifeTimeDelay = 20.0f;
	UPROPERTY(Replicated,EditAnywhere, BlueprintReadWrite,Category = "Lifetime")
		FTimerHandle LifeTimeTimer;
	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category = "Player")
		class AFPSCharacter* AttachedPawn;

private:


public:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void DestroySelf();
	
	UFUNCTION(Category = "Interaction")
		virtual void Thrown(FVector ThrowDirection, float ThrowStrength);

	UFUNCTION(Category = "Interaction")
		virtual void Planted(FVector Location = FVector::ZeroVector);
	UFUNCTION(Category = "Interaction")
		virtual void Applied();


	UFUNCTION(BlueprintAuthorityOnly, Category = "Interaction")
		virtual void PickedUpBy(APawn* Pawn);
};
