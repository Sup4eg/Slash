// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/Weapons/Weapon.h"
#include "Characters/SlashCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interfaces/HitInterface.h"
#include "NiagaraComponent.h"

AWeapon::AWeapon()
{
    WeaponBox = CreateDefaultSubobject<UBoxComponent>("WeaponBox");
    WeaponBox->SetupAttachment(GetRootComponent());
    WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
    WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

    BoxTraceStart = CreateDefaultSubobject<USceneComponent>("BoxTraceStart");
    BoxTraceStart->SetupAttachment(GetRootComponent());

    BoxTraceEnd = CreateDefaultSubobject<USceneComponent>("BoxTraceEnd");
    BoxTraceEnd->SetupAttachment(GetRootComponent());
}

void AWeapon::BeginPlay()
{
    Super::BeginPlay();
    WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);
}

void AWeapon::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent,  //
    AActor* OtherActor,                                                       //
    UPrimitiveComponent* OtherComp,                                           //
    int32 OtherBodyIndex,                                                     //
    bool bFromSweep,                                                          //
    const FHitResult& SweepResult)
{
    Super::OnSphereBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent,  //
    AActor* OtherActor,                                                     //
    UPrimitiveComponent* OtherComp,                                         //
    int32 OtherBodyIndex)
{
    Super::OnSphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}

void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent,  //
    AActor* OtherActor,                                               //
    UPrimitiveComponent* OtherComp,                                   //
    int32 OtherBodyIndex,                                             //
    bool bFromSweep,                                                  //
    const FHitResult& SweepResult)
{
    const FVector Start = BoxTraceStart->GetComponentLocation();
    const FVector End = BoxTraceEnd->GetComponentLocation();

    TArray<AActor*> ActorsToIgnore;
    IgnoredActors.Add(this);

    for (AActor* Actor : IgnoredActors)
    {
        ActorsToIgnore.AddUnique(Actor);
    }

    FHitResult OutBoxHit;

    UKismetSystemLibrary::BoxTraceSingle(this,  //
        Start,                                  //
        End,                                    //
        FVector(5.f, 5.f, 5.f),                 //
        BoxTraceStart->GetComponentRotation(),  //
        ETraceTypeQuery::TraceTypeQuery1,       //
        false,                                  //
        ActorsToIgnore,                         //
        EDrawDebugTrace::None,                  //
        OutBoxHit,                              //
        true                                    //
    );
    if (OutBoxHit.GetActor())
    {
        IHitInterface* HitInterface = Cast<IHitInterface>(OutBoxHit.GetActor());
        if (HitInterface)
        {
            HitInterface->Execute_GetHit(OutBoxHit.GetActor(), OutBoxHit.ImpactPoint);
        }
        IgnoredActors.AddUnique(OutBoxHit.GetActor());
        CreateFields(OutBoxHit.ImpactPoint);
    }
}

void AWeapon::Equip(USceneComponent* InParent, FName InSocketName)
{
    AttachMeshToSocket(InParent, InSocketName);
    if (EquipSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, EquipSound, GetActorLocation());
    }
    if (Sphere)
    {
        Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
    if (EmbersEffect)
    {
        EmbersEffect->Deactivate();
    }
}

void AWeapon::AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName)
{
    Mesh->AttachToComponent(InParent, FAttachmentTransformRules::SnapToTargetIncludingScale, InSocketName);
    ItemState = EItemState::EIS_Equiped;
}
