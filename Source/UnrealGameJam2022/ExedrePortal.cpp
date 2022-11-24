// Fill out your copyright notice in the Description page of Project Settings.


#include "ExedrePortal.h"
#include "BasePlayerController.h"
#include "BaseCharacter.h"

// Sets default values
AExedrePortal::AExedrePortal(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick   = true;
	bIsActive                       = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent->Mobility = EComponentMobility::Static;

	PortalRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("PortalRootComponent"));
	PortalRootComponent->SetupAttachment( GetRootComponent() );
	PortalRootComponent->SetRelativeLocation( FVector(0.0f, 0.0f, 0.0f) );
	PortalRootComponent->SetRelativeRotation( FRotator(0.0f, 0.0f, 0.0f) );
	PortalRootComponent->Mobility = EComponentMobility::Movable;
}

bool AExedrePortal::IsActive()
{
	return bIsActive;
}

void AExedrePortal::SetActive( bool NewActive )
{
	bIsActive = NewActive;
}

void AExedrePortal::ClearRTT_Implementation()
{

}

void AExedrePortal::SetRTT_Implementation( UTexture* RenderTexture )
{

}

void AExedrePortal::ForceTick_Implementation()
{

}

AActor* AExedrePortal::GetTarget()
{
	return Target;
}

void AExedrePortal::SetTarget( AActor* NewTarget )
{
	Target = NewTarget;
}

bool AExedrePortal::IsPointInFrontOfPortal( FVector Point, FVector PortalLocation, FVector PortalNormal )
{
	FPlane PortalPlane  = FPlane( PortalLocation, PortalNormal );
	float PortalDot     = PortalPlane.PlaneDot( Point );

	//If < 0 means we are behind the Plane
	//See : http://api.unrealengine.com/INT/API/Runtime/Core/Math/FPlane/PlaneDot/index.html
	return ( PortalDot >= 0 );
}

bool AExedrePortal::IsPointCrossingPortal( FVector Point, FVector PortalLocation, FVector PortalNormal )
{
	FVector IntersectionPoint;
	FPlane PortalPlane  = FPlane( PortalLocation, PortalNormal );
	float PortalDot     = PortalPlane.PlaneDot( Point );
	bool IsCrossing     = false;
	bool IsInFront      = PortalDot >= 0;

	bool IsIntersect    = FMath::SegmentPlaneIntersection(  LastPosition,
															Point,
															PortalPlane,
															IntersectionPoint );

	// Did we intersect the portal since last Location ?
	// If yes, check the direction : crossing forward means we were in front and now at the back
	// If we crossed backward, ignore it (similar to Prey 2006)
	if( IsIntersect && !IsInFront && LastInFront )
	{
		IsCrossing  = true;
	}

	// Store values for next check
	LastInFront     = IsInFront;
	LastPosition    = Point;

	return IsCrossing;
}
void AExedrePortal::TeleportActor( AActor* ActorToTeleport )
{
    if( ActorToTeleport == nullptr || Target == nullptr )
    {
        return;
    }

    //-------------------------------
    //Retrieve and save Player Velocity
    //(from the Movement Component)
    //--------------- ----------------
    FVector SavedVelocity   = FVector::ZeroVector;
    ABaseCharacter* EC    = nullptr;

    if( ActorToTeleport->IsA( ABaseCharacter::StaticClass() ) )
    {
        EC = Cast<ABaseCharacter>( ActorToTeleport );

        SavedVelocity = EC->GetVelocity();
    }

    //-------------------------------
    //Compute and apply new location
    //-------------------------------
    FHitResult HitResult;
    FVector NewLocation = ConvertLocationToActorSpace(   ActorToTeleport->GetActorLocation(),
                                                                this,
                                                                Target );

    ActorToTeleport->SetActorLocation(  NewLocation,
                                        false,
                                        &HitResult,
                                        ETeleportType::TeleportPhysics );

    //-------------------------------
    //Compute and apply new rotation
    //-------------------------------
    FRotator NewRotation = ConvertRotationToActorSpace(  ActorToTeleport->GetActorRotation(),
                                                                this,
                                                                Target );

    //Apply new rotation
    ActorToTeleport->SetActorRotation( NewRotation );

    //-------------------------------
    //If we are teleporting a character we need to
    //update its controller as well and reapply its velocity
    //-------------------------------
    if( ActorToTeleport->IsA( ABaseCharacter::StaticClass() ) )
    {
        //Update Controller
        AController* EPC =  Cast<APlayerController>(EC->GetController());

        if( EPC != nullptr )
        {
            NewRotation = ConvertRotationToActorSpace(   EPC->GetControlRotation(),
                                                                this,
                                                                Target );

            EPC->SetControlRotation( NewRotation );
        }

        //Reapply Velocity (Need to reorient direction into local space of Portal)
        {
            FVector Dots;
            Dots.X  = FVector::DotProduct( SavedVelocity, GetActorForwardVector() );
            Dots.Y  = FVector::DotProduct( SavedVelocity, GetActorRightVector() );
            Dots.Z  = FVector::DotProduct( SavedVelocity, GetActorUpVector() );

            FVector NewVelocity     = Dots.X * Target->GetActorForwardVector()
                                    + Dots.Y * Target->GetActorRightVector()
                                    + Dots.Z * Target->GetActorUpVector();

            EC->GetVelocity() = NewVelocity;
        }
    }

    //Cleanup Teleport
    LastPosition = NewLocation;
}

