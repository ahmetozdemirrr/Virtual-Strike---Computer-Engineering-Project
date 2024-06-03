// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameCharacter.h"
#include "GameProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Async/Async.h"
#include "Engine/TextureRenderTarget2D.h"
#include "ImageUtils.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Modules/ModuleManager.h"
#include "Runtime/Online/WebSockets/Public/WebSocketsModule.h"
#include <Kismet/GameplayStatics.h>

//////////////////////////////////////////////////////////////////////////
// AGameCharacter

AGameCharacter::AGameCharacter()
{
	//IPSocket = "172.20.10.2";
	IPSocket = "192.168.148.137";
	fire = 0;
	Score = 0;
	// Character doesnt have a rifle at start
	bHasRifle = false;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(0.0f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	
	//Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(0.0f, 0.f, -150.f));
	/*---------------------------------------------------------------------*/

	MobileSocket = FWebSocketsModule::Get().CreateWebSocket("ws://"+ IPSocket +":3030");
	SensorSocket = FWebSocketsModule::Get().CreateWebSocket("ws://"+ IPSocket+":3000");
	WebSocket = FWebSocketsModule::Get().CreateWebSocket("ws://"+ IPSocket +":8080");
	RenderTarget = CreateDefaultSubobject<UTextureRenderTarget2D>(TEXT("RenderTarget"));
	RenderTarget->InitAutoFormat(800, 600); // Çözünürlük ayarlarý
	RenderTarget->ClearColor = FLinearColor::Black; // RenderTarget'ý temizle (siyah)
	RenderTarget->UpdateResourceImmediate(true);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(FirstPersonCameraComponent);
	FollowCamera->bUsePawnControlRotation = true;

	SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCapture"));
	SceneCapture->SetupAttachment(FollowCamera);
	SceneCapture->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
}

void AGameCharacter::BeginPlay()
{
	Super::BeginPlay();
	StartAsyncInitMobileTimer();
	Init(); /* Kol için çalýþan fonksiyon */
	InitMobileSensor(); /* Kafa hareketlerini jiroskopu aktif eder */
	StartAsyncInit();
	WebSocket->Connect();
	MobileSocket->Connect();
	SensorSocket->Connect();

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, "BEGINPLAY!");
}
void AGameCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);
	GetWorldTimerManager().ClearTimer(TimerHandle);
	GetWorldTimerManager().ClearTimer(InitTimerHandle);
	GetWorldTimerManager().ClearTimer(UpdateTimerHandle);
	GetWorldTimerManager().ClearTimer(AsyncInitMobileTimerHandle);
	GetWorldTimerManager().ClearTimer(UnpauseTimerHandle);

	// Pause the game by setting the global time dilation to 0

	Shutdown();
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, "ENDPLAY!");


}
void AGameCharacter::ResumeGame()
{
	// Restore the global time dilation to 1 (normal speed)
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
}

void AGameCharacter::Init() {
	if (!FModuleManager::Get().IsModuleLoaded("WebSockets")) {
		FModuleManager::Get().LoadModule("WebSockets");
	}
		WebSocket->OnConnected().AddLambda([]() {
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, "Successfully connected");
			});

		WebSocket->OnConnectionError().AddLambda([](const FString& Error) {
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Error);
			});

		WebSocket->OnClosed().AddLambda([](int32 StatusCode, const FString& Reason, bool bWasClean) {
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, bWasClean ? FColor::Green : FColor::Red, "Connection closed " + Reason);
			});

		WebSocket->OnMessage().AddLambda([this](const FString& MessageString) {
			if (!MessageString.IsEmpty()) {
				TArray<FString> Parts;
				MessageString.ParseIntoArray(Parts, TEXT(" "), true);

				if (Parts.Num() == 3) {
					int X = FCString::Atoi(*Parts[0]);
					int Y = FCString::Atoi(*Parts[1]);
					 Sw = FCString::Atoi(*Parts[2]);

					if (X == 1) {
						setDirectionX(1);
					}
					else if (X == -1) {
						setDirectionX(-1);
					}
					else {
						setDirectionX(0);
					}

					if (Y == 1) {
						setDirectionY(1);
					}
					else if (Y == -1) {
						setDirectionY(-1);
					}
					else {
						setDirectionY(0);
					}
					//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, FString::Printf(TEXT("%d %d %d"), X, Y, Sw));
			
				}	
			}
			});

		WebSocket->OnMessageSent().AddLambda([](const FString& MessageString) {
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Sent message: " + MessageString);
			});

		WebSocket->Connect();
	
	
}


