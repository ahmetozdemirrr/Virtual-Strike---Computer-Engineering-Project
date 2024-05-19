// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyActor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include <Game/GameProjectile.h>
#include "Components/BoxComponent.h"
#include <Game/GameCharacter.h>


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

}

// Called when the game starts or when spawned
void AEnemyActor::BeginPlay()
{
	Super::BeginPlay();
	MeshComp->OnComponentHit.AddDynamic(this, &AEnemyActor::OnHit);		// set up a notification for when this component hits something blocking
	
}

// Called every frame
void AEnemyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
				this->Destroy();
			}
		}
	}
}

