// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Runtime/Online/WebSockets/Public/IWebSocket.h"
#include "Logging/LogMacros.h"
#include "GameCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AGameCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* MoveAction;
	
public:
	virtual void Tick(float DeltaTime) override;
	AGameCharacter();
	void Init();
	void InitMobileSensor();
	void AsyncInitMobile();
	void Shutdown();
	TSharedPtr<IWebSocket> WebSocket;/*Kol*/
	TSharedPtr<IWebSocket> MobileSocket;/*Görüntü*/
	TSharedPtr<IWebSocket> SensorSocket;/*Jiroskop*/
	void MoveFunction();
	void setDirectionX(int32 directionX);
	void setDirectionY(int32 directionY);
	void setSwitch(int32 switchButton);
	int32 getDirectionX();
	int32 getDirectionY();
	int32 getSwitch();
	UTextureRenderTarget2D* RenderTarget;
	void MoveRight(float Value);
	void MoveForward(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);
private:
	int32 directionX;
	int32 directionY;
	int32 switchButton;
	int32 MoveDirection;
	float LastX;
	float LastY;
	FTimerHandle InitTimerHandle;
	FTimerHandle UpdateTimerHandle;
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* CameraArm;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* FollowCamera;
	class USceneCaptureComponent2D* SceneCapture;

protected:
	virtual void BeginPlay();

public:
		
	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/** Bool for AnimBP to switch to another animation set */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
	bool bHasRifle;

	/** Setter to set the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	void SetHasRifle(bool bNewHasRifle);

	/** Getter for the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	bool GetHasRifle();

protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
public:
	UPROPERTY(EditAnywhere)
	int Score = 0;
	void setScore(int score);
	UFUNCTION(BlueprintCallable)
	int getScore() { return Score; }
	UFUNCTION(BlueprintImplementableEvent, Category = "Teleport")
	void OnMapChange();
};