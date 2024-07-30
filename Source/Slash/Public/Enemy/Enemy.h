// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Characters/BaseCharacter.h"
#include "Characters/CharacterTypes.h"
#include "Enemy.generated.h"

class UHealthBarWidgetComponent;
class AAIController;
class UNiagaraComponent;

UCLASS()
class SLASH_API AEnemy : public ABaseCharacter
{
    GENERATED_BODY()

public:
    AEnemy();

    /** <AActor> */
    virtual void Tick(float DeltaTime) override;

    virtual float TakeDamage(float DamageAmount,  //
        struct FDamageEvent const& DamageEvent,   //
        class AController* EventInstigator,       //
        AActor* DamageCauser) override;

    void DeactivateFocusEffect();

    void ActivateFocusEffect();

    virtual void Destroyed() override;
    /** </AActor> */

    /** <HitInterface> */
    virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
    /** </HitInterface> */

protected:
    /** <AActor> */
    virtual void BeginPlay() override;
    /** </AActor> */

    /** <ABaseCharacter> */
    virtual void Die() override;
    virtual void Attack() override;
    virtual bool CanAttack() const override;
    virtual void AttackEnd() override;
    virtual void HandleDamage(float DamageAmount) override;
    virtual void UpdateMotionWarpingComponent() override;
    /** </ABaseCharacter> */

    UPROPERTY(BlueprintReadOnly)
    EEnemyState EnemyState = EEnemyState::EES_Patrolling;

private:
    /** AI Behavior */
    void Initialize();
    void CheckCombatTarget();
    void CheckPatrolTarget();
    void PatrolTimerFinished();
    void HideHealthBar();
    void ShowHealthBar();
    void LoseInterest();
    void StartPatrolling();
    void ChaseTarget();
    bool IsOutsideCombatRadius() const;
    bool IsOutsideAttackRadius() const;
    bool IsInsideAttackRadius() const;
    bool IsChasing() const;
    bool IsAttacking() const;
    bool IsDead() const;
    void StartPatrolTimer();
    void ClearPatrolTimer();
    bool IsEngaged() const;
    void StartAttackTimer();
    void ClearAttackTimer();
    bool InTargetRange(TWeakObjectPtr<AActor> TargetActor, double Range) const;
    void MoveToTarget(TWeakObjectPtr<AActor>& TargetActor);
    AActor* ChoosePatrolTarget();
    void SpawnDefaultWeapon();

    UFUNCTION()
    virtual void PawnSeen(APawn* SeenPawn);  // Callback for OnPawnSeend in UPawnSensingComponent

    UPROPERTY(VisibleAnywhere)
    UHealthBarWidgetComponent* HealthBarWidgetComponent;

    UPROPERTY(EditAnywhere, Category = "VFX")
    UNiagaraComponent* FocusEffect;

    UPROPERTY(VisibleAnywhere)
    UPawnSensingComponent* PawnSensingComponent;

    UPROPERTY(EditAnywhere)
    TSubclassOf<AWeapon> WeaponClass;

    UPROPERTY(EditAnywhere)
    double CombatRadius = 1000.0;

    UPROPERTY(EditAnywhere)
    double AttackRadius = 120.f;

    UPROPERTY(EditAnywhere)
    double PatrolRadius = 200.f;

    UPROPERTY()
    AAIController* EnemyController;

    UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
    TWeakObjectPtr<AActor> PatrolTarget = nullptr;

    UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
    TArray<AActor*> PatrolTargets;

    FTimerHandle PatrolTimerHandle;

    FTimerHandle AttackTimerHandle;

    UPROPERTY(EditAnywhere, meta = (ClampMin = 1))
    float PatrolMax = 10.f;

    UPROPERTY(EditAnywhere, meta = (ClampMin = 1))
    float PatrolMin = 5.f;

    UPROPERTY(EditAnywhere, Category = Combat)
    float AttackMin = 0.5f;

    UPROPERTY(EditAnywhere, Category = Combat)
    float AttackMax = 1.f;

    UPROPERTY(EditAnywhere, Category = Combat)
    float ChasingSpeed = 300.f;

    UPROPERTY(EditAnywhere, Category = Combat)
    float PatrolSpeed = 125.f;

    UPROPERTY(EditAnywhere, Category = "Combat")
    float DeathLifeSpan = 8.f;

public:
    FORCEINLINE EDeathPose GetDeathPose() const { return DeathPose; }
    FORCEINLINE EEnemyState GetEnemyState() const { return EnemyState; }
};
