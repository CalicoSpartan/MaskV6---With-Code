// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSCharacter.h"
#include "AssaultRifle.h"
#include "FPSPlayerController.h"
#include "Net/UnrealNetwork.h"


// Sets default values
AFPSCharacter::AFPSCharacter()
{
	CurrentState = EPlayerState::EPlayerPlaying;
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	//bReplicates = true;
	// Create a first person camera component.
	FPSCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	// Attach the camera component to our capsule component.
	FPSCameraComponent->SetupAttachment(GetCapsuleComponent());

	// Position the camera slightly above the eyes.
	FPSCameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f + BaseEyeHeight));
	// Allow the pawn to control camera rotation.
	FPSCameraComponent->bUsePawnControlRotation = true;
	DefaultFOV = FPSCameraComponent->FieldOfView;
	// Create a first person mesh component for the owning player.
	FPSMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	// Only the owning player sees this mesh.
	FPSMesh->SetOnlyOwnerSee(true);
	// Attach the FPS mesh to the FPS camera.
	FPSMesh->SetupAttachment(FPSCameraComponent);
	// Disable some environmental shadowing to preserve the illusion of having a single mesh.
	FPSMesh->bCastDynamicShadow = false;
	FPSMesh->CastShadow = false;

	// The owning player doesn't see the regular (third-person) body mesh.
	GetMesh()->SetOwnerNoSee(true);

	CollectionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollectionBox"));
	CollectionBox->SetupAttachment(RootComponent);
	CollectionBox->bGenerateOverlapEvents = true;

	GrenadeDetectionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("GrenadeDetectionBox"));
	GrenadeDetectionBox->SetupAttachment(RootComponent);
	GrenadeDetectionBox->bGenerateOverlapEvents = true;







	//set base weapon values
	bCanSwitchWeapon = true;
	IsFiring = false;
	HasWeapon = false;
	InitialHealth = 100.0f;
	CurrentHealth = InitialHealth;
	HealthPercentage = 1.0f;
	

}

bool AFPSCharacter::DestroySelf_Validate()
{
	return true;
}
void AFPSCharacter::DestroySelf_Implementation()
{
	if (Role == ROLE_Authority)
	{
		Destroy();
	}
}

void AFPSCharacter::ShowEnemyName_Implementation(class AFPSCharacter* Enemy)
{
	ShowEnemyNameBluePrint(Enemy);
}

void AFPSCharacter::Update()
{
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	FHitResult hit;
	
	if (GetWorld()->LineTraceSingleByChannel(hit, FPSCameraComponent->GetComponentLocation(), FPSCameraComponent->GetComponentLocation() + FPSCameraComponent->GetForwardVector() * 1800.0f, ECollisionChannel::ECC_Visibility, QueryParams))
	{
		if (AFPSCharacter* HitCharacter = Cast<AFPSCharacter>(hit.GetActor()))
		{
			if (AFPSPlayerState* HitPlayerState = Cast<AFPSPlayerState>(HitCharacter->PlayerState))
			{
				ShowEnemyName(HitCharacter);
			}

		}
	}
}

void AFPSCharacter::AddTeamColor_Implementation()
{
	TriggerAddColorBlueprint();
}
void AFPSCharacter::TriggerUpdateUIBlueprint_Implementation() {

}
void AFPSCharacter::TriggerAddColorBlueprint_Implementation()
{

}



void AFPSCharacter::TriggerUpdateUI()
{

	TriggerUpdateUIBlueprint();

}

void AFPSCharacter::TriggerAddUIBlueprint_Implementation() {

}

bool AFPSCharacter::TriggerAddUI_Validate() {
	return true;
}

void AFPSCharacter::TriggerAddUI_Implementation()
{

	TriggerAddUIBlueprint();

}

void AFPSCharacter::TriggerDeathUIBlueprint_Implementation() {

}


bool AFPSCharacter::TriggerDeathUI_Validate() {
	return true;
}

void AFPSCharacter::TriggerDeathUI_Implementation()
{

	TriggerDeathUIBlueprint();

}

void AFPSCharacter::DropEquipment_Implementation()
{
	if (Grenades.Num() > 0)
	{
		for (int32 i = 0; i < Grenades.Num(); ++i)
		{
			if (ABaseGrenade* const frag = Cast<ABaseGrenade>(Grenades[i]))
			{
				ABaseGrenade* Grenade = GetWorld()->SpawnActor<ABaseGrenade>(CurrentGrenade, GetActorLocation() + GetActorForwardVector() * 100.0, FRotator::ZeroRotator);
				//Grenades.RemoveAt(i);
			}
		}
		Grenades.Empty();
	}
}

bool AFPSCharacter::ServerDropEquipment_Validate()
{
	return true;
}
void AFPSCharacter::ServerDropEquipment_Implementation()
{
	UE_LOG(LogClass, Log, TEXT("Client called ServerDropEquipment"));
	DropEquipment();
}
bool AFPSCharacter::ClientDropEquipment_Validate()
{
	return true;
}
void AFPSCharacter::ClientDropEquipment_Implementation()
{
	UE_LOG(LogClass, Log, TEXT("Server called ClientDropEquipment"));
	DropEquipment();
}


EPlayerState AFPSCharacter::GetCurrentState() const
{
	return CurrentState;
}
void AFPSCharacter::SetCurrentState(EPlayerState NewState)
{
	if (Role == ROLE_Authority) {
		UE_LOG(LogClass, Log, TEXT("state changed"));
		CurrentState = NewState;
		if (NewState == EPlayerState::EPlayerWaiting)
		{
			GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
		}
		else if (NewState == EPlayerState::EPlayerPlaying)
		{
			GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		}

	}
}
/*
bool AFPSCharacter::ServerOnPlayerDeath_Validate()
{
	return true;
}

void AFPSCharacter::ServerOnPlayerDeath_Implementation()
{
	if (Role == ROLE_Authority)
	{
		OnPlayerDeath();
	}
}
*/



