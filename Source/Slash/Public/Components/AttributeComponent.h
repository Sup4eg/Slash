// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SLASH_API UAttributeComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAttributeComponent();

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(EditAnywhere, Category = "Actor Atributes", meta = (ClampMin = 1.0, ClampMax = 1000.0))
    float MaxHealth = 100.f;

    // Current Health of the actor
    UPROPERTY(EditAnywhere, Category = "Actor Atributes", meta = (ClampMin = 1.0, ClampMax = 1000.0))
    float Health = 100.f;

public:
    void ReceiveDamage(float DamageAmount);
    float GetHealthPercent() const;

    bool IsAlive();
};
