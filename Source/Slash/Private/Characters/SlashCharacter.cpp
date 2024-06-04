// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/SlashCharacter.h"
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
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "Components/BoxComponent.h"

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

    Hair = CreateDefaultSubobject<UGroomComponent>("Hair");
    Hair->SetupAttachment(GetMesh());
    Hair->AttachmentName = "head";

    Eyebrows = CreateDefaultSubobject<UGroomComponent>("Eyebrows");
    Eyebrows->SetupAttachment(GetMesh());
    Eyebrows->AttachmentName = "head";
}

void ASlashCharacter::BeginPlay()
{
    Super::BeginPlay();
    SetUpInputMappingContext();
}

void ASlashCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ASlashCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
    if (EquippedWeapon && EquippedWeapon->GetWeaponBox())
    {
        EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
        EquippedWeapon->IgnoredActors.Empty();
    }
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
        EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ASlashCharacter::Attack);
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

void ASlashCharacter::EKeyPressed()
{
    AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
    if (OverlappingWeapon)
    {
        OverlappingWeapon->Equip(GetMesh(), "RightHandSocket");
        CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
        OverlappingItem = nullptr;
        EquippedWeapon = OverlappingWeapon;
    }
    else
    {
        if (CanDisarm())
        {
            PlayEquipMontage("Unequip");
            CharacterState = ECharacterState::ECS_Unequipped;
            ActionState = EActionState::EAS_EquippingWeapon;
        }
        else if (CanArm())
        {
            PlayEquipMontage("Equip");
            CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
            ActionState = EActionState::EAS_EquippingWeapon;
        }
    }
}

void ASlashCharacter::Attack()
{
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

bool ASlashCharacter::CanDisarm() const
{
    return ActionState == EActionState::EAS_Unoccupied  //
           && CharacterState != ECharacterState::ECS_Unequipped;
}

bool ASlashCharacter::CanArm() const
{
    return ActionState == EActionState::EAS_Unoccupied           //
           && CharacterState == ECharacterState::ECS_Unequipped  //
           && EquippedWeapon;
}

void ASlashCharacter::PlayAttackMontage()
{
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (AnimInstance && AttackMontage)
    {
        AnimInstance->Montage_Play(AttackMontage);
        const int32 Selection = FMath::RandRange(0, 2);
        FName SectionName = FName();
        switch (Selection)
        {
            case 0: SectionName = "Attack2"; break;
            case 1: SectionName = "Attack2"; break;
            case 2: SectionName = "Attack2"; break;
            default: break;
        }
        AnimInstance->Montage_JumpToSection(SectionName, AttackMontage);
    }
}

void ASlashCharacter::PlayEquipMontage(const FName& SectionName)
{
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (AnimInstance && EquipMontage)
    {
        AnimInstance->Montage_Play(EquipMontage);
        AnimInstance->Montage_JumpToSection(SectionName);
    }
}

void ASlashCharacter::AttackEnd()
{
    ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::Disarm()
{
    if (EquippedWeapon)
    {
        EquippedWeapon->AttachMeshToSocket(GetMesh(), "SpineSocket");
    }
}

void ASlashCharacter::Arm()
{
    if (EquippedWeapon)
    {
        EquippedWeapon->AttachMeshToSocket(GetMesh(), "RightHandSocket");
    }
}

void ASlashCharacter::FinishEquipping()
{
    ActionState = EActionState::EAS_Unoccupied;
}
