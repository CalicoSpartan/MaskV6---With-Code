// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponSpawner.h"
#include "Net/UnrealNetwork.h"


// Sets default values
AWeaponSpawner::AWeaponSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AWeaponSpawner::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWeaponSpawner,ReferencedWeapon);
	DOREPLIFETIME(AWeaponSpawner, TotalAmmo);
	DOREPLIFETIME(AWeaponSpawner, MagazineAmmo);
	DOREPLIFETIME(AWeaponSpawner, RespawnTimerDelay);
	DOREPLIFETIME(AWeaponSpawner, WeaponRotation);
	DOREPLIFETIME(AWeaponSpawner, RespawnTimer);
	DOREPLIFETIME(AWeaponSpawner, SpawnOnStart);

}

// Called when the game starts or when spawned
void AWeaponSpawner::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWeaponSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AWeaponSpawner::ServerSetRespawnTimer_Validate()
{
	return true;
}

void AWeaponSpawner::ServerSetRespawnTimer_Implementation()
{

}

void AWeaponSpawner::ClientSetRespawnTimer()
{
	UE_LOG(LogClass, Log, TEXT("SetRespawnTimer was called"));
	if (Role == ROLE_Authority)
	{
		SetRespawnTimer();
	}
	else
	{
		ServerSetRespawnTimer();
	}
}


void AWeaponSpawner::SetRespawnTimer()
{
	if (Role == ROLE_Authority)
	{
		GetWorld()->GetTimerManager().SetTimer(RespawnTimer, this, &AWeaponSpawner::SpawnWeapon, RespawnTimerDelay, false);
	}
}

void AWeaponSpawner::SpawnWeapon()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	if (ReferencedWeapon)
	{
		AGun* MyWeapon = GetWorld()->SpawnActor<AGun>(ReferencedWeapon, GetActorLocation(), WeaponRotation, SpawnParams);
		MyWeapon->MySpawner = this;
		if (TotalAmmo && TotalAmmo <= MyWeapon->MaxAmmo)
		{
			MyWeapon->TotalAmmo = TotalAmmo;
		}
		else
		{
			MyWeapon->TotalAmmo = MyWeapon->MaxAmmo;
		}
		if (MagazineAmmo && MagazineAmmo <= MyWeapon->MagazineSize)
		{
			MyWeapon->AmmoLeftInMag = MagazineAmmo;
		}
		else
		{
			MyWeapon->AmmoLeftInMag = MyWeapon->MagazineSize;
		}
	}
}

