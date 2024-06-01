// Fill out your copyright notice in the Description page of Project Settings.

#include "Breakable/BreakableActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"

ABreakableActor::ABreakableActor()
{
    PrimaryActorTick.bCanEverTick = false;

    GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>("GeometryCollection");
    SetRootComponent(GeometryCollection);

    GeometryCollection->SetGenerateOverlapEvents(true);
    GeometryCollection->bUseSizeSpecificDamageThreshold = true;
    GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
    GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
}

void ABreakableActor::BeginPlay()
{
    Super::BeginPlay();
}

void ABreakableActor::GetHit_Implementation(const FVector& ImpactPoint) {}