void AFPSCharacter::OnPlayerDeath_Implementation()
{
	DropWeapon();
	CollectionBox->bGenerateOverlapEvents = false;
	GrenadeDetectionBox->bGenerateOverlapEvents = false;
	TriggerDeathUI();
	// disconnect controller from pawn
	DetachFromControllerPendingDestroy();
	MyDeathLocation = GetActorLocation();

	if (GetMesh())
	{
		static FName CollisionProfileName(TEXT("Ragdoll"));
		GetMesh()->SetCollisionProfileName(CollisionProfileName);
	}
	SetActorEnableCollision(true);
	//ragdoll (init physics)
	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->WakeAllRigidBodies();
	GetMesh()->bBlendPhysics = true;
	GetMesh()->SetOwnerNoSee(false);

	//disable movement
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->SetComponentTickEnabled(false);
	//disable collisions on the capsule
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
	IsDead = true;
	SetCurrentState(EPlayerState::EPlayerDead);
	/*
	if (Role == ROLE_Authority)
	{
		UE_LOG(LogClass, Log, TEXT("Server Dropped Stuff"));
		DropWeapon();
		DropEquipment();
	}
	else
	{
		ServerDropWeapon();
		ServerDropEquipment();
		UE_LOG(LogClass, Log, TEXT("Im A Client"));
	}
	*/
	/*
	//////drop weapon
	if (AFPSGameState* const GameState = Cast<AFPSGameState>(GetWorld()->GetGameState()))
	{
		if (GameState->GetCurrentState() == EGamePlayState::EPlaying)
		{



			if (CurrentPrimary != NULL)
			{
				if (AGun* Gun = Cast<AGun>(CurrentPrimary))
				{
					Gun->DroppedBy(this);
					CurrentPrimary = NULL;

				}
			}

		}
	}

	//////drop equipment
	if (Grenades.Num() > 0)
	{
		for (int32 i = 0; i < Grenades.Num(); ++i)
		{
			if (AFragGrenade* const frag = Cast<AFragGrenade>(Grenades[i]))
			{
				ABaseGrenade* Grenade = GetWorld()->SpawnActor<ABaseGrenade>(FragSUB, GetActorLocation() + GetActorForwardVector() * 100.0, FRotator::ZeroRotator);
				//Grenades.RemoveAt(i);
			}
		}
		Grenades.Empty();
	}
	*/

	if (LastHitForce != NULL && LastHitDirection != FVector::ZeroVector)
	{
		if (LastHitBone != "")
		{
			GetMesh()->AddForce(LastHitDirection * LastHitForce, LastHitBone);
		}
		else
		{
			GetMesh()->AddForce(LastHitDirection);
		}
	}
	if (Role == ROLE_Authority)
	{
		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			if (Iterator->Get()->AcknowledgedPawn == this) {
				APlayerController* PController = Iterator->Get();
				AFPSPlayerController* PC = Cast<AFPSPlayerController>(PController);
				if (PC)
				{

					PC->OnKilled();
				}
				else
				{
					UE_LOG(LogClass, Log, TEXT("cast failed"));
				}
			}
		}
	}


}

float AFPSCharacter::GetCurrentHealth()
{
	return CurrentHealth;
}

void AFPSCharacter::BeginRechargeHealth()
{
	if (!IsDead)
	{
		GetWorld()->GetTimerManager().SetTimer(HealthRechargeDELTATimer, this, &AFPSCharacter::RechargeHealth, .001f, true);
	}
}


void AFPSCharacter::RechargeHealth()
{
	if (GetCurrentHealth() < GetInitialHealth())
	{
		if (GetCurrentHealth() + DELTA * HealthRechargeSpeed > GetInitialHealth())
		{
			ServerChangeHealthBy(GetInitialHealth() - GetCurrentHealth());
		}
		else
		{
			ServerChangeHealthBy(.01 * HealthRechargeSpeed);
		}
	}
	if (GetCurrentHealth() > GetInitialHealth())
	{
		ServerChangeHealthBy(GetInitialHealth() - GetCurrentHealth());
		GetWorld()->GetTimerManager().ClearTimer(HealthRechargeTimer);
		GetWorld()->GetTimerManager().ClearTimer(HealthRechargeDELTATimer);
	}
}

void AFPSCharacter::ServerChangeHealthBy_Implementation(float delta)
{
	CurrentHealth += delta;
	HealthPercentage = CurrentHealth / InitialHealth;
	if (delta > 0.0f)
	{

	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(HealthRechargeTimer);
		GetWorld()->GetTimerManager().ClearTimer(HealthRechargeDELTATimer);
		GetWorld()->GetTimerManager().SetTimer(HealthRechargeTimer, this, &AFPSCharacter::BeginRechargeHealth, HealthRechargeDelay, false);

	}
	if (Role == ROLE_Authority)
	{
		TriggerUpdateUI();
	}

}
float AFPSCharacter::GetInitialHealth()
{
	return InitialHealth;
}

bool  AFPSCharacter::ServerUpdateRotation_Validate(FRotator Rotation)
{

	return true;

}


void  AFPSCharacter::ServerUpdateRotation_Implementation(FRotator Rotation)
{

	FPSCameraComponent->SetWorldRotation(Rotation);

}


void AFPSCharacter::OnShoot()
{

	if (CurrentPrimary != NULL) {
		SetShotMULTI(ShotStartLocation, ShotDirection);
		ServerOnShoot();

	}

}
void AFPSCharacter::OnStopShoot()
{
	ServerOnStopShoot();
}





void AFPSCharacter::AddRecoil_Implementation()
{
	if (CurrentPrimary != NULL)
	{
		float TrueRecoilValue = 0.0f;
		if (IsZoomed)
		{
			TrueRecoilValue = CurrentPrimary->ZoomRecoilValue / 8.0f;
		}
		else
		{
			TrueRecoilValue = CurrentPrimary->RecoilValue / 8.0f;
		}

		float RandomPitchValue = FMath::RandRange(TrueRecoilValue * 0.7f, TrueRecoilValue * 1.3f);
		
		float RandomYawValue = FMath::RandRange(-TrueRecoilValue / 2.0f, +TrueRecoilValue / 2.0f);
	
		
		AddControllerPitchInput(-RandomPitchValue);
		AddControllerYawInput(RandomYawValue);




	}

}


bool AFPSCharacter::ServerOnZoom_Validate()
{
	return true;
}
void AFPSCharacter::ServerOnZoom_Implementation()
{
	if (CurrentPrimary != NULL)
	{



		SetIsZoomed(true);
		GetWorld()->GetTimerManager().SetTimer(ZoomTimer, this, &AFPSCharacter::Zoom, .005f, true);



	}
}
bool AFPSCharacter::ServerOnStopZoom_Validate()
{
	return true;
}
void AFPSCharacter::ServerOnStopZoom_Implementation()
{
	GetWorld()->GetTimerManager().ClearTimer(ZoomTimer);
	SetIsZoomed(false);


	GetWorld()->GetTimerManager().SetTimer(ZoomTimer, this, &AFPSCharacter::Zoom, .005f, true);


}
bool AFPSCharacter::Zoom_Validate()
{
	return true;
}

