// Fill out your copyright notice in the Description page of Project Settings.

#include "Gun.h"
#include "Net/UnrealNetwork.h"


AGun::AGun()
{
	//tell engine to replicate this actor
	bReplicates = true;
	IsExplosive = false;
	CanFire = true;
	IsReloading = false;
	IsProjectile = false;
	TotalAmmo = 0;
	AmmoLeftInMag = 0;
	MagazineSize = 0;
	
	
	//gun doesn't need to tick
	PrimaryActorTick.bCanEverTick = false;
	//static mesh actor disables overlap events by default, which we need to re-enable
	GetStaticMeshComponent()->bGenerateOverlapEvents = true;
	GetStaticMeshComponent()->SetSimulatePhysics(true);
	MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	MuzzleLocation->SetupAttachment(RootComponent);
	if (Role == ROLE_Authority)
	{
		bIsActive = true;
	}
}

void AGun::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGun, bIsActive);
	DOREPLIFETIME(AGun, AmmoLeftInMag);
	DOREPLIFETIME(AGun, TotalAmmo);
	DOREPLIFETIME(AGun, MagazineSize);
	DOREPLIFETIME(AGun, CanFire);
	DOREPLIFETIME(AGun, BulletForce);
	DOREPLIFETIME(AGun, IsReloading);
	DOREPLIFETIME(AGun, MaxAmmo);
	DOREPLIFETIME(AGun, MySpawner);
	DOREPLIFETIME(AGun, BaseMaterial);
}



void AGun::Shoot(FVector ShotStart, FVector ShotDirection)
{
	if (Role == ROLE_Authority)
	{
		if (AmmoLeftInMag > 0 && CanFire == true && WeaponInstigator != NULL)
		{
			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(WeaponInstigator);
			UE_LOG(LogClass, Log, TEXT("fired weapon"));
			
			float ShotDistance;
			float DamagePercent;
			if (GetWorld()->LineTraceSingleByChannel(shotData, ShotStart, ShotStart + ShotDirection.GetSafeNormal() * Range, ECollisionChannel::ECC_Visibility, QueryParams))
			{
				if (AFPSCharacter* const hitCharacter = Cast<AFPSCharacter>(shotData.GetActor()))
				{
					ShotDistance = FVector::Dist(MuzzleLocation->GetComponentLocation(), shotData.Location);
					if (shotData.BoneName == FName("head"))
					{
						UE_LOG(LogClass, Log, TEXT("HEADSHOT"));
						DamagePercent = HeadShotIncrease;
					}
					else
					{
						DamagePercent = 1.0f;
					}
					if (ShotDistance > PreferredRange)
					{
						/*
						float WeaponRangeDifference = Range - PreferredRange;
						float ShotRangeDifference = Range - ShotDistance;
						float OppositePercentToZeroDamage = ShotRangeDifference / WeaponRangeDifference;
						if (shotData.BoneName == FName("head"))
						{
							UE_LOG(LogClass, Log, TEXT("HEADSHOT"));
							DamagePercent = roundf(OppositePercentToZeroDamage * HeadShotIncrease * 100) / 100;
						}
						else
						{
							DamagePercent = roundf(OppositePercentToZeroDamage * 100) / 100;

						}
						*/
					}
					hitCharacter->Shooter = WeaponInstigator;
					if (hitCharacter->IsZoomed)
					{
						hitCharacter->StopZoom();
					}
					FVector bulletTrail = (shotData.Location - shotData.TraceStart).GetSafeNormal();
					hitCharacter->SetHitData(BulletForce, shotData.BoneName, bulletTrail);
					hitCharacter->ServerChangeHealthBy(-BulletDamage * DamagePercent);
					hitCharacter->TriggerUpdateUI();
					if (hitCharacter->GetCurrentHealth() <= 0.0f)
					{
						if (AFPSProjectGameModeBase* gm = Cast<AFPSProjectGameModeBase>(GetWorld()->GetAuthGameMode()))
						{
							if (AFPSCharacter* killer = Cast<AFPSCharacter>(WeaponInstigator))
							{
								gm->KillPlayer(hitCharacter, killer);
							}
						}
					}
					GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, FString::SanitizeFloat(hitCharacter->HealthPercentage));

				}
			}
			ChangeAmmo(TotalAmmo, AmmoLeftInMag - 1);
		}
	}
}

bool AGun::IsActive()
{
	
	return bIsActive;
}

bool AGun::StartReload_Validate()
{
	return true;
}
bool AGun::EndReload_Validate()
{
	return true;
}
void AGun::BeginPlay()
{
	Super::BeginPlay();
	
	

	
}
void AGun::ChangeAmmo(int32 Ammo, int32 Mag)
{
	//UE_LOG(LogClass, Log, TEXT("UpdateFreq: %f"), MinNetUpdateFrequency);
	if (Role == ROLE_Authority)
	{
		UE_LOG(LogClass, Log, TEXT("ChangedAmmoFrom - Server"));
	}
	else if (Role == ROLE_AutonomousProxy)
	{
		UE_LOG(LogClass, Log, TEXT("ChangedAmmoFrom - Owner"));
	}
	else
	{
		UE_LOG(LogClass, Log, TEXT("ChangedAmmoFrom - Unknown"));
	}
	AmmoLeftInMag = Mag;
	TotalAmmo = Ammo;
	
}

