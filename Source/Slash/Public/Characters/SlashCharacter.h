// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterTypes.h"
#include "SlashCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class UCameraComponent;
class USpringArmComponent;
class UGroomComponent;
class AItem;
class UAnimMontage;
class AWeapon;

UCLASS()
class SLASH_API ASlashCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ASlashCharacter();

    virtual void Tick(float DeltaTime) override;

    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    UFUNCTION(BlueprintCallable)
    void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

protected:
    virtual void BeginPlay() override;

protected:
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

    void SetUpInputMappingContext();

    /**
     * Callbacs for input
     */

    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void Jump();
    void EKeyPressed();
    void Attack();

    /**
     * Play montage functions
     */

    void PlayAttackMontage();
    void PlayEquipMontage(const FName& SectionName);

    bool CanAttack() const;
    bool CanDisarm() const;
    bool CanArm() const;

    UFUNCTION(BlueprintCallable)
    void AttackEnd();

    UFUNCTION(BlueprintCallable)
    void Disarm();

    UFUNCTION(BlueprintCallable)
    void Arm();

    UFUNCTION(BlueprintCallable)
    void FinishEquipping();

private:
    ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

    UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
    EActionState ActionState = EActionState::EAS_Unoccupied;

    UPROPERTY(VisibleAnywhere)
    UCameraComponent* ViewCamera;

    UPROPERTY(VisibleAnywhere)
    USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere, Category = "Hair")
    UGroomComponent* Hair;

    UPROPERTY(VisibleAnywhere, Category = "Hair")
    UGroomComponent* Eyebrows;

    UPROPERTY(VisibleInstanceOnly)
    AItem* OverlappingItem;

    UPROPERTY(VisibleAnywhere, Category = "Weapon")
    AWeapon* EquippedWeapon;

    /*
        Animation montages
    */
    UPROPERTY(EditDefaultsOnly, Category = "Montages")
    UAnimMontage* AttackMontage;

    UPROPERTY(EditDefaultsOnly, Category = "Montages")
    UAnimMontage* EquipMontage;

public:
    FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }
    FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
};