#include "Components/AttributeComponent.h"
#include "AttributeComponent.h"

UAttributeComponent::UAttributeComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UAttributeComponent::RegenStamina(float DeltaTime)
{
    Stamina = FMath::Clamp(Stamina + StaminaRegenRate * DeltaTime, 0.f, MaxStamina);
}

void UAttributeComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UAttributeComponent::ReceiveDamage(float DamageAmount)
{
    Health = FMath::Clamp(Health - DamageAmount, 0.f, MaxHealth);
}

void UAttributeComponent::AddHealth(float HealthAmount)
{
    Health = FMath::Clamp(Health + HealthAmount, 0.f, MaxHealth);
}

void UAttributeComponent::UseStamina(float StaminaCost)
{
    Stamina = FMath::Clamp(Stamina - StaminaCost, 0.f, MaxStamina);
}

float UAttributeComponent::GetHealthPercent() const
{
    return Health / MaxHealth;
}

float UAttributeComponent::GetStaminaPercent() const
{
    return Stamina / MaxStamina;
}

bool UAttributeComponent::IsAlive()
{
    return Health > 0.f;
}

void UAttributeComponent::AddSouls(int32 NumberOfSouls)
{
    Souls += NumberOfSouls;
}

void UAttributeComponent::AddGold(int32 AmountOfGold)
{
    Gold += AmountOfGold;
}