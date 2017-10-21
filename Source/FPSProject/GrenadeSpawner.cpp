// Fill out your copyright notice in the Description page of Project Settings.

#include "GrenadeSpawner.h"
#include "Net/UnrealNetwork.h"


// Sets default values
AGrenadeSpawner::AGrenadeSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGrenadeSpawner::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGrenadeSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGrenadeSpawner::SetRespawnTimer()
{
	if (Role == ROLE_Authority)
	{
		GetWorld()->GetTimerManager().SetTimer(RespawnTimer, this, &AGrenadeSpawner::SpawnGrenade, RespawnTimerDelay, false);
	}
}

void AGrenadeSpawner::SpawnGrenade()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	if (ReferencedGrenade)
	{
		ABaseGrenade* MyGrenade = GetWorld()->SpawnActor<ABaseGrenade>(ReferencedGrenade, GetActorLocation(), GrenadeRotation, SpawnParams);
		MyGrenade->MySpawner = this;
	}
}

void AGrenadeSpawner::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGrenadeSpawner, ReferencedGrenade);
	DOREPLIFETIME(AGrenadeSpawner, RespawnTimerDelay);
	DOREPLIFETIME(AGrenadeSpawner, GrenadeRotation);
	DOREPLIFETIME(AGrenadeSpawner, RespawnTimer);
	DOREPLIFETIME(AGrenadeSpawner, SpawnOnStart);
}

bool AGrenadeSpawner::ServerSetRespawnTimer_Validate()
{
	return true;
}
void AGrenadeSpawner::ServerSetRespawnTimer_Implementation()
{
	SetRespawnTimer();
}
void AGrenadeSpawner::ClientSetRespawnTimer()
{

	if (Role == ROLE_Authority)
	{
		SetRespawnTimer();
	}
	else
	{
		ServerSetRespawnTimer();
	}
}