void AFPSCharacter::Zoom_Implementation()
{
	//UE_LOG(LogClass, Log, TEXT("ZOOMING"))
	if (CurrentPrimary != NULL)
	{
		if (IsZoomed == true)
		{
			FPSCameraComponent->SetFieldOfView(FMath::Lerp(FPSCameraComponent->FieldOfView, CurrentPrimary->ZoomFOV, CurrentPrimary->ZoomRate));
		}
		else
		{
			if (FMath::IsNearlyEqual(FPSCameraComponent->FieldOfView, DefaultFOV, .01f))
			{
				GetWorld()->GetTimerManager().ClearTimer(ZoomTimer);
				UE_LOG(LogClass, Log, TEXT("FullyZoomedOut"))
			}
			else
			{
				FPSCameraComponent->SetFieldOfView(FMath::Lerp(FPSCameraComponent->FieldOfView, DefaultFOV, CurrentPrimary->ZoomRate));
			}
		}
	}
	else
	{
		if (FMath::IsNearlyEqual(FPSCameraComponent->FieldOfView, DefaultFOV, 0.1f))
		{
			GetWorld()->GetTimerManager().ClearTimer(ZoomTimer);
		}
		else
		{
			FPSCameraComponent->SetFieldOfView(FMath::Lerp(FPSCameraComponent->FieldOfView, DefaultFOV, CurrentPrimary->ZoomRate));
		}
		
	}

}

bool AFPSCharacter::ServerOnStopShoot_Validate()
{
	return true;
}
void AFPSCharacter::ServerOnStopShoot_Implementation()
{
	if (Role == ROLE_Authority) {
		IsFiring = false;
		UWorld* World = GetWorld();
		check(World);
		GetWorld()->GetTimerManager().ClearTimer(WeaponFireRateTimer);
	}
}

void AFPSCharacter::FireAgain()
{
	ServerOnShoot();
}



bool AFPSCharacter::SetIsZoomed_Validate(bool zoomvalue)
{
	return true;
}
void AFPSCharacter::SetIsZoomed_Implementation(bool zoomvalue)
{
	IsZoomed = zoomvalue;
	
}
bool AFPSCharacter::ServerOnShoot_Validate()
{
	return true;
}
bool AFPSCharacter::GenerateShotMULTI_Validate()
{
	return true;
}
bool AFPSCharacter::SetShotMULTI_Validate(FVector Location, FVector Direction)
{
	return true;
}

void AFPSCharacter::SetShotMULTI_Implementation(FVector Location, FVector Direction)
{
	ShotStartLocation = Location;
	ShotDirection = Direction;
}
void AFPSCharacter::SetShot(FVector Location, FVector Direction)
{
	SetShotMULTI(Location, Direction);
}

void AFPSCharacter::GenerateShotMULTI_Implementation()
{
	GenerateShot();
}


void AFPSCharacter::SetIsRecoiling_Implementation(bool recoilvalue)
{
	IsRecoiling = recoilvalue;
}


