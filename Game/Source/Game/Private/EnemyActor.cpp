// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyActor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include <Game/GameProjectile.h>
#include "Components/BoxComponent.h"
#include <Game/GameCharacter.h>
#include <Kismet/GameplayStatics.h>


// Sets default values
AEnemyActor::AEnemyActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create the mesh component and attach it to the root component
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(GetRootComponent());
	// Create the collision component and attach it to the mesh component
	//CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	//CollisionComp->SetupAttachment(MeshComp);

	// Initialize default values
	Direction = FVector::BackwardVector;
}

// Called when the game starts or when spawned
void AEnemyActor::BeginPlay()
{
	Super::BeginPlay();
	Speed = 300.0f;
	PlayerCharacter = Cast<AGameCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	MeshComp->OnComponentHit.AddDynamic(this, &AEnemyActor::OnHit);		// set up a notification for when this component hits something blocking
}

// Called every frame
void AEnemyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateLocation(DeltaTime);
	// Check the actor's location
	FVector CurrentLocation = GetActorLocation();
	if (CurrentLocation.X < 7000.0f)
	{
		if (DestroyEffect)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DestroyEffect, GetActorLocation());
		}
		if (PlayerCharacter)
		{
			// Decrease the score by 5
			PlayerCharacter->Score -= 5;
		}
		Destroy();
	}
}

void AEnemyActor::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor != nullptr)
	{
		if (AGameProjectile* actor = Cast<AGameProjectile>(OtherActor))
		{
			if (actor != nullptr)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Enemy Hit")));
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DestroyEffect, GetActorLocation());
				PlayerCharacter->Score += 5;
				this->Destroy();
			}
		}
	}
}

void AEnemyActor::UpdateLocation(float DeltaTime)
{
	if (Speed != 0.0f)
	{
		FVector CurrentLocation = GetActorLocation();
		FVector NewLocation = CurrentLocation + (Direction * Speed * DeltaTime);
		SetActorLocation(NewLocation);
	}
}
