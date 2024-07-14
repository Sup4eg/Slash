// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Engine/Engine.h"
#include "HUD/HealthBarWidgetComponent.h"
#include "Characters/CharacterTypes.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "TimerManager.h"
#include "Perception/PawnSensingComponent.h"
#include "Components/AttributeComponent.h"
#include "Items/Weapons/Weapon.h"
#include "SlashCharacter.h"
#include "MotionWarpingComponent.h"
#include "Enemy.h"
#include "NiagaraComponent.h"

AEnemy::AEnemy()
{
    PrimaryActorTick.bCanEverTick = true;

    GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
    GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Block);
    GetMesh()->SetGenerateOverlapEvents(true);

    HealthBarWidgetComponent = CreateDefaultSubobject<UHealthBarWidgetComponent>("HealthBarWidgetComponent");
    HealthBarWidgetComponent->SetupAttachment(GetRootComponent());

    // Set sensing component
    PawnSensingComponent->SetPeripheralVisionAngle(45.f);
    PawnSensingComponent->SightRadius = 4000.f;
    PawnSensingComponent->bOnlySensePlayers = true;

    GetCharacterMovement()->bOrientRotationToMovement = true;
    bUseControllerRotationYaw = false;
    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;

    FocusEffect = CreateDefaultSubobject<UNiagaraComponent>("FocusEffect");
    FocusEffect->SetupAttachment(GetRootComponent());
}

void AEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (IsDead()) return;
    if (EnemyState > EEnemyState::EES_Patrolling)
    {
        CheckCombatTarget();
    }
    else
    {
        CheckPatrolTarget();
    }
    UpdateMotionWarpingComponent();
}

float AEnemy::TakeDamage(float DamageAmount,  //
    struct FDamageEvent const& DamageEvent,   //
    class AController* EventInstigator,       //
    AActor* DamageCauser)
{
    if (IsDead()) return 0.f;
    HandleDamage(DamageAmount);
    CombatTarget = EventInstigator->GetPawn();
    if (IsInsideAttackRadius())
    {
        EnemyState = EEnemyState::EES_Attacking;
    }
    else if (IsOutsideAttackRadius())
    {
        ChaseTarget();
    }
    return DamageAmount;
}

void AEnemy::DeactivateFocusEffect()
{
    if (FocusEffect)
    {
        FocusEffect->Deactivate();
    }
}

void AEnemy::ActivateFocusEffect()
{
    if (FocusEffect)
    {
        FocusEffect->Activate();
    }
}

void AEnemy::Destroyed()
{
    if (LastEquippedWeapon)
    {
        LastEquippedWeapon->Destroy();
    }
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
    if (IsDead()) return;
    ShowHealthBar();
    Super::GetHit_Implementation(ImpactPoint, Hitter);
    ClearPatrolTimer();
    ClearAttackTimer();
    SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
    StopAttackMontage();
}

void AEnemy::BeginPlay()
{
    Super::BeginPlay();
    DeactivateFocusEffect();
    check(GetWorld());
    Initialize();
    Tags.Add("Enemy");

    if (PawnSensingComponent)
    {
        PawnSensingComponent->OnSeePawn.AddDynamic(this, &ABaseCharacter::PawnSeen);
    }
}

