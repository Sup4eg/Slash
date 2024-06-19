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
    EAS_HitReaction UMETA(DisplayName = "Unocuupied"),
    EAS_Attacking UMETA(DisplayName = "Attacking"),
    EAS_EquippingWeapon UMETA(DisplayName = "Equipping Weapon")
};

UENUM(BlueprintType)
enum EDeathPose
{
    EDP_Death1 UMETA(DisplayName = "Death1"),
    EDP_Death2 UMETA(DisplayName = "Death2"),
    EDP_Death3 UMETA(DisplayName = "Death3"),
    EDP_Death4 UMETA(DisplayName = "Death4"),
    EDP_Death5 UMETA(DisplayName = "Death5"),

    EDP_MAX UMETA(DisplayName = "DefaultMAX")
};