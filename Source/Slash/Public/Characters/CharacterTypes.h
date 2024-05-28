#pragma once

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
    ECS_Unequipped UMETA(DisplayName = "Unequiped"),
    ECS_EquippedOneHandedWeapon UMETA(DisplayName = "Equiped One-Handed Weapon"),
    ECS_EquippedTwoHandedWeapon UMETA(DisplayName = "Equiped Two-Handed Weapon")
};

UENUM(BlueprintType)
enum class EActionState : uint8
{
    EAS_Unoccupied UMETA(DisplayName = "Unoccupied"),
    EAS_Attacking UMETA(DisplayName = "Attacking"),
    EAS_EquippingWeapon UMETA(DisplayName = "Equipping Weapon")
};