void AGameCharacter::InitMobileSensor() {
	LastX = 0.0f;
	LastY = 0.0f;
	LastFilteredY = 0.0f;
	const float SmoothingFactor = 0.1f;

	if (!FModuleManager::Get().IsModuleLoaded("WebSockets")) {
		FModuleManager::Get().LoadModule("WebSockets");
	}

	SensorSocket->OnConnected().AddLambda([]() {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, "Successfully connected");
		});

	SensorSocket->OnConnectionError().AddLambda([](const FString& Error) {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Error);
		});

	SensorSocket->OnClosed().AddLambda([](int32 StatusCode, const FString& Reason, bool bWasClean) {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, bWasClean ? FColor::Green : FColor::Red, "Connection closed " + Reason);
		});

	SensorSocket->OnMessage().AddLambda([this, SmoothingFactor](const FString& MessageString) {
		if (!MessageString.IsEmpty()) {
			TArray<FString> Parts;
			MessageString.ParseIntoArray(Parts, TEXT(" "), true);

			if (Parts.Num() == 4) {
				X = FCString::Atof(*Parts[0]);
				Y = FCString::Atof(*Parts[1]);
				Z = FCString::Atof(*Parts[2]);
				W = FCString::Atof(*Parts[3]);

				float FilteredY = LastFilteredY + (Y - LastFilteredY) * SmoothingFactor;
				LastFilteredY = FilteredY;

				newX = 0.0f;
				newY = 0.0f;
				if (FMath::Abs(LastX - X) > 0.0005f) {
					deltaAngleX = LastX - X;
					if (Z < 0.0) {
						deltaAngleX *= -1;
					}
					newX = deltaAngleX * -60.0f;
				}

				if (FMath::Abs(LastY - FilteredY) > 0.08f) {
					if (W < 0.0) {
						deltaAngleY *= -1;
					}
					else {
						deltaAngleY = LastY - FilteredY;
					}
					newY = deltaAngleY * 0.5f;
				}

				LastX = X;
				LastY = Y;
				AddControllerYawInput(newX);
				AddControllerPitchInput(newY);
			}
		}
		});

	SensorSocket->OnMessageSent().AddLambda([](const FString& MessageString) {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Sent message: " + MessageString);
		});

	SensorSocket->Connect();
}



void AGameCharacter::Shutdown() {
	if (WebSocket.IsValid() && WebSocket->IsConnected()) {
		WebSocket->Close();
	}
	if (MobileSocket.IsValid() && MobileSocket->IsConnected()) {
		MobileSocket->Close();
	}
	if (SensorSocket.IsValid() && SensorSocket->IsConnected()) {
		SensorSocket->Close();
	}
}

void AGameCharacter::setDirectionX(int32 DirectionX) {
	directionX = DirectionX;
}
void AGameCharacter::setDirectionY(int32 DirectionY) {
	directionY = DirectionY;
}
void AGameCharacter::setSwitch(int32 SwitchButton) {
	switchButton = SwitchButton;
}
int32 AGameCharacter::getDirectionX() {
	return directionX;
}
int32 AGameCharacter::getDirectionY() {
	return directionY;
}
int32 AGameCharacter::getSwitch() {
	return switchButton;
}
void AGameCharacter::MoveFunction() {
	int DirectionX = getDirectionX();
	int DirectionY = getDirectionY();
	FVector currentDirectionX = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
	FVector currentDirectionY = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
	switch (DirectionX) {
	case 1:
		AddMovementInput(currentDirectionX, 1.0);
		break;
	case 0:
		AddMovementInput(currentDirectionX, 0.0);
		break;
	case -1:
		AddMovementInput(currentDirectionX, -1.0);
		break;
	}

	switch (DirectionY) {
	case 1:
		AddMovementInput(currentDirectionY, 1.0);
		break;
	case 0:
		AddMovementInput(currentDirectionY, 0.0);
		break;
	case -1:
		AddMovementInput(currentDirectionY, -1.0);
		break;
	}
}

void AGameCharacter::AsyncInitMobile() {
	AsyncTask(ENamedThreads::GameThread, [this]() {
		if (!FModuleManager::Get().IsModuleLoaded("WebSockets")) {
			FModuleManager::Get().LoadModule("WebSockets");
		}

		SceneCapture->TextureTarget = RenderTarget;
		SceneCapture->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
		SceneCapture->CaptureScene();
	

		// Render target'dan piksel verilerini alýr
		FTextureRenderTargetResource* RenderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();
		TArray<FColor> Bitmap;
		RenderTargetResource->ReadPixels(Bitmap);
		TArray<uint8> PNGData;
		FImageUtils::CompressImageArray(RenderTarget->SizeX, RenderTarget->SizeY, Bitmap, PNGData);

		AsyncTask(ENamedThreads::GameThread, [this, PNGData]() {
			if (MobileSocket.IsValid() && MobileSocket->IsConnected()) {
				MobileSocket->Send(PNGData.GetData(), PNGData.Num(),true);
				
			}
			});
		});
}
void AGameCharacter::StartAsyncInitMobileTimer()
{
	// Set the timer to call AsyncInitMobile every 0.1 seconds
	GetWorldTimerManager().SetTimer(AsyncInitMobileTimerHandle, this, &AGameCharacter::AsyncInitMobile, 0.05f, true);
}
void AGameCharacter::StartAsyncInit()
{
	// Set the timer to call AsyncInitMobile every 0.1 seconds
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AGameCharacter::MoveFunction, 0.01f, true);
}
void AGameCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	

}

//////////////////////////////////////////////////////////////////////////// Input

void AGameCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGameCharacter::MoveRight);
	PlayerInputComponent->BindAxis("MoveForward", this, &AGameCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Turn", this, &AGameCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &AGameCharacter::LookUpAtRate);
}


void AGameCharacter::SetHasRifle(bool bNewHasRifle)
{
	bHasRifle = bNewHasRifle;
}

bool AGameCharacter::GetHasRifle()
{
	return bHasRifle;
}

void AGameCharacter::setScore(int newScore) {
	Score += newScore;
}

void AGameCharacter::MoveForward(float Value)
{
	// Hareket yönünü ayarla
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
	// Karakteri ileriye doðru hareket ettir
	AddMovementInput(Direction, Value);
}

void AGameCharacter::MoveRight(float Value)
{
	// Hareket yönünü ayarla
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
	// Karakteri yana doðru hareket ettir
	AddMovementInput(Direction, Value);
}
void AGameCharacter::TurnAtRate(float Rate)
{
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, FString::Printf(TEXT("%f"),Rate));
	AddControllerYawInput(Rate);
}

void AGameCharacter::LookUpAtRate(float Rate)
{
	Rate = Rate * (-1);
	AddControllerPitchInput(Rate);
}