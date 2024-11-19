// Fill out your copyright notice in the Description page of Project Settings.

#include "Interfaces/PickupInterface.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/HitResult.h"
#include "Soul.h"

void ASoul::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    const double LocationZ = GetActorLocation().Z;
    if (LocationZ > DesiredZ)
    {
        const FVector DeltaLocation = FVector(0.f, 0.f, DriftRate * DeltaTime);
        AddActorWorldOffset(DeltaLocation);
    }
}

void ASoul::BeginPlay()
{
    Super::BeginPlay();

    const FVector Start = GetActorLocation();
    const FVector End = Start - FVector(0.f, 0.f, 2000.f);

    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(EObjectTypeQuery::ObjectTypeQuery1);
    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(GetOwner());
    FHitResult OutHitResult;
    UKismetSystemLibrary::LineTraceSingleForObjects(this,  //
        Start,                                             //
        End,                                               //
        ObjectTypes,                                       //
        false,                                             //
        ActorsToIgnore,                                    //
        EDrawDebugTrace::None,                             //
        OutHitResult,                                      //
        true);

    DesiredZ = OutHitResult.ImpactPoint.Z + 50.f;
}

void ASoul::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
    if (PickupInterface)
    {
        PickupInterface->AddSouls(this);
        SpawnPickupSystem();
        SpawnPickupSound();
        Destroy();
    }
}