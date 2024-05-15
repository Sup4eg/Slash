// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bird.generated.h"
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

UCLASS()
class SLASH_API ABird : public APawn
{
    GENERATED_BODY()

public:
    ABird();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
