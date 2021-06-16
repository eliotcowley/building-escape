// Copyright Eliot Cowley 2021

#include "OpenDoor.h"

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();
	this->InitialYaw = GetOwner()->GetActorRotation().Yaw;
	this->CurrentYaw = this->InitialYaw;

	if (!PressurePlate)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("%s has the OpenDoor component on it, but no PressurePlate set."),
			*GetOwner()->GetName());
	}

	ActorThatOpens = GetWorld()->GetFirstPlayerController()->GetPawn();
}

// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (PressurePlate && this->PressurePlate->IsOverlappingActor(this->ActorThatOpens))
	{
		this->OpenDoor(DeltaTime);
		DoorLastOpened = GetWorld()->GetTimeSeconds();
	}
	else
	{
		if (GetWorld()->GetTimeSeconds() - DoorLastOpened >= DoorCloseDelay)
		{
			this->CloseDoor(DeltaTime);
		}
	}
}

void UOpenDoor::OpenDoor(float DeltaTime)
{
	FRotator CurrentRotation = GetOwner()->GetActorRotation();
	this->CurrentYaw = CurrentRotation.Yaw;
	FRotator OpenRotation{CurrentRotation};

	OpenRotation.Yaw = FMath::FInterpTo(
		this->CurrentYaw,
		this->InitialYaw + this->OpenAngle,
		DeltaTime,
		this->DoorOpenSpeed);

	GetOwner()->SetActorRotation(OpenRotation);
}

void UOpenDoor::CloseDoor(float DeltaTime)
{
	FRotator CurrentRotation = GetOwner()->GetActorRotation();
	this->CurrentYaw = CurrentRotation.Yaw;
	FRotator ClosedRotation{CurrentRotation};

	ClosedRotation.Yaw = FMath::FInterpTo(
		this->CurrentYaw,
		this->InitialYaw,
		DeltaTime,
		this->DoorCloseSpeed);

	GetOwner()->SetActorRotation(ClosedRotation);
}