#include "Components/AttributeComponent.h"

UAttributeComponent::UAttributeComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UAttributeComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UAttributeComponent::ReceiveDamage(float DamageAmount)
{
    Health = FMath::Clamp(Health - DamageAmount, 0.f, MaxHealth);
}

float UAttributeComponent::GetHealthPercent() const
{
    return Health / MaxHealth;
}

bool UAttributeComponent::IsAlive()
{
    return Health > 0.f;
}