void AFPSCharacter::ServerOnShoot_Implementation()
{
	if (AFPSGameState* const GameState = Cast<AFPSGameState>(GetWorld()->GetGameState()))
	{
		if (GameState->GetCurrentState() == EGamePlayState::EPlaying)
		{
			if (Role = ROLE_Authority) {
				UWorld* World = GetWorld();

				check(World);

				FHitResult hit;
				FCollisionQueryParams QueryParams;
				QueryParams.AddIgnoredActor(this);
				if (CurrentPrimary->AmmoLeftInMag > 0 && IsFiring == false && CurrentPrimary->CanFire == true) {
					AddRecoil();

					UE_LOG(LogClass, Log, TEXT("Max Spread: %f"), CurrentPrimary->MaxSpread);
					UE_LOG(LogClass, Log, TEXT("Accuracy value: %f"), AccuracySpreadValue);
					if (CurrentPrimary->IsProjectile == false) {
						if (CurrentPrimary->IsAutomatic == true)
						{
							GetWorld()->GetTimerManager().SetTimer(WeaponFireRateTimer, this, &AFPSCharacter::FireAgain, CurrentPrimary->FireRate, false);
						}

						FVector BulletDestination;
						FVector TestBulletDestination = FVector(FPSCameraComponent->GetComponentLocation() + (FPSCameraComponent->GetForwardVector() * CurrentPrimary->Range));
						/*
						if (World->LineTraceSingleByChannel(testhit,FPSCameraComponent->GetComponentLocation(), FPSCameraComponent->GetComponentLocation() + (FPSCameraComponent->GetForwardVector() * CurrentPrimary->Range * AccuracyChange), ECollisionChannel::ECC_Visibility, QueryParams))
						{
						BulletDestination = testhit.Location;
						}
						else
						{
						BulletDestination = FPSCameraComponent->GetComponentLocation() + (FPSCameraComponent->GetForwardVector() * CurrentPrimary->Range * AccuracyChange);
						}
						*/

						if (ShotStartLocation != FVector::ZeroVector && ShotDirection != FVector::ZeroVector)
						{
							
							

							if (World->LineTraceSingleByChannel(hit, ShotStartLocation, ShotStartLocation + ShotDirection.GetSafeNormal() * CurrentPrimary->Range, ECollisionChannel::ECC_Visibility, QueryParams))
							{
								DrawDebugLine(GetWorld(), CurrentPrimary->MuzzleLocation->GetComponentLocation(), hit.Location, FColor::Red, false, 2.0f);
								if (AFPSCharacter* const hitplayer = Cast<AFPSCharacter>(hit.GetActor())) {
									UE_LOG(LogClass, Log, TEXT("Distance: %f"), FVector::Dist(hit.Location, CurrentPrimary->MuzzleLocation->GetComponentLocation()));
									float ShotDistance = FVector::Dist(hit.Location, CurrentPrimary->MuzzleLocation->GetComponentLocation());
									float DamagePercent = 0;
									if (AFPSPlayerState* testps = Cast<AFPSPlayerState>(hitplayer->PlayerState))
									{
										if (testps->MyCharacter != NULL)
										{
											UE_LOG(LogClass, Log, TEXT("mycharacter: %s"), *testps->MyCharacter->GetName());
										}
										else
										{
											UE_LOG(LogClass, Log, TEXT("mycharacter is NULL"));
										}
									}
									if (ShotDistance <= CurrentPrimary->PreferredRange)
									{
										if (hit.BoneName == FName("head"))
										{
											UE_LOG(LogClass, Log, TEXT("HEADSHOT"));
											DamagePercent = CurrentPrimary->HeadShotIncrease;
										}
										else
										{
											DamagePercent = 1.0f;
										}
									}
									else
									{
										float WeaponRangeDifference = CurrentPrimary->Range - CurrentPrimary->PreferredRange;
										float ShotRangeDifference = CurrentPrimary->Range - ShotDistance;
										float OppositePercentToZeroDamage = ShotRangeDifference / WeaponRangeDifference;
										if (hit.BoneName == FName("head"))
										{
											UE_LOG(LogClass, Log, TEXT("HEADSHOT"));
											DamagePercent = roundf(OppositePercentToZeroDamage * CurrentPrimary->HeadShotIncrease * 100) / 100;
										}
										else
										{
											DamagePercent = roundf(OppositePercentToZeroDamage * 100) / 100;

										}
									}
									UE_LOG(LogClass, Log, TEXT("DamagePercent: %f"), DamagePercent);
									hitplayer->Shooter = this;
									FVector BulletTrailDir = FVector(hit.Location - hit.TraceStart).GetSafeNormal();
									hitplayer->SetHitData(CurrentPrimary->BulletForce, hit.BoneName, BulletTrailDir);
									hitplayer->ServerChangeHealthBy(-CurrentPrimary->BulletDamage * DamagePercent);
									hitplayer->TriggerUpdateUI();
									
									//UE_LOG(LogClass, Log, FString::SanitizeFloat(hitplayer->GetCurrentHealth());
									//hitplayer->HealthPercentage = hitplayer->GetCurrentHealth() / hitplayer->GetInitialHealth();
									GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, FString::SanitizeFloat(hitplayer->GetCurrentHealth()));
									GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, FString::SanitizeFloat(hitplayer->HealthPercentage));
									//UE_LOG(LogClass, Log, TEXT("%s health is now %s"), *hitplayer->GetName(), FString::SanitizeFloat(hitplayer->GetCurrentHealth()));
								}

							}
							else
							{
								DrawDebugLine(GetWorld(), CurrentPrimary->MuzzleLocation->GetComponentLocation(), ShotStartLocation + ShotDirection.GetSafeNormal() * CurrentPrimary->Range, FColor::Red, false, 2.0f);
							}
							
						}
						


					}
					if (Role == ROLE_Authority)
					{
						CurrentPrimary->ChangeAmmo(CurrentPrimary->TotalAmmo, CurrentPrimary->AmmoLeftInMag - 1);
					}
					
					else
					{
						UE_LOG(LogClass, Log, TEXT("ServerOnShoot(): Somehow we are a client"));
					}
					
					if (IsZoomed == true)
					{
						if (simASV <= CurrentPrimary->ZoomMaxSpread)
						{
							simASV += CurrentPrimary->ZoomAccuracySpreadIncrease;
							
						}
					}
					else
					{
						if (simASV <= CurrentPrimary->MaxSpread)
						{
							simASV += CurrentPrimary->AccuracySpreadIncrease;
							
						}
					}
					if (AccuracySpreadValue <= CurrentPrimary->MaxSpread)
					{
						AccuracySpreadValue += CurrentPrimary->AccuracySpreadIncrease;
						
					}

					if (Left_CHLocation != FVector::ZeroVector && Right_CHLocation != FVector::ZeroVector && Top_CHLocation != FVector::ZeroVector && Bottom_CHLocation != FVector::ZeroVector)
					{
						/*
						DrawDebugLine(GetWorld(), Left_CHLocation, Left_CHLocation + FPSCameraComponent->GetForwardVector() * CurrentPrimary->Range, FColor::Red, true);
						DrawDebugLine(GetWorld(), Right_CHLocation, Right_CHLocation + FPSCameraComponent->GetForwardVector() * CurrentPrimary->Range, FColor::Red, true);
						DrawDebugLine(GetWorld(), Top_CHLocation, Top_CHLocation + FPSCameraComponent->GetForwardVector() * CurrentPrimary->Range, FColor::Red, true);
						DrawDebugLine(GetWorld(), Bottom_CHLocation, Bottom_CHLocation + FPSCameraComponent->GetForwardVector() * CurrentPrimary->Range, FColor::Red, true);
						*/


						if (Left_CHDirection != FVector::ZeroVector && Right_CHDirection != FVector::ZeroVector && Top_CHDirection != FVector::ZeroVector && Bottom_CHDirection != FVector::ZeroVector)
						{
							/*
							DrawDebugLine(GetWorld(), Left_CHLocation, Left_CHLocation + Left_CHDirection.GetSafeNormal() * CurrentPrimary->Range, FColor::Red, true);
							DrawDebugLine(GetWorld(), Right_CHLocation, Right_CHLocation + Right_CHDirection.GetSafeNormal() * CurrentPrimary->Range, FColor::Red, true);
							DrawDebugLine(GetWorld(), Top_CHLocation, Top_CHLocation + Top_CHDirection.GetSafeNormal() * CurrentPrimary->Range, FColor::Red, true);
							DrawDebugLine(GetWorld(), Bottom_CHLocation, Bottom_CHLocation + Bottom_CHDirection.GetSafeNormal() * CurrentPrimary->Range, FColor::Red, true);
							*/
						}
					}


				}
				if (CurrentPrimary->AmmoLeftInMag <= 0 && CurrentPrimary->TotalAmmo > 0)
				{
					ServerReload();
				}
			}
		}
	}

}


// Called when the game starts or when spawned
void AFPSCharacter::BeginPlay()
{
	Super::BeginPlay();
	CurrentState = EPlayerState::EPlayerPlaying;
	if (AFPSPlayerState* myPS = Cast<AFPSPlayerState>(PlayerState))
	{
		myPS->BeginPlaySetMyCharacter(this);
	}
	switch (GetCurrentState())
	{
	case EPlayerState::EPlayerWaiting:

		break;
	case EPlayerState::EPlayerPlaying:


		break;
	case EPlayerState::EPlayerDead:

		break;
	default:
	case EPlayerState::EPlayerUnknown:
		break;
	}
	if (GEngine)
	{
		// Put up a debug message for five seconds. The -1 "Key" value (first argument) indicates that we will never need to update or refresh this message.
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("We are using FPSCharacter."));
	}
	GetWorld()->GetTimerManager().SetTimer(UpdateTimer, this, &AFPSCharacter::Update, UpdateDelay, true);
	bCanSwitchWeapon = true;
}


