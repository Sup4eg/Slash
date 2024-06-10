// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Characters/CharacterTypes.h"
#include "EnemyTypes.h"
#include "Enemy.generated.h"

class UAnimMontage;
class USoundBase;
class UParticleSystem;
class UAttributeComponent;
class UHealthBarWidgetComponent;
class AAIController;
class UPawnSensingComponent;

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
    /*
        Components
    */

    UPROPERTY(VisibleAnywhere)
    UPawnSensingComponent* PawnSensingComponent;

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
    double CombatRadius = 1000.0;

    UPROPERTY(EditAnywhere)
    double AttackRadius = 150.f;

    UPROPERTY(EditAnywhere)
    double PatrolRadius = 200.f;

    /*Navigation*/

    UPROPERTY()
    AAIController* EnemyController;

    // Current patrol target
    UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
    AActor* PatrolTarget;

    UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
    TArray<AActor*> PatrolTargets;

    bool bDead = false;

    FTimerHandle PatrolTimerHandle;

    void PatrolTimerFinished();

    UPROPERTY(EditAnywhere, meta = (ClampMin = 1))
    float MaxPatrolTimerDelay = 10.f;

    UPROPERTY(EditAnywhere, meta = (ClampMin = 1))
    float MinPatrolTimerDelay = 5.f;

    UPROPERTY(EditAnywhere)
    float ChasingSpeed = 300.f;

    UPROPERTY(EditAnywhere)
    float PatrolSpeed = 125.f;

    EEnemyState EnemyState = EEnemyState::EES_Patrolling;

protected:
    virtual void BeginPlay() override;

    void Die();

    bool InTargetRange(AActor* TargetActor, double Range);

    void MoveToTarget(AActor*& TargetActor);

    AActor* ChoosePatrolTarget();

    void CheckCombatTarget();

    void CheckPatrolTarget();

    UFUNCTION()
    void PawnSeen(APawn* SeenPawn);

    /**
     * Play montage functions
     */

    void PlayHitReactMontage(const FName& SectionName);

    UPROPERTY(BluePrintReadOnly)
    EDeathPose DeathPose = EDeathPose::EDP_Alive;

public:
    FORCEINLINE EDeathPose GetDeathPose() const { return DeathPose; }
};
