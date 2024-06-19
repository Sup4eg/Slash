// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/EnemyAnimInstance.h"

void UEnemyAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();
    Enemy = Cast<AEnemy>(TryGetPawnOwner());
}

void UEnemyAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
    if (Enemy)
    {
        DeathPose = Enemy->GetDeathPose();
        GroundSpeed = Enemy->GetVelocity().Size();
        EnemyState = Enemy->GetEnemyState();
    }
}
