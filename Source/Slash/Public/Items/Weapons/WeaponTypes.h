#pragma once

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
    EWT_OneHanded UMETA(DisplayName = "One Handed"),
    EWT_TwoHanded UMETA(DisplayName = "Two Handed")
};