void AFPSCharacter::SetCanSwitchWeapon()
{
	bCanSwitchWeapon = true;
}

bool AFPSCharacter::SwitchWeapon_Validate()
{
	return true;
}

void AFPSCharacter::SwitchWeapon_Implementation()
{
	ServerSwitchWeapon();
}
void AFPSCharacter::ServerSwitchWeapon_Implementation()
{
	if (AFPSGameState* const GameState = Cast<AFPSGameState>(GetWorld()->GetGameState()))
	{
		if (GameState->GetCurrentState() == EGamePlayState::EPlaying)
		{
			if (CurrentPrimary != NULL)
			{
				if (MyWeapons.Num() > 1)
				{
					
					if (bCanSwitchWeapon == true)
					{
						
						int32 PrimaryIndex = MyWeapons.Find(CurrentPrimary);
						if (PrimaryIndex == NULL)
						{
							//UE_LOG(LogClass, Log, TEXT("Couldn't find weapon"));


						}
						if (PrimaryIndex != MyWeapons.Num() - 1)
						{
							if (AGun* SecondaryGun = Cast<AGun>(MyWeapons[PrimaryIndex + 1]))
							{
								CurrentPrimary->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
								CurrentPrimary->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName(TEXT("WeaponSocket1")));
								SecondaryInstance = CurrentPrimary;
								//this->AttachRootComponentTo(AttachedPlayer->FPSMesh, FName(TEXT("WeaponLocation")),EAttachLocation::SnapToTarget);
								SecondaryGun->AttachToComponent(FPSMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, FName(TEXT("WeaponLocation")));
								PrimaryInstance = SecondaryGun;
								CurrentPrimary = SecondaryGun;

							}
						}
						else
						{
							if (AGun* SecondaryGun = Cast<AGun>(MyWeapons[0]))
							{
								CurrentPrimary->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
								CurrentPrimary->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName(TEXT("WeaponSocket1")));
								SecondaryInstance = CurrentPrimary;
								//this->AttachRootComponentTo(AttachedPlayer->FPSMesh, FName(TEXT("WeaponLocation")),EAttachLocation::SnapToTarget);
								SecondaryGun->AttachToComponent(FPSMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, FName(TEXT("WeaponLocation")));
								PrimaryInstance = SecondaryGun;
								CurrentPrimary = SecondaryGun;
							}
						}
						bCanSwitchWeapon = false;
						GetWorld()->GetTimerManager().SetTimer(WeaponSwitchDelayTimer, this, &AFPSCharacter::SetCanSwitchWeapon, WeaponSwitchDelay, false);
					}

				}
				else
				{
					//if (PickingUpWeapon == true)
					//{
					//CurrentPrimary->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
					//CurrentPrimary->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName(TEXT("WeaponSocket1")));
						
					//}
				}
			}
		}
	}

}

bool AFPSCharacter::PickupWeapon_Validate()
{
	return true;
}

void AFPSCharacter::PickupWeapon_Implementation()
{
	ServerPickupWeapon();
}

