// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterTypes.h"
#include "SlashAnimInstacne.generated.h"

class ASlashCharacter;
class UCharacterMovementComponent;

UCLASS()
class SLASH_API USlashAnimInstacne : public UAnimInstance
{
    GENERATED_BODY()

public:
    virtual void NativeInitializeAnimation() override;

    virtual void NativeUpdateAnimation(float DeltaSeconds) override;

    UPROPERTY(BlueprintReadOnly)
    ASlashCharacter* SlashCharacter;

    UPROPERTY(BlueprintReadOnly)
    UCharacterMovementComponent* SlashCharacterMovement;

    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    float GroundSpeed = 0.f;

    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    bool isFalling = false;

    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    ECharacterState CharacterState;

    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    EActionState ActionState;

    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    TEnumAsByte<EDeathPose> DeathPose;
};
