// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/Treasure.h"
#include "Components/StaticMeshComponent.h"
#include "Interfaces/PickupInterface.h"
#include "Components/SphereComponent.h"

ATreasure::ATreasure()
{
    PrimaryActorTick.bCanEverTick = true;

    SceneComponent = CreateDefaultSubobject<USceneComponent>("Scene");
    SetRootComponent(SceneComponent);
    Mesh->SetupAttachment(SceneComponent);

    Sphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    Sphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

void ATreasure::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent,  //
    AActor* OtherActor,                                                         //
    UPrimitiveComponent* OtherComp,                                             //
    int32 OtherBodyIndex,                                                       //
    bool bFromSweep,                                                            //
    const FHitResult& SweepResult)
{
    IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
    if (PickupInterface)
    {
        PickupInterface->AddGold(this);
        SpawnPickupSound();
        Destroy();
    }
}

void ATreasure::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (ItemState == EItemState::EIS_Hovering)
    {
        AddActorLocalRotation(FRotator(0.0f, FMath::RandRange(1.0f, 1.5f), 0.0f));
    }
}
