// Fill out your copyright notice in the Description page of Project Settings.

#include "MenuActor.h"
#include "EnemyActor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include <Game/GameProjectile.h>
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMenuActor::AMenuActor()
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
void AMenuActor::BeginPlay()
{
	Super::BeginPlay();
	MeshComp->OnComponentHit.AddDynamic(this, &AMenuActor::OnHit);		// set up a notification for when this component hits something blocking
	//MeshComp->OnCom.AddDynamic(this, &AAllyActor::OnOverlapBegin);
}

// Called every frame
void AMenuActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMenuActor::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor != nullptr)
	{
		if (AGameProjectile* actor = Cast<AGameProjectile>(OtherActor))
		{
			if (actor != nullptr)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Restart")));
				if (choice == 0)
				{
					UGameplayStatics::OpenLevel(GetWorld(), "FirstPersonMap");
				}
				else if (choice == 1)
				{
					UGameplayStatics::OpenLevel(GetWorld(), "Polygon");
				}
				else if (choice == 2)
				{
					UGameplayStatics::OpenLevel(GetWorld(), "Level2");
				}
				this->Destroy();
			}
		}
	}
}

void AMenuActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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

