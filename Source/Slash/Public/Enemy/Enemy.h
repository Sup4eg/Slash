// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Characters/CharacterTypes.h"
#include "Enemy.generated.h"

class UAnimMontage;
class USoundBase;
class UParticleSystem;
class UAttributeComponent;
class UHealthBarWidgetComponent;

UCLASS()
class SLASH_API AEnemy : public ACharacter, public IHitInterface
{
    GENERATED_BODY()

public:
    AEnemy();

    virtual void Tick(float DeltaTime) override;

    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    virtual void GetHit_Implementation(const FVector& ImpactPoint) override;

    virtual float TakeDamage(float DamageAmount,  //
        struct FDamageEvent const& DamageEvent,   //
        class AController* EventInstigator,       //
        AActor* DamageCauser) override;

private:
    UPROPERTY(VisibleAnywhere)
    UAttributeComponent* AttributeComponent;

    UPROPERTY(VisibleAnywhere)
    UHealthBarWidgetComponent* HealthBarWidgetComponent;

    /*
        Animation montages
    */
    UPROPERTY(EditDefaultsOnly, Category = "Montages")
    UAnimMontage* HitReactMontage;

    UPROPERTY(EditDefaultsOnly, Category = "Montages")
    UAnimMontage* DeathMontage;

    UPROPERTY(EditAnywhere, Category = "Sounds")
    USoundBase* HitSound;

    UPROPERTY(EditAnywhere, Category = "VFX")
    UParticleSystem* HitParticles;

    void DirectionalHitReact(const FVector& ImpactPoint);

    UPROPERTY(VisibleAnywhere)
    AActor* CombatTarget;

    UPROPERTY(EditAnywhere)
    double CombatRadius = 500.0;

    bool bDead = false;

protected:
    virtual void BeginPlay() override;

    void Die();

    /**
     * Play montage functions
     */

    void PlayHitReactMontage(const FName& SectionName);

    UPROPERTY(BluePrintReadOnly)
    EDeathPose DeathPose = EDeathPose::EDP_Alive;

public:
    FORCEINLINE EDeathPose GetDeathPose() const { return DeathPose; }
};
