// Fill out your copyright notice in the Description page of Project Settings.

#include "Breakable/BreakableActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Engine/World.h"
#include "Items/Treasure.h"
#include "Components/CapsuleComponent.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"

ABreakableActor::ABreakableActor()
{
    PrimaryActorTick.bCanEverTick = false;

    GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>("GeometryCollection");
    SetRootComponent(GeometryCollection);

    GeometryCollection->SetGenerateOverlapEvents(true);
    GeometryCollection->bUseSizeSpecificDamageThreshold = false;
    GeometryCollection->EnableClustering = true;
    GeometryCollection->SetShowBoneColors(false);
    GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
    GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
    GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);

    CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>("CapsuleComponent");
    CapsuleComponent->SetupAttachment(GetRootComponent());
    CapsuleComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    CapsuleComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
}

void ABreakableActor::BeginPlay()
{
    Super::BeginPlay();

    GeometryCollection->OnChaosBreakEvent.AddDynamic(this, &ABreakableActor::OnChaousBreak);
}

void ABreakableActor::OnChaousBreak(const FChaosBreakEvent& BreakEvent)
{
    OnBreak(BreakEvent.Location);
}

void ABreakableActor::OnBreak(const FVector& ImpactPoint)
{
    if (!GetWorld() || !BreakSound || bBroken) return;
    bBroken = true;
    SpawnTreasure();
    CapsuleComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
    UGameplayStatics::PlaySoundAtLocation(this, BreakSound, ImpactPoint);
    SetLifeSpan(LifeSpan);
}

void ABreakableActor::SpawnTreasure()
{
    if (TreasureClassess.IsEmpty()) return;
    FVector Location = GetActorLocation();
    Location.Z += TreasureSpawnHeight;
    int32 TreasureIndex = FMath::RandRange(0, TreasureClassess.Num() - 1);
    GetWorld()->SpawnActor<ATreasure>(TreasureClassess[TreasureIndex], Location, GetActorRotation());
}

void ABreakableActor::GetHit_Implementation(const FVector& ImpactPoint)
{
    OnBreak(ImpactPoint);
}
