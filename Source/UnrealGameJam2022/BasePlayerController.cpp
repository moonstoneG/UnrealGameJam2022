// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerController.h"

void ABasePlayerController::BeginPlay()
{

	Super::BeginPlay();
	FActorSpawnParameters SpawnParams;

	PortalManager = nullptr;
	PortalManager = GetWorld()->SpawnActor<AExedrePortalManager>(   AExedrePortalManager::StaticClass(),
																	FVector::ZeroVector,
																	FRotator::ZeroRotator,
																	SpawnParams);
	PortalManager->AttachToActor( this, FAttachmentTransformRules::SnapToTargetIncludingScale);
	PortalManager->SetControllerOwner( this );
	PortalManager->Init();
}
FMatrix ABasePlayerController::GetCameraProjectionMatrix()
{
	FMatrix ProjectionMatrix;

	if( GetLocalPlayer() != nullptr )
	{
		FSceneViewProjectionData PlayerProjectionData;

		GetLocalPlayer()->GetProjectionData( GetLocalPlayer()->ViewportClient->Viewport,
										EStereoscopicPass::eSSP_FULL,
										PlayerProjectionData );

		ProjectionMatrix = PlayerProjectionData.ProjectionMatrix;
	}

	return ProjectionMatrix;
}