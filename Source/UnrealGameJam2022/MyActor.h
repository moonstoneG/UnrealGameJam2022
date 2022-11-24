// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyActor.generated.h"

UCLASS()
class UNREALGAMEJAM2022_API AMyActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyActor();
	UFUNCTION(BlueprintPure, Category = "Corridor", meta = (Keywords = "corridor vertex mesh meshdata", NativeBreakFunc))
    TArray<FVector> MeshData(const UStaticMeshComponent* StaticMeshComponent);

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	UStaticMeshComponent *explode_actor;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