bool AExedrePortal::IsPointInsideBox(FVector Point, UBoxComponent* Box)
{
	if( Box != nullptr )
	{
		// From :
		// https://stackoverflow.com/questions/52673935/check-if-3d-point-inside-a-box/52674010

		FVector Center      = Box->GetComponentLocation();
		FVector Half        = Box->GetScaledBoxExtent();
		FVector DirectionX  = Box->GetForwardVector();
		FVector DirectionY  = Box->GetRightVector();
		FVector DirectionZ  = Box->GetUpVector();

		FVector Direction   = Point - Center;

		bool IsInside = FMath::Abs( FVector::DotProduct( Direction, DirectionX ) ) <= Half.X &&
						FMath::Abs( FVector::DotProduct( Direction, DirectionY ) ) <= Half.Y &&
						FMath::Abs( FVector::DotProduct( Direction, DirectionZ ) ) <= Half.Z;

		return IsInside;
	}
	else
	{
		return false;
	}
}

AExedrePortalManager* AExedrePortal::GetPortalManager(AActor* Context)
{
	AExedrePortalManager* Manager = nullptr;

	//Retrieve the World from the Context actor
	if( Context != nullptr && Context->GetWorld() != nullptr )
	{
		//Find PlayerController
		ABasePlayerController* EPC = Cast<ABasePlayerController>( Context->GetWorld()->GetFirstPlayerController() );

		//Retrieve the Portal Manager
		if( EPC != nullptr && EPC->GetPortalManager() != nullptr )
		{
			Manager = EPC->GetPortalManager();
		}
	}

	return Manager;
}

FVector AExedrePortal::ConvertLocationToActorSpace(FVector Location, AActor* Reference, AActor* Targetf)
{
	if( Reference == nullptr || Targetf == nullptr )
	{
		return FVector::ZeroVector;
	}

	FVector Direction       = Location - Reference->GetActorLocation();
	FVector TargetLocation  = Targetf->GetActorLocation();

	FVector Dots;
	Dots.X  = FVector::DotProduct( Direction, Reference->GetActorForwardVector() );
	Dots.Y  = FVector::DotProduct( Direction, Reference->GetActorRightVector() );
	Dots.Z  = FVector::DotProduct( Direction, Reference->GetActorUpVector() );

	FVector NewDirection    = Dots.X * Targetf->GetActorForwardVector()
							+ Dots.Y * Targetf->GetActorRightVector()
							+ Dots.Z * Targetf->GetActorUpVector();

	return TargetLocation + NewDirection;
}

FRotator AExedrePortal::ConvertRotationToActorSpace(FRotator Rotation, AActor* Reference, AActor* Targetf)
{
	if( Reference == nullptr || Targetf == nullptr )
	{
		return FRotator::ZeroRotator;
	}

	FTransform SourceTransform  = Reference->GetActorTransform();
	FTransform TargetTransform  = Targetf->GetActorTransform();
	FQuat QuatRotation          = FQuat( Rotation );

	FQuat LocalQuat             = SourceTransform.GetRotation().Inverse() * QuatRotation;
	FQuat NewWorldQuat          = TargetTransform.GetRotation() * LocalQuat;

	return NewWorldQuat.Rotator();
}
