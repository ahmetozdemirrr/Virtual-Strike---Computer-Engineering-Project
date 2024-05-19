// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "AllyActor.generated.h"

UCLASS()
class GAME_API AAllyActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAllyActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	//	UPROPERTY(EditAnywhere)
//	UBoxComponent* CollisionComp;
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* MeshComp;
	//UMaterial* MaterialRed = LoadObject<UMaterial>(nullptr, "../../../Content/LevelPrototyping/Materials/MI_Solid_Blue.MI_Solid_Blue";

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	/** Returns CollisionComp subobject **/
//	UBoxComponent* GetCollisionComp() const { return CollisionComp; }
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UStaticMeshComponent* GetStaticMeshComponent() const { return MeshComp; }
};
