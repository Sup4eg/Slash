// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Weapon.generated.h"

class USceneComponent;
class USoundBase;
class UBoxComponent;

UCLASS()
class SLASH_API AWeapon : public AItem
{
    GENERATED_BODY()

public:
    AWeapon();

    void Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator);

    void AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName);

    TArray<AActor*> IgnoredActors;

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
    UPROPERTY(EditAnywhere, Category = "Weapon properties")
    USoundBase* EquipSound;

    UPROPERTY(VisibleAnywhere, Category = "Weapon properties")
    UBoxComponent* WeaponBox;

    UPROPERTY(VisibleAnywhere)
    USceneComponent* BoxTraceStart;

    UPROPERTY(VisibleAnywhere)
    USceneComponent* BoxTraceEnd;

    UPROPERTY(EditAnywhere, Category = "Weapon properties")
    float Damage = 20.f;

public:
    FORCEINLINE UBoxComponent* GetWeaponBox() const { return WeaponBox; }
};
