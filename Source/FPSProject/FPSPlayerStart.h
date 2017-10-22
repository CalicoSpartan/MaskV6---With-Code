// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "FPSPlayerStart.generated.h"

/**
*
*/
UCLASS()
class FPSPROJECT_API AFPSPlayerStart : public APlayerStart
{
	GENERATED_BODY()


public:


	UPROPERTY(EditAnywhere, Category = "Defaults")
		bool bTeamB;

	UPROPERTY(EditAnywhere, Category = "Defaults")
		int32 TeamNumber;

	float FarthestDistanceFromEnemy = 0.0f;
	float ClosestDistanceToTeammate = 3000000.0f;

	UPROPERTY(VisibleAnywhere)
		UShapeComponent* TakenBox;

};
