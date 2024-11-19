// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BaseCharacter.h"
#include "CharacterTypes.h"
#include "WeaponTypes.h"
#include "Interfaces/PickupInterface.h"
#include "SlashCharacter.generated.h"

struct FInputActionValue;
class UInputMappingContext;
class UInputAction;
class UCameraComponent;
class USpringArmComponent;
class UGroomComponent;
class AItem;
class ASoul;
class ATreasure;
class AHealing;
class USphereComponent;
class AEnemy;
class USlashOverlay;

UCLASS()
class SLASH_API ASlashCharacter : public ABaseCharacter, public IPickupInterface
{
    GENERATED_BODY()

public:
    ASlashCharacter();

    virtual void Tick(float DeltaTime) override;

    virtual float TakeDamage(float DamageAmount,  //
        struct FDamageEvent const& DamageEvent,   //
        class AController* EventInstigator,       //
        AActor* DamageCauser) override;

    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    /** <HitInterface> */
    virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
    /** </HitInterface> */

    /** <PickupInterface> */
    virtual void SetOverlappingItem(AItem* Item) override;
    virtual void AddSouls(ASoul* Souls) override;
    virtual void AddGold(ATreasure* Treasure) override;
    virtual bool AddHealth(AHealing* Healing) override;
    /** </PickupInterface> */

protected:
    /** <AActor> */
    virtual void BeginPlay() override;
    /** </AActor> */

    /** <ABaseCharacter> */
    virtual void Attack() override;
    virtual bool CanAttack() const override;
    virtual void AttackEnd() override;
    virtual void DodgeEnd() override;
    virtual void UpdateMotionWarpingComponent() override;
    /** </ABaseCharacter> */

    void PlayEquipMontage(const FName& SectionName);
    virtual void Die_Implementation() override;

    UFUNCTION(BlueprintCallable)
    void AttachWeaponToBack();

    UFUNCTION(BlueprintCallable)
    void AttachWeaponToHand();

    UFUNCTION(BlueprintCallable)
    void FinishEquipping();

    UFUNCTION(BlueprintCallable)
    void HitReactEnd();

    /** Input */
    void SetUpInputMappingContext();

    /** Callbacks for input */
    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void Jump();
    void Focus();
    void Dodge();
    void EKeyPressed();
    void Key1Pressed();
    void Key2Pressed();

    UPROPERTY(EditAnywhere, Category = "Input")
    UInputMappingContext* DefaultMappingContext;

    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* MoveAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* JumpAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* LookAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* EquipAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* AttackAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* EquipOneHandedWeaponAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* EquipTwoHandedWeaponAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* FocusAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* DodgeAction;

private:
    void InitializeSlashOverlay();
    void SetHUDHealth();
    ECharacterState GetCharacterStateByWeaponType(EWeaponType WeaponType) const;
    bool CanEquip(AWeapon* OverlappingWeapon) const;
    bool CanDisarm() const;
    bool CanArm() const;
    void Disarm();
    void Arm();
    void EquipWeapon(AWeapon* Weapon);
    void DestroyUnequippedWeapon(EWeaponType WeaponType);
    void RemoveFromUnequippedWeapons(EWeaponType WeaponType);
    void FocusOn(AEnemy* TargetEnemy);
    void FocusOff();
    AEnemy* GetNearestVisibleEnemy();
    bool IsEnemyVisible(AEnemy* Enemy);
    bool IsUnoccupied() const;
    bool IsOccupied() const;
    bool HasEnoughStamina() const;
    bool IsNeedToHeal() const;

    UFUNCTION()
    virtual void EnemySeen(UPrimitiveComponent* OverlappedComponent,  //
        AActor* OtherActor,                                           //
        UPrimitiveComponent* OtherComp,                               //
        int32 OtherBodyIndex,                                         //
        bool bFromSweep,                                              //
        const FHitResult& SweepResult);

    UFUNCTION()
    virtual void EnemyUnSeen(UPrimitiveComponent* OverlappedComponent,  //
        AActor* OtherActor,                                             //
        UPrimitiveComponent* OtherComp,                                 //
        int32 OtherBodyIndex);

    /** Character components */
    UPROPERTY(VisibleAnywhere)
    UCameraComponent* ViewCamera;

    UPROPERTY(VisibleAnywhere)
    USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere)
    USphereComponent* VisibilitySphere;

    UPROPERTY(VisibleAnywhere, Category = "Hair")
    UGroomComponent* Hair;

    UPROPERTY(VisibleAnywhere, Category = "Hair")
    UGroomComponent* Eyebrows;

    UPROPERTY(VisibleInstanceOnly)
    AItem* OverlappingItem;

    UPROPERTY(VisibleAnywhere, Category = "Weapon")
    TMap<EWeaponType, AWeapon*> UnequippedWeapons;

    // Montages

    UPROPERTY(EditDefaultsOnly, Category = "Montages")
    TMap<EWeaponType, UAnimMontage*> WeaponTypeToEquipMontages;

    ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

    UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
    EActionState ActionState = EActionState::EAS_Unoccupied;

    UPROPERTY(VisibleInstanceOnly)
    TSet<TWeakObjectPtr<AEnemy>> VisibleEnemies;

    UPROPERTY(VisibleInstanceOnly)
    TWeakObjectPtr<AEnemy> CombatEnemy;

    UPROPERTY()
    USlashOverlay* SlashOverlay;

public:
    FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
    FORCEINLINE EActionState GetActionState() const { return ActionState; }
};