void AFPSCharacter::DeadDropWeapon()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	if (PrimaryInstance != NULL)
	{
		
		AGun* DroppedPrimary = GetWorld()->SpawnActor<AGun>(PrimaryInstance->GetClass(), MyDeathLocation, FRotator::ZeroRotator,SpawnParams);
		if (Role == ROLE_Authority)
		{
			UE_LOG(LogClass, Log, TEXT("Changing ammo from server"));
			DroppedPrimary->ChangeAmmo(DroppedPrimary->MaxAmmo - (PrimaryInstance->MaxAmmo - PrimaryInstance->TotalAmmo), DroppedPrimary->MagazineSize - (PrimaryInstance->MagazineSize - PrimaryInstance->AmmoLeftInMag));
		}
		else
		{
			UE_LOG(LogClass, Log, TEXT("Changing ammo from client"));
			DroppedPrimary->ServerChangeAmmo(DroppedPrimary->MaxAmmo - (PrimaryInstance->MaxAmmo - PrimaryInstance->TotalAmmo), DroppedPrimary->MagazineSize - (PrimaryInstance->MagazineSize - PrimaryInstance->AmmoLeftInMag));
		}
	}
	if (SecondaryInstance != NULL)
	{

		AGun* DroppedSecondary = GetWorld()->SpawnActor<AGun>(SecondaryInstance->GetClass(), MyDeathLocation, FRotator::ZeroRotator,SpawnParams);
		if (Role == ROLE_Authority)
		{
			UE_LOG(LogClass, Log, TEXT("Changing ammo from server"));
			DroppedSecondary->ChangeAmmo(DroppedSecondary->MaxAmmo - (SecondaryInstance->MaxAmmo - SecondaryInstance->TotalAmmo), DroppedSecondary->MagazineSize - (SecondaryInstance->MagazineSize - SecondaryInstance->AmmoLeftInMag));
		}
		else
		{
			UE_LOG(LogClass, Log, TEXT("Changing ammo from client"));
			DroppedSecondary->ServerChangeAmmo(DroppedSecondary->MaxAmmo - (SecondaryInstance->MaxAmmo - SecondaryInstance->TotalAmmo), DroppedSecondary->MagazineSize - (SecondaryInstance->MagazineSize - SecondaryInstance->AmmoLeftInMag));
		}
	}

	for (int32 i = 0; i < Grenades.Num(); ++i)
	{
		if (ABaseGrenade* gren = Cast<ABaseGrenade>(Grenades[i]))
		{
			ABaseGrenade* DroppedGrenade = GetWorld()->SpawnActor<ABaseGrenade>(gren->GetClass(), MyDeathLocation, FRotator::ZeroRotator,SpawnParams);
		}
	}
	
}
void AFPSCharacter::DropWeapon()//_Implementation()
{
	if (AFPSGameState* const GameState = Cast<AFPSGameState>(GetWorld()->GetGameState()))
	{
		if (GameState->GetCurrentState() == EGamePlayState::EPlaying)
		{
			
			
			if (IsDead)
			{
				//GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);
				GetWorld()->GetTimerManager().SetTimer(DeadWeaponDropTimer, this, &AFPSCharacter::DeadDropWeapon, DeadWeaponDropDelay, false);
				for (int32 i = 0; i < MyWeapons.Num(); ++i)
				{
					if (AGun* Gun = Cast<AGun>(MyWeapons[i]))
					{

						
						Gun->Destroy();
						/*
						if (Gun != CurrentPrimary)
						{
							UE_LOG(LogClass, Log, TEXT("SecondaryDropping: %s"), *Gun->GetName());
							Gun->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
							Gun->SecondaryDroppedBy(this);
						}
						else
						{
							UE_LOG(LogClass, Log, TEXT("PrimaryDropping: %s"), *Gun->GetName());
							Gun->DroppedBy(this);
						}
						//Gun->AttachToComponent(FPSMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, FName(TEXT("WeaponLocation")));
						
						//Gun->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);
						*/

						
					}
				
				}
				MyWeapons.Empty();
			}
			else
			{

				if (CurrentPrimary != NULL)
				{
					if (AGun* Gun = Cast<AGun>(CurrentPrimary))
					{
						MyWeapons.Remove(CurrentPrimary);
						Gun->DroppedBy(this);
						CurrentPrimary = NULL;

					}
				}
			}
			
		}
	}
	
}
void AFPSCharacter::SetPickingUpWeapon_Implementation(bool IsPickingUpWeapon)
{
	PickingUpWeapon = IsPickingUpWeapon;
}
void AFPSCharacter::PickupEquipment()
{
	if (AFPSGameState* const GameState = Cast<AFPSGameState>(GetWorld()->GetGameState()))
	{
		if (GameState->GetCurrentState() == EGamePlayState::EPlaying)
		{


			TArray<AActor*> PossibleEquipment;
			CollectionBox->GetOverlappingActors(PossibleEquipment);
			bool FoundPossibleWeapon = false;
			FString PossibleGunName = "";
			for (int32 i = 0; i < PossibleEquipment.Num(); ++i)
			{

				if (AGun* const Gun = Cast<AGun>(PossibleEquipment[i]))
				{
					if (!Gun->IsPendingKill() && Gun->IsActive() && Gun->WeaponInstigator == NULL)
					{
						bool AlreadyHaveWeapon = false;
						if (MyWeapons.Num() > 0)
						{
							for (int32 i = 0; i < MyWeapons.Num(); ++i)
							{
								if (AGun* MyWeapon = Cast<AGun>(MyWeapons[i]))
								{



									if (MyWeapon != NULL)
									{
										if (Gun != MyWeapon)
										{
											if (MyWeapon->GunName != Gun->GunName && Gun != PrimaryInstance && Gun != SecondaryInstance)
											{
												if (!AlreadyHaveWeapon)
												{
													FoundPossibleWeapon = true;
													PossibleGunName = Gun->GunName.ToString();

												}
												else
												{
													AlreadyHaveWeapon = true;
												}
											}
											else
											{
												AlreadyHaveWeapon = true;
												//PickupWeaponText = "";

											}
											if (MyWeapon->GunName == Gun->GunName)
											{
												if (MyWeapon->TotalAmmo != MyWeapon->MaxAmmo)
												{
													int32 AmmoNeeded = MyWeapon->MaxAmmo - MyWeapon->TotalAmmo;
													if (Gun->TotalAmmo >= AmmoNeeded)
													{
														if (Role == ROLE_Authority)
														{
															UE_LOG(LogClass, Log, TEXT("This is Server Picking up Ammo"));
															Gun->ChangeAmmo(Gun->TotalAmmo - AmmoNeeded, Gun->MagazineSize);
															MyWeapon->ChangeAmmo(MyWeapon->MaxAmmo, MyWeapon->AmmoLeftInMag);
														}
														/*
														else
														{
															Gun->ServerChangeAmmo(Gun->TotalAmmo - AmmoNeeded, Gun->MagazineSize);
															MyWeapon->ServerChangeAmmo(MyWeapon->MaxAmmo, MyWeapon->AmmoLeftInMag);
														}
														*/


													}
													else
													{
														if (Role == ROLE_Authority)
														{
															UE_LOG(LogClass, Log, TEXT("This is Server Picking up Ammo"));
															MyWeapon->ChangeAmmo(MyWeapon->TotalAmmo + Gun->TotalAmmo, MyWeapon->AmmoLeftInMag);
															Gun->ChangeAmmo(0, Gun->AmmoLeftInMag);
														}
														/*
														else
														{
															MyWeapon->ServerChangeAmmo(MyWeapon->TotalAmmo + Gun->TotalAmmo, MyWeapon->AmmoLeftInMag);
															Gun->ServerChangeAmmo(0, Gun->AmmoLeftInMag);
														}
														*/

													}

												}
											}
										}
									}
								}
							}
						}
						else
						{
							FoundPossibleWeapon = true;
							PossibleGunName = Gun->GunName.ToString();
						}
						if (AlreadyHaveWeapon == true)
						{
							FoundPossibleWeapon = false;
						}
					}
				}

				if (ABaseGrenade* const Grenade = Cast<ABaseGrenade>(PossibleEquipment[i]))
				{
					if (!Grenade->IsPendingKill() && !Grenade->bPendingExplode)
					{
						CurrentGrenade = Grenade->GetClass();
						Grenades.Add(Grenade);
						Grenade->Destroy();
					}
				}
			}
			if (!FoundPossibleWeapon)
			{

				PickupWeaponText = "";
			
			}
			else
			{
				PickupWeaponText = "Press E to pickup " + PossibleGunName;
			}
			
		}
	}
	
}


bool AFPSCharacter::ServerDropWeapon_Validate()
{
	return true;
}

void AFPSCharacter::ServerDropWeapon_Implementation()
{
	DropWeapon();
}



bool AFPSCharacter::ServerPickupEquipment_Validate()
{
	return true;
}
void AFPSCharacter::ServerPickupEquipment_Implementation()
{
	PickupEquipment();
}

bool AFPSCharacter::ServerPickupWeapon_Validate()
{
	return true;
}

