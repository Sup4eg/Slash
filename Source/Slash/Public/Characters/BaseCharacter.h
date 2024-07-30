// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Items/Weapons/WeaponTypes.h"
#include "Interfaces/HitInterface.h"
#include "Characters/CharacterTypes.h"
#include "BaseCharacter.generated.h"

class AWeapon;
class UAttributeComponent;
class USoundBase;
class UParticleSystem;
class UMotionWarpingComponent;
class UPawnSensingComponent;

UCLASS()
class SLASH_API ABaseCharacter : public ACharacter, public IHitInterface
{
    GENERATED_BODY()

public:
    ABaseCharacter();

    virtual void Tick(float DeltaTime) override;

protected:
    virtual void BeginPlay() override;

    virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;

    virtual void SendHit_Implementation(const FVector& ImpactPoint, AActor* DamagedActor, float BaseDamage, AActor* DamageCauser) override;

    // Combat
    virtual void Die();
    virtual void Attack();
    virtual bool CanAttack() const;
    void DirectionalHitReact(const FVector& ImpactPoint);
    virtual void HandleDamage(float DamageAmount);
    bool IsAlive() const;
    void DisableMeshCollision();
    FVector GetTranslationWarpTarget() const;
    FVector GetRotationWarpTarget() const;

    UFUNCTION(BlueprintCallable)
    virtual void AttackEnd();

    // Montages
    virtual int32 PlayAttackMontage();
    void PlayHitReactMontage(const FName& SectionName);
    void StopAttackMontage();
    // VFX
    void PlayHitSound(const FVector& ImpactPoint);
    void PlayDeathSound(const FVector& ActorLocation);

    void SpawnHitParticles(const FVector& ImpactPoint);

    void DisableCapsule();

    virtual void UpdateMotionWarpingComponent();

    UFUNCTION(BlueprintCallable)
    void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

    UPROPERTY(VisibleAnywhere, Category = "Weapon")
    AWeapon* LastEquippedWeapon;

    UPROPERTY(VisibleAnywhere)
    UAttributeComponent* AttributeComponent;

    UPROPERTY(VisibleAnywhere)
    UMotionWarpingComponent* MotionWarpingComponent;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat")
    TWeakObjectPtr<AActor> CombatTarget;

    UPROPERTY(EditAnywhere, Category = "Combat")
    FName TranslationTargetName = "TranslationTarget";

    UPROPERTY(EditAnywhere, Category = "Combat")
    FName RotationTargetName = "RotationTarget";

    UPROPERTY(BluePrintReadOnly)
    TEnumAsByte<EDeathPose> DeathPose;

private:
    UAnimMontage* GetAttackMontage(EWeaponType WeaponType) const;
    double GetDirectionalHitReactAngle(const FVector& ImpactPoint) const;
    FName GetDirectionalHitReactSection(double Theta) const;
    int32 PlayMontageSection(UAnimMontage* Montage);
    int32 PlayDeathMontage();
    int32 GetRandomMontageSection(UAnimMontage* Montage);
    bool IsMontageSectionsEmpty(UAnimMontage* Montage) const;

    void ExecuteGetEnemyHit(AActor* DamagedActor, const FVector& ImpactPoint);

    // VFX
    UPROPERTY(EditAnywhere, Category = "Sounds")
    USoundBase* HitSound;

    UPROPERTY(EditAnywhere, Category = "Sounds")
    USoundBase* DeathSound;

    UPROPERTY(EditAnywhere, Category = "VFX")
    UParticleSystem* HitParticles;

    // Montages
    UPROPERTY(EditDefaultsOnly, Category = "Montages")
    UAnimMontage* DeathMontage;

    UPROPERTY(EditDefaultsOnly, Category = "Montages")
    UAnimMontage* HitReactMontage;

    UPROPERTY(EditDefaultsOnly, Category = "Montages")
    TMap<EWeaponType, UAnimMontage*> WeaponTypeToAttackMontages;

    UPROPERTY(EditAnywhere, Category = "Combat")
    double WarpTargetDistance = 75.f;

public:
    FORCEINLINE TEnumAsByte<EDeathPose> GetDeathPose() const { return DeathPose; }
};
