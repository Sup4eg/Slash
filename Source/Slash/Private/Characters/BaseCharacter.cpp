// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/BaseCharacter.h"
#include "Items/Weapons/Weapon.h"
#include "Components/BoxComponent.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Components/AttributeComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "Components/CapsuleComponent.h"
#include "MotionWarpingComponent.h"
#include "BaseCharacter.h"

ABaseCharacter::ABaseCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    AttributeComponent = CreateDefaultSubobject<UAttributeComponent>("AttributeComponent");
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

    MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>("MotionWarpingComponent");
}

void ABaseCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ABaseCharacter::BeginPlay()
{
    Super::BeginPlay();
}

void ABaseCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
    if (IsAlive() && Hitter)
    {
        DirectionalHitReact(Hitter->GetActorLocation());
    }
    else
    {
        Die();
    }
    PlayHitSound(ImpactPoint);
    SpawnHitParticles(ImpactPoint);
}

void ABaseCharacter::SendHit_Implementation(const FVector& ImpactPoint, AActor* DamagedActor, float BaseDamage, AActor* DamageCauser)
{
    UGameplayStatics::ApplyDamage(DamagedActor, BaseDamage, this->GetController(), DamageCauser, UDamageType::StaticClass());
    ExecuteGetEnemyHit(DamagedActor, ImpactPoint);
}

void ABaseCharacter::Die()
{
    Tags.Add("Dead");
    PlayDeathMontage();
}

void ABaseCharacter::Attack()
{
    if (CombatTarget.IsValid() && CombatTarget->ActorHasTag("Dead"))
    {
        CombatTarget = nullptr;
    }
}

bool ABaseCharacter::CanAttack() const
{
    return false;
}

void ABaseCharacter::DirectionalHitReact(const FVector& ImpactPoint)
{
    double Theta = GetDirectionalHitReactAngle(ImpactPoint);
    FName Section = GetDirectionalHitReactSection(Theta);
    PlayHitReactMontage(Section);
}

void ABaseCharacter::HandleDamage(float DamageAmount)
{
    if (!AttributeComponent) return;
    AttributeComponent->ReceiveDamage(DamageAmount);
}

bool ABaseCharacter::IsAlive() const
{
    return AttributeComponent && AttributeComponent->IsAlive();
}

void ABaseCharacter::DisableMeshCollision()
{
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

FVector ABaseCharacter::GetTranslationWarpTarget() const
{
    if (!CombatTarget.IsValid()) return FVector();
    const FVector CombatTargetLocation = CombatTarget->GetActorLocation();
    const FVector Location = GetActorLocation();
    FVector TargetToMe = (Location - CombatTargetLocation).GetSafeNormal();
    TargetToMe *= WarpTargetDistance;
    return CombatTargetLocation + TargetToMe;
}

FVector ABaseCharacter::GetRotationWarpTarget() const
{
    if (!CombatTarget.IsValid()) return FVector();
    const FVector TargetLocation = CombatTarget->GetActorLocation();
    return TargetLocation;
}

void ABaseCharacter::AttackEnd() {}

int32 ABaseCharacter::PlayAttackMontage()
{
    if (!LastEquippedWeapon) return -1;
    UAnimMontage* AttackMontage = GetAttackMontage(LastEquippedWeapon->GetWeaponType());
    return PlayMontageSection(AttackMontage);
}

void ABaseCharacter::PlayHitReactMontage(const FName& SectionName)
{
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (AnimInstance && HitReactMontage)
    {
        AnimInstance->Montage_Play(HitReactMontage);
        AnimInstance->Montage_JumpToSection(SectionName);
    }
}

void ABaseCharacter::StopAttackMontage()
{
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (AnimInstance && LastEquippedWeapon)
    {
        AnimInstance->Montage_Stop(0.25f, GetAttackMontage(LastEquippedWeapon->GetWeaponType()));
    }
}

void ABaseCharacter::PlayHitSound(const FVector& ImpactPoint)
{
    if (!HitSound) return;
    UGameplayStatics::PlaySoundAtLocation(this, HitSound, ImpactPoint);
}

void ABaseCharacter::PlayDeathSound(const FVector& ActorLocation)
{
    if (!DeathSound) return;
    UGameplayStatics::PlaySoundAtLocation(this, DeathSound, ActorLocation);
}

void ABaseCharacter::SpawnHitParticles(const FVector& ImpactPoint)
{
    if (!HitParticles) return;
    UGameplayStatics::SpawnEmitterAtLocation(this, HitParticles, ImpactPoint);
}

void ABaseCharacter::DisableCapsule()
{
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseCharacter::UpdateMotionWarpingComponent() {}

void ABaseCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
    if (LastEquippedWeapon && LastEquippedWeapon->GetWeaponBox())
    {
        LastEquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
        LastEquippedWeapon->IgnoredActors.Empty();
    }
}

UAnimMontage* ABaseCharacter::GetAttackMontage(EWeaponType WeaponType) const
{
    return WeaponTypeToAttackMontages.Contains(WeaponType) ? WeaponTypeToAttackMontages[WeaponType] : nullptr;
}

double ABaseCharacter::GetDirectionalHitReactAngle(const FVector& ImpactPoint) const
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
    return Theta;
}

FName ABaseCharacter::GetDirectionalHitReactSection(double Theta) const
{
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
    return Section;
}

int32 ABaseCharacter::PlayMontageSection(UAnimMontage* Montage)
{
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (AnimInstance && Montage && !IsMontageSectionsEmpty(Montage))
    {
        AnimInstance->Montage_Play(Montage);
        const int32 SectionNameIndex = GetRandomMontageSection(Montage);
        const FName SectionName = Montage->CompositeSections[SectionNameIndex].SectionName;
        AnimInstance->Montage_JumpToSection(SectionName);
        return SectionNameIndex;
    }
    return -1;
}

int32 ABaseCharacter::PlayDeathMontage()
{
    const int32 Selection = PlayMontageSection(DeathMontage);
    TEnumAsByte<EDeathPose> Pose(Selection);
    if (Pose >= EDeathPose::EDP_Death1 && Pose < EDeathPose::EDP_MAX)
    {
        DeathPose = Pose;
    }
    return Selection;
}

int32 ABaseCharacter::GetRandomMontageSection(UAnimMontage* Montage)
{
    const int32 SectionNum = Montage->CompositeSections.Num();
    const int32 RandomSectionIndex = FMath::RandRange(0, SectionNum - 1);
    return RandomSectionIndex;
}

bool ABaseCharacter::IsMontageSectionsEmpty(UAnimMontage* Montage) const
{
    return Montage->CompositeSections.IsEmpty();
}

void ABaseCharacter::ExecuteGetEnemyHit(AActor* DamagedActor, const FVector& ImpactPoint)
{
    IHitInterface* HitInterface = Cast<IHitInterface>(DamagedActor);
    if (HitInterface)
    {
        HitInterface->Execute_GetHit(DamagedActor, ImpactPoint, this);
    }
}