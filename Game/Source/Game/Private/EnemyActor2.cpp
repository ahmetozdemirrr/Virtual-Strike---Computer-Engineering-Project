// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyActor2.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include <Game/GameProjectile.h>
#include "Components/BoxComponent.h"
#include <Game/GameCharacter.h>
#include <Kismet/GameplayStatics.h>


// Sets default values
AEnemyActor2::AEnemyActor2()
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
void AEnemyActor2::BeginPlay()
{
	Super::BeginPlay();
	Speed = 300.0f;
	PlayerCharacter = Cast<AGameCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	MeshComp->OnComponentHit.AddDynamic(this, &AEnemyActor2::OnHit);		// set up a notification for when this component hits something blocking
}

// Called every frame
void AEnemyActor2::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemyActor2::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
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