void AEnemy::Die()
{
    HideHealthBar();
    PlayDeathMontage();
    ClearAttackTimer();
    DisableCapsule();
    EnemyState = EEnemyState::EES_Dead;
    SetLifeSpan(DeathLifeSpan);
    PlayDeathSound(GetActorLocation());
    SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::Attack()
{
    Super::Attack();
    EnemyState = EEnemyState::EES_Engaged;
    PlayAttackMontage();
}

bool AEnemy::CanAttack() const
{
    return IsInsideAttackRadius() && !IsAttacking() && !IsEngaged() && !IsDead();
}

void AEnemy::AttackEnd()
{
    Super::AttackEnd();
    EnemyState = EEnemyState::EES_NoState;
    CheckCombatTarget();
}

void AEnemy::HandleDamage(float DamageAmount)
{
    Super::HandleDamage(DamageAmount);
    if (AttributeComponent && HealthBarWidgetComponent)
    {
        HealthBarWidgetComponent->SetHealthPercentage(AttributeComponent->GetHealthPercent());
    }
}

int32 AEnemy::PlayDeathMontage()
{
    const int32 Selection = Super::PlayDeathMontage();
    TEnumAsByte<EDeathPose> Pose(Selection);
    if (Pose >= EDeathPose::EDP_Death1 && Pose < EDeathPose::EDP_MAX)
    {
        DeathPose = Pose;
    }
    return Selection;
}

void AEnemy::UpdateMotionWarpingComponent()
{
    Super::UpdateMotionWarpingComponent();
    if (MotionWarpingComponent && CombatTarget.IsValid())
    {
        MotionWarpingComponent->AddOrUpdateWarpTargetFromLocation(TranslationTargetName, GetTranslationWarpTarget());
        MotionWarpingComponent->AddOrUpdateWarpTargetFromLocation(RotationTargetName, GetRotationWarpTarget());
    }
}

void AEnemy::CheckCombatTarget()
{
    if (IsOutsideCombatRadius())
    {
        ClearAttackTimer();
        LoseInterest();
        if (!IsEngaged()) StartPatrolling();
    }
    else if (IsOutsideAttackRadius() && !IsChasing())
    {
        ClearAttackTimer();
        if (!IsEngaged()) ChaseTarget();
    }
    else if (CanAttack())
    {
        StartAttackTimer();
    }
}

void AEnemy::CheckPatrolTarget()
{
    if (InTargetRange(PatrolTarget, PatrolRadius))
    {
        StartPatrolTimer();
    }
}

void AEnemy::PatrolTimerFinished()
{
    MoveToTarget(PatrolTarget);
}

void AEnemy::HideHealthBar()
{
    if (HealthBarWidgetComponent)
    {
        HealthBarWidgetComponent->SetVisibility(false);
    }
}

void AEnemy::ShowHealthBar()
{
    if (HealthBarWidgetComponent)
    {
        HealthBarWidgetComponent->SetVisibility(true);
    }
}

void AEnemy::LoseInterest()
{
    CombatTarget = nullptr;
    HideHealthBar();
}

void AEnemy::StartPatrolling()
{
    EnemyState = EEnemyState::EES_Patrolling;
    GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;
    MoveToTarget(PatrolTarget);
}

void AEnemy::ChaseTarget()
{
    EnemyState = EEnemyState::EES_Chasing;
    GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
    MoveToTarget(CombatTarget);
}

bool AEnemy::IsOutsideCombatRadius() const
{
    return !InTargetRange(CombatTarget, CombatRadius);
}

bool AEnemy::IsOutsideAttackRadius() const
{
    return !InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemy::IsInsideAttackRadius() const
{
    return InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemy::IsChasing() const
{
    return EnemyState == EEnemyState::EES_Chasing;
}

bool AEnemy::IsAttacking() const
{
    return EnemyState == EEnemyState::EES_Attacking;
}

bool AEnemy::IsDead() const
{
    return EnemyState == EEnemyState::EES_Dead;
}

void AEnemy::StartPatrolTimer()
{
    PatrolTarget = ChoosePatrolTarget();
    const float PatrolTime = FMath::RandRange(PatrolMin, PatrolMax);
    GetWorldTimerManager().SetTimer(   //
        PatrolTimerHandle,             //
        this,                          //
        &AEnemy::PatrolTimerFinished,  //
        PatrolTime                     //
    );
}

void AEnemy::ClearPatrolTimer()
{
    GetWorldTimerManager().ClearTimer(PatrolTimerHandle);
}

bool AEnemy::IsEngaged() const
{
    return EnemyState == EEnemyState::EES_Engaged;
}

void AEnemy::StartAttackTimer()
{
    EnemyState = EEnemyState::EES_Attacking;
    const float AttackTime = FMath::RandRange(AttackMin, AttackMax);
    GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &AEnemy::Attack, AttackTime);
}

void AEnemy::ClearAttackTimer()
{
    GetWorldTimerManager().ClearTimer(AttackTimerHandle);
}

bool AEnemy::InTargetRange(TWeakObjectPtr<AActor> TargetActor, double Range) const
{
    if (!TargetActor.IsValid()) return false;
    const double DistanceToTarget = (TargetActor->GetActorLocation() - GetActorLocation()).Size();
    return DistanceToTarget <= Range;
}

void AEnemy::MoveToTarget(TWeakObjectPtr<AActor>& TargetActor)
{
    if (EnemyController && TargetActor.IsValid())
    {
        FAIMoveRequest MoveRequest;
        MoveRequest.SetGoalActor(TargetActor.Get());
        MoveRequest.SetAcceptanceRadius(20.f);
        EnemyController->MoveTo(MoveRequest);
    }
}

AActor* AEnemy::ChoosePatrolTarget()
{
    TArray<AActor*> ValidTargets;
    for (AActor* Target : PatrolTargets)
    {
        if (Target != PatrolTarget)
        {
            ValidTargets.AddUnique(Target);
        }
    }
    if (ValidTargets.IsEmpty()) return nullptr;
    const int32 PatrolTargetIndex = FMath::RandRange(0, ValidTargets.Num() - 1);
    return ValidTargets[PatrolTargetIndex];
}

void AEnemy::PawnSeen(APawn* SeenPawn)
{
    Super::PawnSeen(SeenPawn);
    const bool bShouldChaseTarget =                 //
        !IsDead() &&                                //
        EnemyState != EEnemyState::EES_Chasing &&   //
        EnemyState < EEnemyState::EES_Attacking &&  //
        SeenPawn->ActorHasTag("EngageableTarget");  //

    if (bShouldChaseTarget)
    {
        CombatTarget = SeenPawn;
        ClearPatrolTimer();
        ChaseTarget();
    }
}

void AEnemy::SpawnDefaultWeapon()
{
    if (WeaponClass)
    {
        LastEquippedWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass);
        LastEquippedWeapon->Equip(GetMesh(), LastEquippedWeapon->ArmSocketName, this, this);
    }
}

void AEnemy::Initialize()
{
    EnemyController = Cast<AAIController>(GetController());
    MoveToTarget(PatrolTarget);
    HideHealthBar();
    SpawnDefaultWeapon();
}