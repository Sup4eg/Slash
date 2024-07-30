// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "SlashOverlay.h"

void USlashOverlay::SetHealthPercent(float Percent) {
    if (HealthProgressBar) {
        HealthProgressBar->SetPercent(Percent);
    }
}

void USlashOverlay::SetStaminaPercent(float Percent) {
    if (StaminaProgressBar) {
        StaminaProgressBar->SetPercent(Percent);
    }
}

void USlashOverlay::SetGold(int32 Amount) {
    if (GoldText) {
        GoldText->SetText(FText::AsNumber(Amount));
    }
}

void USlashOverlay::SetSouls(int32 Amount) {
    if (SoulsText) {
        SoulsText->SetText(FText::AsNumber(Amount));
    }
}
