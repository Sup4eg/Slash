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
#include "Items/Weapons/Weapon.h"
#include "Animation/AnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"

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
    }
}

void ASlashCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
    Super::GetHit_Implementation(ImpactPoint, Hitter);
    SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
    ActionState = EActionState::EAS_HitReaction;
}

void ASlashCharacter::BeginPlay()
{
    Super::BeginPlay();
    SetUpInputMappingContext();
    Tags.Add("EngageableTarget");
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
    return ActionState == EActionState::EAS_Unoccupied  //
           && CharacterState != ECharacterState::ECS_Unequipped;
}

void ASlashCharacter::AttackEnd()
{
    ActionState = EActionState::EAS_Unoccupied;
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
    if (ActionState != EActionState::EAS_Unoccupied) return;
    Super::Jump();
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
    return ActionState == EActionState::EAS_Unoccupied  //
           && CharacterState != ECharacterState::ECS_Unequipped;
}

bool ASlashCharacter::CanArm() const
{
    return ActionState == EActionState::EAS_Unoccupied           //
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