void AFPSCharacter::ServerPickupWeapon_Implementation()
{
	if (AFPSGameState* const GameState = Cast<AFPSGameState>(GetWorld()->GetGameState()))
	{
		if (GameState->GetCurrentState() == EGamePlayState::EPlaying)
		{
			SetPickingUpWeapon(false);
			//if (Role == ROLE_Authority)
			//{
				// Get all overlapping actors and store them in an array
			TArray<AActor*> CollectedActors;
			CollectionBox->GetOverlappingActors(CollectedActors);

			for (int i = 0; i < CollectedActors.Num(); ++i)
			{
				AGun* const Gun = Cast<AGun>(CollectedActors[i]);
				if (Gun != NULL && !Gun->IsPendingKill() && Gun->IsActive() && Gun->WeaponInstigator == NULL) {
					int32 WhereToInsert = 0;
					if (CurrentPrimary != NULL)
					{
						if (AGun* PrimaryGun = Cast<AGun>(CurrentPrimary))
						{


							if (Gun->GunName == PrimaryGun->GunName)
							{
								UE_LOG(LogClass, Log, TEXT("AlreadyHaveWeapon"));
								return;
							}
							else
							{
								if (MyWeapons.Num() >= MaxNumberOfWeapons)
								{
									DropWeapon();
								}
							}
						}
					}
					if (CurrentPrimary != NULL)
					{
						WhereToInsert = MyWeapons.Find(CurrentPrimary);

					}
					if (MyWeapons.Num() > 0)
					{
						MyWeapons.Insert(Gun, WhereToInsert);
					}
					else
					{
						MyWeapons.Insert(Gun, 0);
					}
					if (CurrentPrimary != NULL)
					{
						CurrentPrimary->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
						CurrentPrimary->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName(TEXT("WeaponSocket1")));
						SecondaryInstance = CurrentPrimary;
					}
					UE_LOG(LogClass,Log,TEXT("%s"),*Gun->GetClass()->GetName());
					PickupWeaponText = "";
					PrimaryInstance = Gun;
					CurrentPrimary = Gun;
					Gun->GetStaticMeshComponent()->SetSimulatePhysics(false);
					//this->AttachRootComponentTo(AttachedPlayer->FPSMesh, FName(TEXT("WeaponLocation")),EAttachLocation::SnapToTarget);
					Gun->AttachToComponent(FPSMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, FName(TEXT("WeaponLocation")));
					Gun->PickedUpBy(this);
					//SetPickingUpWeapon(true);
					
					UE_LOG(LogClass, Log, TEXT("Current Primary: %s"), *CurrentPrimary->GetName());
					HasWeapon = true;
					break;

				}
			}
			//}
			
		}
	}
}
void AFPSCharacter::SetHitData_Implementation(float Force, FName Bone, FVector Direction)
{
	LastHitForce = Force;
	LastHitBone = Bone;
	LastHitDirection = Direction;
}
void AFPSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFPSCharacter, GrenadeThrowStrength);
	DOREPLIFETIME(AFPSCharacter, PickupWeaponText);
	DOREPLIFETIME(AFPSCharacter, bCanSwitchWeapon);
	DOREPLIFETIME(AFPSCharacter, MyDeathLocation);
	DOREPLIFETIME(AFPSCharacter, IsDead);
	DOREPLIFETIME(AFPSCharacter, GrenadeThrowUpForce);
	DOREPLIFETIME(AFPSCharacter, simASV);
	DOREPLIFETIME(AFPSCharacter, FPSMesh);
	DOREPLIFETIME(AFPSCharacter, FPSCameraComponent);
	DOREPLIFETIME(AFPSCharacter, CurrentPrimary);
	DOREPLIFETIME(AFPSCharacter, AccuracySpreadValue);
	DOREPLIFETIME(AFPSCharacter, IsZoomed);
	DOREPLIFETIME(AFPSCharacter, LastHitBone);
	DOREPLIFETIME(AFPSCharacter, LastHitForce);
	DOREPLIFETIME(AFPSCharacter, LastHitDirection);
	DOREPLIFETIME(AFPSCharacter, bIsNearGrenade);
	DOREPLIFETIME(AFPSCharacter, Grenades);
	DOREPLIFETIME(AFPSCharacter, MyWeapons);
	DOREPLIFETIME(AFPSCharacter, PickingUpWeapon);
	DOREPLIFETIME(AFPSCharacter, Right_CHLocation);
	DOREPLIFETIME(AFPSCharacter, Top_CHLocation);
	DOREPLIFETIME(AFPSCharacter, Left_CHLocation);
	DOREPLIFETIME(AFPSCharacter, Bottom_CHLocation);
	DOREPLIFETIME(AFPSCharacter, Right_CHDirection);
	DOREPLIFETIME(AFPSCharacter, Left_CHDirection);
	DOREPLIFETIME(AFPSCharacter, Top_CHDirection);
	DOREPLIFETIME(AFPSCharacter, Bottom_CHDirection);
	DOREPLIFETIME(AFPSCharacter, ShotStartLocation);
	DOREPLIFETIME(AFPSCharacter, ShotDirection);
	DOREPLIFETIME(AFPSCharacter, ShotLocation_MaxX);
	DOREPLIFETIME(AFPSCharacter, ShotLocation_MinX);
	DOREPLIFETIME(AFPSCharacter, ShotLocation_MaxY);
	DOREPLIFETIME(AFPSCharacter, ShotLocation_MinY);
	
}

bool AFPSCharacter::GrenadeNearby_Validate()
{
	return true;
}
void AFPSCharacter::GrenadeNearby_Implementation()
{
	TArray<AActor*> PossibleGrenades;
	FString Array;
	
	GrenadeDetectionBox->GetOverlappingActors(PossibleGrenades);
	if (PossibleGrenades.Num() > 0)
	{
		bool GrenadeExist = false;
		for (int32 i = 0; i < PossibleGrenades.Num(); ++i)
		{
			Array.Append(PossibleGrenades[i]->GetName());
			if (ABaseGrenade* Grenade = Cast<ABaseGrenade>(PossibleGrenades[i]))
			{
				if (Grenade->bIsActive)
				{
					GrenadeExist = true;
					//GrenadeNearby(Grenade);
					bIsNearGrenade = true;
					//SetIsNearGrenade(true);
					float DirToGrenade = ((GetActorLocation() - Grenade->GetActorLocation()) * FVector(-1.0f, -1.0f, 0.0f)).GetSafeNormal().Rotation().Yaw;
					float ForwardVector = (FPSCameraComponent->GetForwardVector() * FVector(1.0f, 1.0f, 0.0f)).GetSafeNormal().Rotation().Yaw;
					GrenadeDegrees = DirToGrenade - ForwardVector - 90.0f;

					GrenadeDir = FVector2D(FMath::Cos(FMath::DegreesToRadians(GrenadeDegrees)), FMath::Sin(FMath::DegreesToRadians(GrenadeDegrees)));


					/*
					FVector2D DirectionToGrenade = -1.0f *  FVector2D(FVector2D(GetActorLocation().X, GetActorLocation().Y) - FVector2D(Grenade->GetActorLocation().X, Grenade->GetActorLocation().Y)).GetSafeNormal();
					FVector2D ForwardVector = FVector2D(FPSCameraComponent->GetForwardVector().X, FPSCameraComponent->GetForwardVector().Y).GetSafeNormal();
					float DotProduct = FVector2D::DotProduct(ForwardVector,DirectionToGrenade);
					GrenadeDegrees = FMath::RadiansToDegrees(FMath::Acos(DotProduct));
					GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Cyan, "Degrees:  " + FString::SanitizeFloat(GrenadeDegrees));
					*/
				}

				//UE_LOG(LogClass, Log, TEXT("DotProduct:  %f"),DotProduct);
			}
			else
			{
				//bIsNearGrenade = false;
				//SetIsNearGrenade(false);
			}

		}
		if (GrenadeExist == false)
		{
			bIsNearGrenade = false;
		}

	}
	else
	{
		bIsNearGrenade = false;
		//SetIsNearGrenade(false);
	}
	//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Orange, Array);

}