bool AGun::ServerChangeAmmo_Validate(int32 Ammo, int32 Mag)
{
	return true;
}

void AGun::ServerChangeAmmo_Implementation(int32 Ammo, int32 Mag)
{

	if (Role == ROLE_Authority)
	{
		UE_LOG(LogClass, Log, TEXT("Server Calling ChangeAmmo()"));
		ChangeAmmo(Ammo, Mag);
	}
	else
	{
		UE_LOG(LogClass, Log, TEXT("Unknown User ChangeAmmo()"));
	}

}


void AGun::SetActive(bool NewWeaponState)
{
	if (Role == ROLE_Authority)
	{
		bIsActive = NewWeaponState;
	}
}

void AGun::WasCollected_Implementation()
{
	// log a debug message
	//UE_LOG(LogClass, Log, TEXT("AGun::WasCollected_Implementation %s"), *GetName());
}

bool AGun::SetCanFire_Validate(bool NewCanFire)
{
	return true;
}


void AGun::SetCanFire_Implementation(bool NewCanFire)
{
	CanFire = NewCanFire;
}
bool AGun::SetIsReloading_Validate(bool NewReloading)
{
	return true;
}


void AGun::SetIsReloading_Implementation(bool NewReloading)
{
	IsReloading = NewReloading;
}

void AGun::ClientStartReload()
{
	

	
	
	//StartReload();
	
}
void AGun::ClientEndReload()
{

}

void AGun::StartReload_Implementation()
{
	if (!IsReloading)
	{
		if (TotalAmmo > 0) {
			SetCanFire(false);
			GetWorld()->GetTimerManager().SetTimer(ReloadTimer, this, &AGun::EndReload, ReloadTime, false);
			SetIsReloading(true);
		}
	}
	else
	{
		UE_LOG(LogClass, Log, TEXT("AlreadyReloading"));
	}
}
void AGun::EndReload_Implementation()
{

	GetWorld()->GetTimerManager().ClearTimer(ReloadTimer);
	if (Role == ROLE_Authority)
	{
		UE_LOG(LogClass, Log, TEXT("SERVER RELOAD"));

	//ClientStartReload();
	
	
	}
	if (AmmoLeftInMag > 0)
	{
		if (TotalAmmo + AmmoLeftInMag >= MagazineSize)
		{
			ChangeAmmo(TotalAmmo + AmmoLeftInMag - MagazineSize, MagazineSize);
		}
		else
		{
			ChangeAmmo(0, TotalAmmo + AmmoLeftInMag);
		}
	}
	else
	{
		if (TotalAmmo >= MagazineSize)
		{
			ChangeAmmo(TotalAmmo - MagazineSize, MagazineSize);
		}
		else
		{
			ChangeAmmo(0, TotalAmmo);
		}
	}
	//ChangeAmmo(TotalAmmo, MagazineSize);
	SetIsReloading(false);
	SetCanFire(true);
	
	UE_LOG(LogClass, Log, TEXT("EndedReload"));
	
}
void AGun::DroppedBy(APawn * Pawn)
{
	
	if (Role == ROLE_Authority)
	{
		MinNetUpdateFrequency = 2.0f;
		ClientOnDroppedBy(Pawn);
		DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
		GetStaticMeshComponent()->SetSimulatePhysics(true);
		
	}
}
void AGun::SecondaryDroppedBy(APawn * Pawn)
{

	if (Role == ROLE_Authority)
	{
		
		GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//GetStaticMeshComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);
		Pawn->MoveIgnoreActorAdd(this);
		ClientOnDroppedBy(Pawn);
		
		GetStaticMeshComponent()->IgnoreActorWhenMoving(Pawn,true);
		//GetStaticMeshComponent()->SetSimulatePhysics(true);
	}
}

void AGun::PickedUpBy(APawn * Pawn)
{
	if (Role == ROLE_Authority)
	{
		//store the pawn who picked up the pickup
		WeaponInstigator = Pawn;
		//Notify clients of the picked up action
		ClientOnPickedUpBy(Pawn);
		MinNetUpdateFrequency = 15.0f;
		if (AFPSCharacter* const AttachedPlayer = Cast<AFPSCharacter>(Pawn)) {
			UE_LOG(LogClass, Log, TEXT("%s was picked up by %s"), *GetName(), *Pawn->GetName());


			//this->AttachRootComponentTo(AttachedPlayer->FPSMesh, FName(TEXT("WeaponLocation")));
			//this->AttachRootComponentTo(atta)
			//FRotator MyRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), AttachedPlayer->FPSCameraComponent->GetComponentLocation() + AttachedPlayer->FPSCameraComponent->GetForwardVector() * 200);
			//SetActorRotation(FRotator(MyRotation.Pitch,MyRotation.Yaw,MyRotation.Roll));
			//SetActorRotation(MyRotation);

		}
		// give clients time to play vfx, etc... before destroying the battery
		//SetLifeSpan(2.0f);

	}
}

void AGun::OnRep_IsActive()
{

}

void AGun::ClientOnPickedUpBy_Implementation(APawn* Pawn)
{
	//store the pawn who picked up pickup (client
	WeaponInstigator = Pawn;
	//fire the blueprint native event, which itself cannot be replicated
	WasCollected();
}
void AGun::ClientOnDroppedBy_Implementation(APawn* Pawn)
{

	WeaponInstigator = NULL;

}


