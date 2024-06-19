// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BaseCharacter.h"
#include "CharacterTypes.h"
#include "WeaponTypes.h"
#include "SlashCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class UCameraComponent;
class USpringArmComponent;
class UGroomComponent;
class AItem;

UCLASS()
class SLASH_API ASlashCharacter : public ABaseCharacter
{
    GENERATED_BODY()

public:
    ASlashCharacter();

    virtual void Tick(float DeltaTime) override;

    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    /** <HitInterface> */
    virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
    /** </HitInterface> */

protected:
    virtual void BeginPlay() override;

    /** Combat */
    virtual void Attack() override;
    virtual bool CanAttack() const override;
    virtual void AttackEnd() override;
    void PlayEquipMontage(const FName& SectionName);

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

private:
    ECharacterState GetCharacterStateByWeaponType(EWeaponType WeaponType) const;
    bool CanEquip(AWeapon* OverlappingWeapon) const;
    bool CanDisarm() const;
    bool CanArm() const;
    void Disarm();
    void Arm();
    void EquipWeapon(AWeapon* Weapon);
    void DestroyUnequippedWeapon(EWeaponType WeaponType);
    void RemoveFromUnequippedWeapons(EWeaponType WeaponType);

    /** Character components */
    UPROPERTY(VisibleAnywhere) UCameraComponent* ViewCamera;

    UPROPERTY(VisibleAnywhere)
    USpringArmComponent* CameraBoom;

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

public:
    FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }
    FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
};