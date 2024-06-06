// Fill out your copyright notice in the Description page of Project Settings.

#include "HUD/HealthBarWidgetComponent.h"
#include "HUD/HealthBar.h"
#include "Components/ProgressBar.h"

void UHealthBarWidgetComponent::SetHealthPercentage(float HealthPercentage)
{
    if (!HealthBarWidget)
    {
        HealthBarWidget = Cast<UHealthBar>(GetUserWidgetObject());
    }
    if (HealthBarWidget && HealthBarWidget->HealthBar)
    {
        HealthBarWidget->HealthBar->SetPercent(HealthPercentage);
    }
}
