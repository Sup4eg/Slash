// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/HitInterface.h"
#include "BreakableActor.generated.h"

class UGeometryCollectionComponent;
class ATreasure;
class UCapsuleComponent;
class USoundBase;

UCLASS()
class SLASH_API ABreakableActor : public AActor, public IHitInterface
{
    GENERATED_BODY()

public:
    ABreakableActor();

    virtual void GetHit_Implementation(const FVector& ImpactPoint) override;

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
    UGeometryCollectionComponent* GeometryCollection;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
    UCapsuleComponent* CapsuleComponent;

    UPROPERTY(EditAnywhere, Category = "Breakable Properties")
    TArray<TSubclassOf<ATreasure>> TreasureClassess;

    UPROPERTY(EditAnywhere, Category = "Breakable Properties")
    USoundBase* BreakSound;

    UPROPERTY(EditAnywhere, Category = "Breakable Properties")
    float TreasureSpawnHeight = 75.f;

    UPROPERTY(EditAnywhere, Category = "Breakable Properties")
    int32 LifeSpan = 3.f;

    bool bBroken = false;

    UFUNCTION()
    void OnChaousBreak(const FChaosBreakEvent& BreakEvent);

    void OnBreak(const FVector& ImpactPoint);

    void SpawnTreasure();
};
