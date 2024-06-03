#include "AllyActor.h"
#include "EnemyActor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Game/GameProjectile.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AAllyActor::AAllyActor()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    // Create the mesh component and attach it to the root component
    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    MeshComp->SetupAttachment(GetRootComponent());

    // Initialize default values
    Direction = FVector::BackwardVector;
}

// Called when the game starts or when spawned
void AAllyActor::BeginPlay()
{
    Super::BeginPlay();
    Speed = 300.0f;
    PlayerCharacter = Cast<AGameCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    MeshComp->OnComponentHit.AddDynamic(this, &AAllyActor::OnHit);  // set up a notification for when this component hits something blocking
}

// Called every frame
void AAllyActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    UpdateLocation(DeltaTime);
    // Check the actor's location
    FVector CurrentLocation = GetActorLocation();
    if (CurrentLocation.X < 7000.0f)
    {
        Destroy();
    }
}

void AAllyActor::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor != nullptr)
    {
        if (AGameProjectile* actor = Cast<AGameProjectile>(OtherActor))
        {
            if (actor != nullptr)
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Ally Hit")));
                PlayerCharacter->Score -= 5;
                this->Destroy();
            }
        }
    }
}

void AAllyActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor != nullptr)
    {
        if (AEnemyActor* actor = Cast<AEnemyActor>(OtherActor))
        {
            if (actor != nullptr)
            {
                if (actor->getIsEnemy())
                {
                    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("OVERLAP")));
                    actor->Destroy();
                }
            }
        }
    }
}

void AAllyActor::UpdateLocation(float DeltaTime)
{
    if (Speed != 0.0f)
    {
        FVector CurrentLocation = GetActorLocation();
        FVector NewLocation = CurrentLocation + (Direction * Speed * DeltaTime);
        SetActorLocation(NewLocation);
    }
}
