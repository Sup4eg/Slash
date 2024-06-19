#pragma once

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
    EES_NoState UMETA(DisplayName = "NoState"),
    EES_Dead UMETA(DisplayName = "Dead"),
    EES_Patrolling UMETA(DisplayName = "Patrolling"),
    EES_Chasing UMETA(DisplayName = "Chasing"),
    EES_Attacking UMETA(DisplayName = "Attacking"),
    EES_Engaged UMETA(DisplayName = "Engaged")
};