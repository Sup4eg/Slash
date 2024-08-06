// Fill out your copyright notice in the Description page of Project Settings.

#include "PickupInterface.h"

void IPickupInterface::SetOverlappingItem(AItem* Item) {}

void IPickupInterface::AddSouls(ASoul* Soul) {}

void IPickupInterface::AddGold(ATreasure* Treasure) {}

bool IPickupInterface::AddHealth(AHealing* Healing)
{
    return false;
}
