#pragma once

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
    EES_Patrolling UMETA(DisplayName = "Patrolling"),
    EES_Chasing UMETA(DisplayName = "Chasing"),
    EES_Attacking UMETA(DisplayName = "Attacking")
};