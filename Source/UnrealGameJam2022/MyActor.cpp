// Fill out your copyright notice in the Description page of Project Settings.


#include "MyActor.h"

#include "DrawDebugHelpers.h"

// Sets default values
AMyActor::AMyActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	explode_actor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	explode_actor->SetupAttachment(RootComponent);

}

TArray<FVector> AMyActor::MeshData(const UStaticMeshComponent* StaticMeshComponent)
{
	TArray<FVector> vertices = TArray<FVector>();

	// Vertex Buffer
	if (!IsValidLowLevel()) return vertices;
	if (!StaticMeshComponent) return vertices;
	if (!StaticMeshComponent->GetStaticMesh()) return vertices;
	if (!StaticMeshComponent->GetStaticMesh()->RenderData) return vertices;
	if (StaticMeshComponent->GetStaticMesh()->RenderData->LODResources.Num() > 0)
	{
		FPositionVertexBuffer* VertexBuffer = &StaticMeshComponent->GetStaticMesh()->RenderData->LODResources[0].VertexBuffers.PositionVertexBuffer;
		if (VertexBuffer)
		{
			const int32 VertexCount = VertexBuffer->GetNumVertices();
			for (int32 Index = 0; Index < VertexCount; Index++)
			{
				//This is in the Static Mesh Actor Class, so it is location and tranform of the SMActor
				const FVector WorldSpaceVertexLocation = GetActorLocation() + GetTransform().TransformVector(VertexBuffer->VertexPosition(Index));
				//add to output FVector array
				vertices.Add(WorldSpaceVertexLocation);
			}
		}
	}

	return vertices;
}

// Called when the game starts or when spawned
void AMyActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TArray<FVector> Points = MeshData(explode_actor);
	for (FVector point : Points)
	{
		DrawDebugPoint(GetWorld(), point, 20, FColor::Orange);
		DrawDebugSphere(GetWorld(), point,20, 20, FColor::Orange);
	}

}

