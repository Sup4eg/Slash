// Fill out your copyright notice in the Description page of Project Settings.

#include "SlashCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GroomComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Items/Item.h"
#include "Items/Soul.h"
#include "Items/Treasure.h"
#include "Items/Healing.h"
#include "Items/Weapons/Weapon.h"
#include "Animation/AnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/AttributeComponent.h"
#include "MotionWarpingComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SlashHUD.h"
#include "SlashOverlay.h"
#include "Enemy.h"

ASlashCharacter::ASlashCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
    CameraBoom->SetupAttachment(GetRootComponent());
    CameraBoom->bUsePawnControlRotation = true;

    ViewCamera = CreateDefaultSubobject<UCameraComponent>("CameraView");
    ViewCamera->SetupAttachment(CameraBoom);
    CameraBoom->TargetArmLength = 300.f;

    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

    GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
    GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
    GetMesh()->SetGenerateOverlapEvents(true);

    VisibilitySphere = CreateDefaultSubobject<USphereComponent>("VisibilitySphere");
    VisibilitySphere->SetupAttachment(GetRootComponent());
    VisibilitySphere->SetSphereRadius(1000.f);
    VisibilitySphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    VisibilitySphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

    Hair = CreateDefaultSubobject<UGroomComponent>("Hair");
    Hair->SetupAttachment(GetMesh());
    Hair->AttachmentName = "head";

    Eyebrows = CreateDefaultSubobject<UGroomComponent>("Eyebrows");
    Eyebrows->SetupAttachment(GetMesh());
    Eyebrows->AttachmentName = "head";
}

void ASlashCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    UpdateMotionWarpingComponent();
    if (AttributeComponent && SlashOverlay)
    {
        AttributeComponent->RegenStamina(DeltaTime);
        SlashOverlay->SetStaminaPercent(AttributeComponent->GetStaminaPercent());
    }
}

float ASlashCharacter::TakeDamage(float DamageAmount,  //
    struct FDamageEvent const& DamageEvent,            //
    class AController* EventInstigator,                //
    AActor* DamageCauser)
{
    HandleDamage(DamageAmount);
    SetHUDHealth();
    return DamageAmount;
}

void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Move);
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Look);
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ASlashCharacter::Jump);
        EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Started, this, &ASlashCharacter::EKeyPressed);
        EnhancedInputComponent->BindAction(EquipOneHandedWeaponAction, ETriggerEvent::Started, this, &ASlashCharacter::Key1Pressed);
        EnhancedInputComponent->BindAction(EquipTwoHandedWeaponAction, ETriggerEvent::Started, this, &ASlashCharacter::Key2Pressed);
        EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ASlashCharacter::Attack);
        EnhancedInputComponent->BindAction(FocusAction, ETriggerEvent::Started, this, &ASlashCharacter::Focus);
        EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Started, this, &ASlashCharacter::Dodge);
    }
}

void ASlashCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
    Super::GetHit_Implementation(ImpactPoint, Hitter);
    SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
    if (AttributeComponent && AttributeComponent->IsAlive())
    {
        ActionState = EActionState::EAS_HitReaction;
    }
}

void ASlashCharacter::SetOverlappingItem(AItem* Item)
{
    OverlappingItem = Item;
}

void ASlashCharacter::AddSouls(ASoul* Souls)
{
    if (AttributeComponent && SlashOverlay)
    {
        AttributeComponent->AddSouls(Souls->GetSouls());
        SlashOverlay->SetSouls(AttributeComponent->GetSouls());
    }
}

void ASlashCharacter::AddGold(ATreasure* Treasure)
{
    if (AttributeComponent && SlashOverlay)
    {
        AttributeComponent->AddGold(Treasure->GetGold());
        SlashOverlay->SetGold(AttributeComponent->GetGold());
    }
}

bool ASlashCharacter::AddHealth(AHealing* Healing)
{
    if (AttributeComponent && SlashOverlay && IsNeedToHeal())
    {
        AttributeComponent->AddHealth(Healing->GetHealthAmount());
        SlashOverlay->SetHealthPercent(AttributeComponent->GetHealthPercent());
        return true;
    }
    return false;
}

