// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Healing.generated.h"

UCLASS()
class SLASH_API AHealing : public AItem
{
    GENERATED_BODY()

protected:
    virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent,  //
        AActor* OtherActor,                                                      //
        UPrimitiveComponent* OtherComp,                                          //
        int32 OtherBodyIndex,                                                    //
        bool bFromSweep,                                                         //
        const FHitResult& SweepResult) override;

private:
    UPROPERTY(EditAnywhere, Category = "Heal Properties", meta = (ClampMin = 1))
    int32 Health = 25;

public:
    FORCEINLINE int32 GetHealthAmount() const { return Health; }
};
