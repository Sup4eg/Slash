// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Items/Weapons/WeaponTypes.h"
#include "Weapon.generated.h"

class UBoxComponent;
class USceneComponent;
class USoundBase;

UCLASS(ClassGroup = (Custom))
class SLASH_API AWeapon : public AItem
{
    GENERATED_BODY()

public:
    AWeapon();

    virtual void Tick(float DeltaTime) override;

    void Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator);

    void AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName);

    TArray<AActor*> IgnoredActors;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon properties")
    FName ArmSocketName = "OneHandedRightArmSocket";

    UPROPERTY(EditDefaultsOnly, Category = "Weapon properties")
    FName SpineSocketName = "OneHandedSpineSocket";

protected:
    virtual void BeginPlay() override;

    virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent,  //
        AActor* OtherActor,                                                      //
        UPrimitiveComponent* OtherComp,                                          //
        int32 OtherBodyIndex,                                                    //
        bool bFromSweep,                                                         //
        const FHitResult& SweepResult) override;

    virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent,  //
        AActor* OtherActor,                                                    //
        UPrimitiveComponent* OtherComp,                                        //
        int32 OtherBodyIndex) override;

    UFUNCTION()
    void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent,  //
        AActor* OtherActor,                                      //
        UPrimitiveComponent* OtherComp,                          //
        int32 OtherBodyIndex,                                    //
        bool bFromSweep,                                         //
        const FHitResult& SweepResult);

    UFUNCTION(BlueprintImplementableEvent)
    void CreateFields(const FVector& FieldLocation);

private:
    UPROPERTY(VisibleAnywhere, Category = "Weapon data")
    USoundBase* EquipSound;

    UPROPERTY(VisibleAnywhere, Category = "Weapon data")
    UBoxComponent* WeaponBox;

    UPROPERTY(EditAnywhere, Category = "Weapon data")
    float Damage = 20.f;

    UPROPERTY(EditAnywhere, Category = "Weapon data")
    USceneComponent* BoxTraceStart;

    UPROPERTY(EditAnywhere, Category = "Weapon data")
    USceneComponent* BoxTraceEnd;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon properties")
    EWeaponType WeaponType = EWeaponType::EWT_OneHanded;

public:
    FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
    FORCEINLINE UBoxComponent* GetWeaponBox() const { return WeaponBox; }
};