void ASlashCharacter::BeginPlay()
{
    Super::BeginPlay();
    SetUpInputMappingContext();
    VisibilitySphere->OnComponentBeginOverlap.AddDynamic(this, &ASlashCharacter::EnemySeen);
    VisibilitySphere->OnComponentEndOverlap.AddDynamic(this, &ASlashCharacter::EnemyUnSeen);
    Tags.Add("EngageableTarget");
    InitializeSlashOverlay();
}

void ASlashCharacter::Attack()
{
    Super::Attack();
    if (CanAttack())
    {
        ActionState = EActionState::EAS_Attacking;
        PlayAttackMontage();
    }
}

bool ASlashCharacter::CanAttack() const
{
    return IsUnoccupied()  //
           && CharacterState != ECharacterState::ECS_Unequipped;
}

void ASlashCharacter::AttackEnd()
{
    Super::AttackEnd();
    ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::DodgeEnd()
{
    Super::DodgeEnd();
    ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::UpdateMotionWarpingComponent()
{
    Super::UpdateMotionWarpingComponent();
    const FMotionWarpingTarget* CombatWarpTarget = MotionWarpingComponent->FindWarpTarget(RotationTargetName);
    if (CombatWarpTarget)
    {
        MotionWarpingComponent->RemoveWarpTarget(RotationTargetName);
    }
    if (MotionWarpingComponent && CombatTarget.IsValid())
    {
        MotionWarpingComponent->AddOrUpdateWarpTargetFromLocation(RotationTargetName, GetRotationWarpTarget());
    }
}

void ASlashCharacter::PlayEquipMontage(const FName& SectionName)
{
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    UAnimMontage* EquipMontage = WeaponTypeToEquipMontages.Contains(LastEquippedWeapon->GetWeaponType())
                                     ? WeaponTypeToEquipMontages[LastEquippedWeapon->GetWeaponType()]
                                     : nullptr;
    if (AnimInstance && EquipMontage)
    {
        AnimInstance->Montage_Play(EquipMontage);
        AnimInstance->Montage_JumpToSection(SectionName);
    }
}

void ASlashCharacter::Die()
{
    Super::Die();
    ActionState = EActionState::EAS_Dead;
    DisableMeshCollision();
}

void ASlashCharacter::AttachWeaponToBack()
{
    if (LastEquippedWeapon)
    {
        LastEquippedWeapon->AttachMeshToSocket(GetMesh(), LastEquippedWeapon->SpineSocketName);
    }
}

void ASlashCharacter::AttachWeaponToHand()
{
    if (LastEquippedWeapon)
    {
        LastEquippedWeapon->AttachMeshToSocket(GetMesh(), LastEquippedWeapon->ArmSocketName);
    }
}

void ASlashCharacter::FinishEquipping()
{
    ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::HitReactEnd()
{
    ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::SetUpInputMappingContext()
{
    if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
                ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }
}

void ASlashCharacter::Move(const FInputActionValue& Value)
{
    if (ActionState != EActionState::EAS_Unoccupied || !Controller) return;
    const FVector2D MovementVector = Value.Get<FVector2D>();

    const FRotator ControlRotation = GetControlRotation();
    const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);
    const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);  // |v| = 1
    const FVector RightVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);       // |v| = 1

    AddMovementInput(ForwardDirection, MovementVector.X);
    AddMovementInput(RightVector, MovementVector.Y);
}

void ASlashCharacter::Look(const FInputActionValue& Value)
{
    if (!Controller) return;
    const FVector2D LookAxisVector = Value.Get<FVector2D>();
    AddControllerYawInput(LookAxisVector.X);
    AddControllerPitchInput(LookAxisVector.Y);
}

void ASlashCharacter::Jump()
{
    if (IsUnoccupied()) Super::Jump();
}

void ASlashCharacter::Focus()
{
    AEnemy* TargetEnemy = GetNearestVisibleEnemy();
    if (!TargetEnemy) return;

    if (CombatEnemy != TargetEnemy)
    {
        FocusOn(TargetEnemy);
    }
    else
    {
        FocusOff();
    }
}

