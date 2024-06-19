// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Enemy/Enemy.h"
#include "Characters/CharacterTypes.h"
#include "EnemyAnimInstance.generated.h"

UCLASS()
class SLASH_API UEnemyAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    virtual void NativeInitializeAnimation() override;

    virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds);

    UPROPERTY(BlueprintReadWrite)
    AEnemy* Enemy;

    UPROPERTY(BlueprintReadOnly, Category = "Enemy state")
    TEnumAsByte<EDeathPose> DeathPose;

    UPROPERTY(BlueprintReadOnly, Category = "Enemy state")
    EEnemyState EnemyState = EEnemyState::EES_NoState;

    UPROPERTY(BlueprintReadOnly, Category = "Enemy movement")
    float GroundSpeed = 0.f;
};