// Called every frame
void AFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TArray<AActor*> PossibleEquipment;
	CollectionBox->GetOverlappingActors(PossibleEquipment);
	if (PossibleEquipment.Num() > 0 && !IsDead)
	{
		//if (Role == ROLE_AutonomousProxy)
		//{
		if (Role == ROLE_AutonomousProxy)
		{
			ServerPickupEquipment();
		}
		else
		{
			PickupEquipment();
		}


		



		//}
	}
	else
	{
		PickupWeaponText = "";
	}
	GrenadeNearby();
	


	if (CurrentPrimary != NULL)
	{
		if (!IsZoomed)
		{
			if (simASV > CurrentPrimary->MinSpread)
			{


				simASV -= CurrentPrimary->AccuracySpreadDecrease;


			}
			if (simASV < CurrentPrimary->MinSpread)
			{

				simASV = CurrentPrimary->MinSpread;


			}

		}
		else
		{

			if (simASV > CurrentPrimary->ZoomMinSpread)
			{


				simASV -= CurrentPrimary->AccuracySpreadDecrease;


			}
			if (simASV < CurrentPrimary->ZoomMinSpread)
			{

				simASV = CurrentPrimary->ZoomMinSpread;


			}


		}

		if (AccuracySpreadValue > CurrentPrimary->MinSpread)
		{


			AccuracySpreadValue -= CurrentPrimary->AccuracySpreadDecrease;


		}
		if (AccuracySpreadValue < CurrentPrimary->MinSpread)
		{

			AccuracySpreadValue = CurrentPrimary->MinSpread;


		}

	}
	else
	{
		simASV = 0.0f;
		AccuracySpreadValue = 0.0f;
	}
	if (CurrentState == EPlayerState::EPlayerWaiting)
	{

		//GetCharacterMovement()->SetMovementModeEMovementMode
	}
	else if (CurrentState = EPlayerState::EPlayerPlaying)
	{

		
	}

	if (Role == ROLE_AutonomousProxy)
	{
		if (GetController())
		{
			ServerUpdateRotation(FPSCameraComponent->GetComponentRotation());
		}

	}
	//WeaponHoldLocation->SetWorldLocation(FPSMesh->GetSocketLocation(FName(TEXT("WeaponLocation"))));
}

// Called to bind functionality to input
void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up "movement" bindings.
	PlayerInputComponent->BindAxis("MoveForward", this, &AFPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFPSCharacter::MoveRight);

	// Set up "look" bindings.
	PlayerInputComponent->BindAxis("Turn", this, &AFPSCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &AFPSCharacter::AddControllerPitchInput);

	// Set up "action" bindings.
	PlayerInputComponent->BindAction("ThrowGrenade", IE_Pressed, this, &AFPSCharacter::ThrowGrenade);
	PlayerInputComponent->BindAction("SwitchWeapon", IE_Pressed, this, &AFPSCharacter::SwitchWeapon);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AFPSCharacter::ServerReload);
	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &AFPSCharacter::ServerOnZoom);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &AFPSCharacter::ServerOnStopZoom);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AFPSCharacter::StartJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AFPSCharacter::StopJump);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AFPSCharacter::PickupWeapon);
	PlayerInputComponent->BindAction("Shoot", IE_Pressed, this, &AFPSCharacter::OnShoot);
	PlayerInputComponent->BindAction("Shoot", IE_Released, this, &AFPSCharacter::OnStopShoot);
	
}

bool AFPSCharacter::ThrowGrenade_Validate()
{
	return true;
}
void AFPSCharacter::ThrowGrenade_Implementation()
{
	FString Array;
	if (Grenades.Num() > 0)
	{
		

		if (Grenades[0] != NULL)

		{
			if (CurrentGrenade)
			{
				ABaseGrenade* Grenade = GetWorld()->SpawnActor<ABaseGrenade>(CurrentGrenade, FPSCameraComponent->GetComponentLocation() + FPSCameraComponent->GetForwardVector() * 100.0, FRotator::ZeroRotator);
				MoveIgnoreActorAdd(Grenade);
				Grenade->Thrown(FPSCameraComponent->GetForwardVector() + FVector(0, 0, GrenadeThrowUpForce), GrenadeThrowStrength, this);
				Grenades.RemoveAt(0);
			}
			

		}
		else
		{
			UE_LOG(LogClass, Log, TEXT("Grenade does not exist"));
		}
	}
}

bool AFPSCharacter::ServerReload_Validate()
{
	return true;
}

void AFPSCharacter::ServerReload_Implementation()
{
	if (CurrentPrimary != NULL)
	{
		if (!CurrentPrimary->IsReloading && CurrentPrimary->AmmoLeftInMag != CurrentPrimary->MagazineSize)
		{
			ServerOnStopShoot();
			CurrentPrimary->StartReload();
		}
	}
}
void AFPSCharacter::MoveForward(float Value)
{

	// Find out which way is "forward" and record that the player wants to move that way.
	FVector Direction = FRotationMatrix(FRotator(0, Controller->GetControlRotation().Yaw, 0)).GetScaledAxis(EAxis::X);
	AddMovementInput(Direction, Value);
}

void AFPSCharacter::MoveRight(float Value)
{
	// Find out which way is "right" and record that the player wants to move that way.
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
	AddMovementInput(Direction, Value);
}

void AFPSCharacter::StartJump()
{
	bPressedJump = true;
}

void AFPSCharacter::StopJump()
{
	bPressedJump = false;
}