void ASlashCharacter::Dodge()
{
    if (IsOccupied() || !HasEnoughStamina() || !SlashOverlay) return;
    Super::PlayDodgeMontage();
    ActionState = EActionState::EAS_Dodge;
    AttributeComponent->UseStamina(AttributeComponent->GetDodgeCost());
    SlashOverlay->SetStaminaPercent(AttributeComponent->GetStaminaPercent());
}

void ASlashCharacter::EKeyPressed()
{
    AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);

    if (CanEquip(OverlappingWeapon))
    {
        EquipWeapon(OverlappingWeapon);
    }
    else if (CanDisarm())
    {
        Disarm();
    }
    else if (CanArm())
    {
        Arm();
    }
}

void ASlashCharacter::Key1Pressed()
{
    if (CanDisarm())
    {
        Disarm();
    }
    else if (CanArm() && UnequippedWeapons.Contains(EWeaponType::EWT_OneHanded))
    {
        LastEquippedWeapon = UnequippedWeapons[EWeaponType::EWT_OneHanded];
        Arm();
    }
}

void ASlashCharacter::Key2Pressed()
{
    if (CanDisarm())
    {
        Disarm();
    }
    else if (CanArm() && UnequippedWeapons.Contains(EWeaponType::EWT_TwoHanded))
    {
        LastEquippedWeapon = UnequippedWeapons[EWeaponType::EWT_TwoHanded];
        Arm();
    }
}

void ASlashCharacter::InitializeSlashOverlay()
{
    APlayerController* PlayerController = Cast<APlayerController>(GetController());
    if (PlayerController)
    {
        if (ASlashHUD* SlashHUD = Cast<ASlashHUD>(PlayerController->GetHUD()))
        {
            SlashOverlay = SlashHUD->GetSlashOverlay();
            if (SlashOverlay && AttributeComponent)
            {
                SlashOverlay->SetHealthPercent(AttributeComponent->GetHealthPercent());
                SlashOverlay->SetStaminaPercent(1.f);
                SlashOverlay->SetGold(0);
                SlashOverlay->SetSouls(0);
            }
        }
    }
}

void ASlashCharacter::SetHUDHealth()
{
    if (SlashOverlay)
    {
        SlashOverlay->SetHealthPercent(AttributeComponent->GetHealthPercent());
    }
}

ECharacterState ASlashCharacter::GetCharacterStateByWeaponType(EWeaponType WeaponType) const
{
    switch (WeaponType)
    {
        case EWeaponType::EWT_OneHanded: return ECharacterState::ECS_EquippedOneHandedWeapon;
        case EWeaponType::EWT_TwoHanded: return ECharacterState::ECS_EquippedTwoHandedWeapon;
        default: return ECharacterState::ECS_EquippedOneHandedWeapon;
    }
}

bool ASlashCharacter::CanEquip(AWeapon* OverlappingWeapon) const
{
    return OverlappingWeapon && CharacterState == ECharacterState::ECS_Unequipped;
}

bool ASlashCharacter::CanDisarm() const
{
    return IsUnoccupied()  //
           && CharacterState != ECharacterState::ECS_Unequipped;
}

bool ASlashCharacter::CanArm() const
{
    return IsUnoccupied()                                        //
           && CharacterState == ECharacterState::ECS_Unequipped  //
           && LastEquippedWeapon;
}

void ASlashCharacter::Disarm()
{
    PlayEquipMontage("Unequip");
    DestroyUnequippedWeapon(LastEquippedWeapon->GetWeaponType());
    UnequippedWeapons.Add(LastEquippedWeapon->GetWeaponType(), LastEquippedWeapon);
    CharacterState = ECharacterState::ECS_Unequipped;
    ActionState = EActionState::EAS_EquippingWeapon;
}

void ASlashCharacter::Arm()
{
    PlayEquipMontage("Equip");
    RemoveFromUnequippedWeapons(LastEquippedWeapon->GetWeaponType());
    CharacterState = GetCharacterStateByWeaponType(LastEquippedWeapon->GetWeaponType());
    ActionState = EActionState::EAS_EquippingWeapon;
}

