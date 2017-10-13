// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEquipment.h"
#include "EQ_Invis.generated.h"

/**
 * 
 */
UCLASS()
class FPSPROJECT_API AEQ_Invis : public ABaseEquipment
{
	GENERATED_BODY()
	
public:
	AEQ_Invis();

	UPROPERTY(Replicated, EditAnywhere,BlueprintReadWrite)
		float EffectTransitionSpeed = 3.0f;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
		float PlayerMinInvisSpeed = 3.0f;


public:

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintImplementableEvent)
		void BP_Applied();
	UFUNCTION(BlueprintImplementableEvent)
		void BP_Destroyed();

	UFUNCTION(Category = "Interaction")
		void Applied();


	UFUNCTION(BlueprintAuthorityOnly, Category = "Interaction")
		void PickedUpBy(APawn* Pawn);
	
	void DestroySelf();
};
