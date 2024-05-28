// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/SlashAnimInstacne.h"
#include "Characters/SlashCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void USlashAnimInstacne::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    SlashCharacter = Cast<ASlashCharacter>(TryGetPawnOwner());

    if (SlashCharacter)
    {
        SlashCharacterMovement = SlashCharacter->GetCharacterMovement();
    }
}

void USlashAnimInstacne::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    if (SlashCharacterMovement)
    {
        GroundSpeed = UKismetMathLibrary::VSizeXY(SlashCharacterMovement->Velocity);
        isFalling = SlashCharacterMovement->IsFalling();
        CharacterState = SlashCharacter->GetCharacterState();
    }
}