void ASlashCharacter::EquipWeapon(AWeapon* OverlappingWeapon)
{
    OverlappingWeapon->Equip(GetMesh(), OverlappingWeapon->ArmSocketName, this, this);
    CharacterState = GetCharacterStateByWeaponType(OverlappingWeapon->GetWeaponType());
    OverlappingItem = nullptr;
    LastEquippedWeapon = OverlappingWeapon;
}

void ASlashCharacter::DestroyUnequippedWeapon(EWeaponType WeaponType)
{
    if (UnequippedWeapons.Contains(WeaponType))
    {
        UnequippedWeapons[WeaponType]->Destroy();
    }
}

void ASlashCharacter::RemoveFromUnequippedWeapons(EWeaponType WeaponType)
{
    if (UnequippedWeapons.Contains(WeaponType))
    {
        UnequippedWeapons.Remove(WeaponType);
    }
}

void ASlashCharacter::FocusOn(AEnemy* TargetEnemy)
{
    // Deactivate previous combat enemy focus effect
    if (CombatEnemy.IsValid())
    {
        CombatEnemy->DeactivateFocusEffect();
    }
    // Set new combat enemy
    CombatTarget = TargetEnemy;
    CombatEnemy = TargetEnemy;
    if (CombatEnemy.IsValid())
    {
        TargetEnemy->ActivateFocusEffect();
    }
}

void ASlashCharacter::FocusOff()
{
    if (CombatEnemy.IsValid())
    {
        CombatEnemy->DeactivateFocusEffect();
    }
    CombatTarget = nullptr;
    CombatEnemy = nullptr;
}

AEnemy* ASlashCharacter::GetNearestVisibleEnemy()
{
    AEnemy* NearestEnemy = CombatEnemy.Get();
    float NearestPlayerToEnemyDistance = FLT_MAX;

    for (TWeakObjectPtr<AEnemy>& VisibleEnemy : VisibleEnemies)
    {
        if (VisibleEnemy.IsValid() && VisibleEnemy.Get() != CombatEnemy.Get() && IsEnemyVisible(VisibleEnemy.Get()))
        {
            float PlayerToEnemyDistance = FVector::DistSquared(VisibleEnemy->GetActorLocation(), GetActorLocation());
            if (PlayerToEnemyDistance < NearestPlayerToEnemyDistance)
            {
                NearestPlayerToEnemyDistance = PlayerToEnemyDistance;
                NearestEnemy = VisibleEnemy.Get();
            }
        }
    }
    return NearestEnemy;
}

bool ASlashCharacter::IsEnemyVisible(AEnemy* Enemy)
{
    FHitResult OutHitResult;
    UKismetSystemLibrary::LineTraceSingle(this, GetActorLocation(), Enemy->GetActorLocation(), ETraceTypeQuery::TraceTypeQuery1, false,
        TArray<AActor*>{this}, EDrawDebugTrace::None, OutHitResult, true);
    return OutHitResult.GetActor() == Enemy;
}

bool ASlashCharacter::IsUnoccupied() const
{
    return ActionState == EActionState::EAS_Unoccupied;
}

bool ASlashCharacter::IsOccupied() const
{
    return ActionState != EActionState::EAS_Unoccupied;
}

bool ASlashCharacter::HasEnoughStamina() const
{
    return AttributeComponent && AttributeComponent->GetStamina() > AttributeComponent->GetDodgeCost();
}

bool ASlashCharacter::IsNeedToHeal() const
{
    return AttributeComponent && AttributeComponent->GetHealth() < AttributeComponent->GetMaxHealth();
}

void ASlashCharacter::EnemySeen(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    AEnemy* Enemy = Cast<AEnemy>(OtherActor);
    if (Enemy)
    {
        VisibleEnemies.Add(Enemy);
    }
}

void ASlashCharacter::EnemyUnSeen(
    UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    AEnemy* Enemy = Cast<AEnemy>(OtherActor);
    if (Enemy && Enemy == CombatEnemy.Get())
    {
        FocusOff();
    }
    VisibleEnemies.Remove(Enemy);
}
