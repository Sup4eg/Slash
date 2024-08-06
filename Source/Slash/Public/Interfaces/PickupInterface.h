// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PickupInterface.generated.h"

class AItem;
class ASoul;
class ATreasure;
class AHealing;

UINTERFACE(MinimalAPI)
class UPickupInterface : public UInterface
{
    GENERATED_BODY()
};

class SLASH_API IPickupInterface
{
    GENERATED_BODY()

public:
    virtual void SetOverlappingItem(AItem* Item);
    virtual void AddSouls(ASoul* Soul);
    virtual void AddGold(ATreasure* Treasure);
    virtual bool AddHealth(AHealing* Healing);
};
