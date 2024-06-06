// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Slash/DebugMacros.h"
#include "Animation/AnimInstance.h"
#include "Engine/Engine.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AttributeComponent.h"
#include "HUD/HealthBarWidgetComponent.h"
#include "Animation/AnimMontage.h"
#include "Characters/CharacterTypes.h"

AEnemy::AEnemy()
{
    PrimaryActorTick.bCanEverTick = true;

    GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
    GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Block);
    GetMesh()->SetGenerateOverlapEvents(true);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

    AttributeComponent = CreateDefaultSubobject<UAttributeComponent>("AttributeComponent");
    HealthBarWidgetComponent = CreateDefaultSubobject<UHealthBarWidgetComponent>("HealthBarWidgetComponent");
    HealthBarWidgetComponent->SetupAttachment(GetRootComponent());
}

void AEnemy::BeginPlay()
{
    Super::BeginPlay();
    if (HealthBarWidgetComponent)
    {
        HealthBarWidgetComponent->SetVisibility(false);
    }
}

void AEnemy::Die()
{
    if (!GetMesh()->GetAnimInstance() || !DeathMontage || !HealthBarWidgetComponent) return;
    HealthBarWidgetComponent->SetVisibility(false);
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

    AnimInstance->Montage_Play(DeathMontage);
    int32 SectionNum = DeathMontage->CompositeSections.Num();
    int32 RandomSectionIndex = FMath::RandRange(0, SectionNum - 1);
    FName SectionName = DeathMontage->CompositeSections[RandomSectionIndex].SectionName;

    switch (RandomSectionIndex)
    {
        case 0: DeathPose = EDeathPose::EDP_Death1; break;
        case 1: DeathPose = EDeathPose::EDP_Death2; break;
        case 2: DeathPose = EDeathPose::EDP_Death3; break;
        case 3: DeathPose = EDeathPose::EDP_Death4; break;
        case 4: DeathPose = EDeathPose::EDP_Death5; break;
        default: break;
    }

    AnimInstance->Montage_JumpToSection(SectionName);
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    bDead = true;
    SetLifeSpan(5.f);
}

void AEnemy::PlayHitReactMontage(const FName& SectionName)
{
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (AnimInstance && HitReactMontage)
    {
        AnimInstance->Montage_Play(HitReactMontage);
        AnimInstance->Montage_JumpToSection(SectionName);
    }
}

void AEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (CombatTarget && HealthBarWidgetComponent)
    {
        const double DistanceToTarget = (CombatTarget->GetActorLocation() - GetActorLocation()).Size();
        if (DistanceToTarget > CombatRadius)
        {
            CombatTarget = nullptr;
            HealthBarWidgetComponent->SetVisibility(false);
        }
    }
}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint)
{
    if (!HitSound || !HitParticles || !AttributeComponent || !HealthBarWidgetComponent || bDead) return;
    HealthBarWidgetComponent->SetVisibility(true);
    if (AttributeComponent->IsAlive())
    {
        DirectionalHitReact(ImpactPoint);
    }
    else
    {
        Die();
    }

    UGameplayStatics::PlaySoundAtLocation(this, HitSound, ImpactPoint);
    UGameplayStatics::SpawnEmitterAtLocation(this, HitParticles, ImpactPoint);
}

float AEnemy::TakeDamage(float DamageAmount,  //
    struct FDamageEvent const& DamageEvent,   //
    class AController* EventInstigator,       //
    AActor* DamageCauser)
{
    if (!AttributeComponent || !HealthBarWidgetComponent) return 0.f;
    AttributeComponent->ReceiveDamage(DamageAmount);
    HealthBarWidgetComponent->SetHealthPercentage(AttributeComponent->GetHealthPercent());
    CombatTarget = EventInstigator->GetPawn();
    return DamageAmount;
}

void AEnemy::DirectionalHitReact(const FVector& ImpactPoint)
{
    const FVector Forward = GetActorForwardVector();
    const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
    const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal();

    // Forward * ToHIt = |Forward| * |ToHit| * cos(theta)
    const double Dot = FVector::DotProduct(Forward, ToHit);
    double Theta = FMath::Acos(Dot);
    // convert from radians to degrees
    Theta = FMath::RadiansToDegrees(Theta);

    // if CrossProduct points down, Theta should be negative
    const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);

    if (CrossProduct.Z < 0)
    {
        Theta *= -1.f;
    }

    FName Section("FromBack");
    if (Theta >= -45.f && Theta < 45.f)
    {
        Section = FName("FromFront");
    }
    else if (Theta >= -135.f && Theta < -45.f)
    {
        Section = FName("FromLeft");
    }
    else if (Theta >= 45.f && Theta < 135.f)
    {
        Section = FName("FromRight");
    }

    PlayHitReactMontage(Section);

    /*
    UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + CrossProduct * 100.f, 10.f, FColor::Blue, 5.f);

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Theta: %f"), Theta));
    }
    UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + Forward * 100.f, 10.f, FColor::Red, 5.f);
    UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + ToHit * 100.f, 10.f, FColor::Green, 5.f);

    */
}
