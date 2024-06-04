// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UNiagaraComponent;

enum class EItemState : uint8
{
    EIS_Hovering,
    EIS_Equiped
};

UCLASS()
class SLASH_API AItem : public AActor
{
    GENERATED_BODY()

public:
    AItem();

    virtual void Tick(float DeltaTime) override;

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent,  //
        AActor* OtherActor,                                                      //
        UPrimitiveComponent* OtherComp,                                          //
        int32 OtherBodyIndex,                                                    //
        bool bFromSweep,                                                         //
        const FHitResult& SweepResult);

    UFUNCTION()
    virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent,  //
        AActor* OtherActor,                                                    //
        UPrimitiveComponent* OtherComp,                                        //
        int32 OtherBodyIndex);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UStaticMeshComponent* Mesh;

    UPROPERTY(VisibleAnywhere)
    USphereComponent* Sphere;

    EItemState ItemState = EItemState::EIS_Hovering;

    UPROPERTY(EditAnywhere, Category = "VFX")
    UNiagaraComponent* EmbersEffect;

private:
    UPROPERTY(EditAnywhere, Category = "Sine Parameters")
    float Amplitude = 0.25f;

    UPROPERTY(EditAnywhere, Category = "Sine Parameters")
    float TimeConstant = 5.f;

    float RunningTime;

    float TransformedSin();
};