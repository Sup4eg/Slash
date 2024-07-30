// Fill out your copyright notice in the Description page of Project Settings.

#include "SlashOverlay.h"
#include "SlashHUD.h"

void ASlashHUD::BeginPlay()
{
    Super::BeginPlay();
    check(GetWorld());
    if (SlashOverlayClass)
    {
        SlashOverlay = CreateWidget<USlashOverlay>(GetWorld()->GetFirstPlayerController(), SlashOverlayClass);
        SlashOverlay->AddToViewport();
    }
}