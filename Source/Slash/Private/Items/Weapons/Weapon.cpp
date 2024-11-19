// Fill out your copyright notice in the Description page of Project Settings.

#include "DrawDebugHelpers.h"
#include "Items/Weapons/Weapon.h"
#include "Characters/SlashCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Sound/SoundBase.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interfaces/HitInterface.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Weapon.h"

AWeapon::AWeapon()
{
    PrimaryActorTick.bCanEverTick = true;

    WeaponBox = CreateDefaultSubobject<UBoxComponent>("WeaponBox");
    WeaponBox->SetupAttachment(GetRootComponent());
    WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
    WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

    BoxTraceStart = CreateDefaultSubobject<USceneComponent>("BoxTraceStart");
    BoxTraceEnd = CreateDefaultSubobject<USceneComponent>("BoxTraceEnd");
    BoxTraceStart->SetupAttachment(GetRootComponent());
    BoxTraceEnd->SetupAttachment(GetRootComponent());
}

void AWeapon::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AWeapon::Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator)
{
    ItemState = EItemState::EIS_Equiped;
    SetOwner(NewOwner);
    SetInstigator(NewInstigator);
    AttachMeshToSocket(InParent, InSocketName);
    DisableSphereCollision();

    PlayEquipSound();
    DeactivateEmbers();
}

void AWeapon::AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName)
{
    Mesh->AttachToComponent(InParent, FAttachmentTransformRules::SnapToTargetIncludingScale, InSocketName);
}

void AWeapon::BeginPlay()
{
    Super::BeginPlay();
    WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);
}

void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent,  //
    AActor* OtherActor,                                               //
    UPrimitiveComponent* OtherComp,                                   //
    int32 OtherBodyIndex,                                             //
    bool bFromSweep,                                                  //
    const FHitResult& SweepResult)                                    //
{

    if (IsActorSameType(OtherActor)) return;
    FHitResult OutBoxHit;
    BoxTrace(OutBoxHit);

    if (ShouldProcessHit(OutBoxHit))
    {
        ExecuteSendHit(OutBoxHit, GetInstigator());
        CreateFields(OutBoxHit.ImpactPoint);
    }
}

void AWeapon::PlayEquipSound()
{
    if (EquipSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, EquipSound, GetActorLocation());
    }
}

void AWeapon::DisableSphereCollision()
{
    if (Sphere)
    {
        Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
}

void AWeapon::DeactivateEmbers()
{
    if (ItemEffect)
    {
        ItemEffect->Deactivate();
    }
}

void AWeapon::BoxTrace(FHitResult& OutBoxHit)
{
    const FVector Start = BoxTraceStart->GetComponentLocation();
    const FVector End = BoxTraceEnd->GetComponentLocation();

    TArray<AActor*> ActorsToIgnore;
    IgnoredActors.AddUnique(this);
    IgnoredActors.AddUnique(GetOwner());

    for (AActor* Actor : IgnoredActors)
    {
        ActorsToIgnore.AddUnique(Actor);
    }

    UKismetSystemLibrary::BoxTraceSingle(this,                                 //
        Start,                                                                 //
        End,                                                                   //
        FVector(BoxTraceCoord),                                                //
        BoxTraceStart->GetComponentRotation(),                                 //
        ETraceTypeQuery::TraceTypeQuery1,                                      //
        false,                                                                 //
        ActorsToIgnore,                                                        //
        bShowBoxDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,  //
        OutBoxHit,                                                             //
        true                                                                   //
    );

    IgnoredActors.AddUnique(OutBoxHit.GetActor());
}

void AWeapon::ExecuteSendHit(FHitResult& OutBoxHit, AActor* InstigatorActor)
{
    IHitInterface* HitInterface = Cast<IHitInterface>(InstigatorActor);
    if (HitInterface)
    {
        HitInterface->Execute_SendHit(InstigatorActor, OutBoxHit.ImpactPoint, OutBoxHit.GetActor(), Damage, this);
    }
}

bool AWeapon::IsActorSameType(AActor* OtherActor)
{
    return GetOwner()->ActorHasTag("Enemy") && OtherActor->ActorHasTag("Enemy");
}

bool AWeapon::ShouldProcessHit(FHitResult& OutBoxHit)
{
    return OutBoxHit.GetActor() && !IsActorSameType(OutBoxHit.GetActor()) && GetInstigator();
}
