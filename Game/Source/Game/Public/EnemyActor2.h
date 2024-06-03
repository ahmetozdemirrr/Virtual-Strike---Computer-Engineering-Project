// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include <Game/GameCharacter.h>
#include "EnemyActor2.generated.h"

UCLASS()
class GAME_API AEnemyActor2 : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AEnemyActor2();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	bool isEnemy = true;
public:
	//	UPROPERTY(EditAnywhere)
//	UBoxComponent* CollisionComp;
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* MeshComp;
	//UMaterial* MaterialRed = LoadObject<UMaterial>(nullptr, "../../../Content/LevelPrototyping/Materials/MI_Solid_Blue.MI_Solid_Blue";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UParticleSystem* DestroyEffect;

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	/** Returns CollisionComp subobject **/
//	UBoxComponent* GetCollisionComp() const { return CollisionComp; }
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	UStaticMeshComponent* GetStaticMeshComponent() const { return MeshComp; }
	bool getIsEnemy() { return isEnemy; }

private:

	FVector Direction;

	AGameCharacter* PlayerCharacter;
};
