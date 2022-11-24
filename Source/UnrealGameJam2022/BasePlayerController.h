// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ExedrePortalManager.h"
#include "GameFramework/PlayerController.h"
#include "BasePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class UNREALGAMEJAM2022_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()

	virtual void BeginPlay() override;

public:
	UPROPERTY()
	AExedrePortalManager* PortalManager;

	 AExedrePortalManager* GetPortalManager(){return PortalManager;}

	ABaseCharacter* GetCharacter(){return  Cast<ABaseCharacter>( GetPawn());}
	FMatrix GetCameraProjectionMatrix();
};
