// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "GameFramework/Actor.h"
#include "ExedrePortal.h"
#include "ExedrePortalManager.generated.h"

class ABasePlayerController;


UCLASS()
class UNREALGAMEJAM2022_API AExedrePortalManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExedrePortalManager(const FObjectInitializer& ObjectInitializer);
	// Called by a Portal actor when wanting to teleport something
	UFUNCTION(BlueprintCallable, Category="Portal")
	void RequestTeleportByPortal( AExedrePortal* Portal, AActor* TargetToTeleport );

	// Save a reference to the PlayerControler
	void SetControllerOwner( ABasePlayerController* NewOwner );

	// Various setup that happens during spawn
	void Init();

	// Manual Tick
	void Update( float DeltaTime );

	// Find all the portals in world and update them
	// returns the most valid/usable one for the Player
	AExedrePortal* UpdatePortalsInWorld();

	// Update SceneCapture
	void UpdateCapture( AExedrePortal* Portal );

public:

private:
	//Function to create the Portal render target
	void GeneratePortalTexture();

	UPROPERTY()
	USceneCaptureComponent2D* SceneCapture;

	UPROPERTY(transient)
	UTextureRenderTarget2D* PortalTexture;

	UPROPERTY()
	ABasePlayerController* ControllerOwner;

	UPROPERTY()
	ABaseCharacter* Character;

	int32 PreviousScreenSizeX;
	int32 PreviousScreenSizeY;

	float UpdateDelay;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
