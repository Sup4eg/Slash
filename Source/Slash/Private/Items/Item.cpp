// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/Item.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"
#include "Characters/SlashCharacter.h"
#include "NiagaraComponent.h"

AItem::AItem()
{
    PrimaryActorTick.bCanEverTick = true;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
    Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SetRootComponent(Mesh);

    Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
    Sphere->SetupAttachment(Mesh);

    EmbersEffect = CreateDefaultSubobject<UNiagaraComponent>("Embers");
    EmbersEffect->SetupAttachment(GetRootComponent());
}

void AItem::BeginPlay()
{
    Super::BeginPlay();

    Sphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereBeginOverlap);
    Sphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);
}

void AItem::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent,  //
    AActor* OtherActor,                                                     //
    UPrimitiveComponent* OtherComp,                                         //
    int32 OtherBodyIndex,                                                   //
    bool bFromSweep,                                                        //
    const FHitResult& SweepResult)
{
    ASlashCharacter* SlashCharacter = Cast<ASlashCharacter>(OtherActor);
    if (SlashCharacter)
    {
        SlashCharacter->SetOverlappingItem(this);
    }
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent,  //
    AActor* OtherActor,                                                   //
    UPrimitiveComponent* OtherComp,                                       //
    int32 OtherBodyIndex)
{
    ASlashCharacter* SlashCharacter = Cast<ASlashCharacter>(OtherActor);
    if (SlashCharacter)
    {
        SlashCharacter->SetOverlappingItem(nullptr);
    }
}

float AItem::TransformedSin()
{
    return Amplitude * FMath::Sin(RunningTime * TimeConstant);
}

void AItem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    RunningTime += DeltaTime;
    if (ItemState == EItemState::EIS_Hovering)
    {
        AddActorWorldOffset(FVector(0.f, 0.f, TransformedSin()));
    }
}