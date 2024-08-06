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

    void RegenStamina(float DeltaTime);

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(EditAnywhere, Category = "Actor Atributes", meta = (ClampMin = 1.0, ClampMax = 1000.0))
    float MaxHealth = 100.f;

    // Current Health of the actor
    UPROPERTY(EditAnywhere, Category = "Actor Atributes", meta = (ClampMin = 1.0, ClampMax = 1000.0))
    float Health = 100.f;

    UPROPERTY(EditAnywhere, Category = "Actor Atributes", meta = (ClampMin = 1.0, ClampMax = 1000.0))
    float MaxStamina = 100.f;

    // Current Stamina of the actor
    UPROPERTY(EditAnywhere, Category = "Actor Atributes", meta = (ClampMin = 1.0, ClampMax = 1000.0))
    float Stamina = 100.f;

    UPROPERTY(EditAnywhere, Category = "Actor Attributes", meta = (ClampMin = 0))
    int32 Gold;

    UPROPERTY(EditAnywhere, Category = "Actor Attributes", meta = (ClampMin = 0))
    int32 Souls;

    UPROPERTY(EditAnywhere, Category = "Actor Attributes", meta = (ClampMin = 0))
    float DodgeCost = 14.f;

    UPROPERTY(EditAnywhere, Category = "Actor Attributes", meta = (ClampMin = 0))
    float StaminaRegenRate = 8.f;

public:
    void ReceiveDamage(float DamageAmount);
    void AddHealth(float HealthAmount);
    void UseStamina(float StaminaCost);
    float GetHealthPercent() const;
    float GetStaminaPercent() const;
    bool IsAlive();
    void AddSouls(int32 NumberOfSouls);
    void AddGold(int32 AmountOfGold);

    FORCEINLINE int32 GetGold() const { return Gold; }
    FORCEINLINE int32 GetSouls() const { return Souls; }
    FORCEINLINE float GetDodgeCost() const { return DodgeCost; }
    FORCEINLINE float GetStamina() const { return Stamina; }
    FORCEINLINE float GetHealth() const { return Health; }
    FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
};
