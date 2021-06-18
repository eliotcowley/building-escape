// Copyright Eliot Cowley 2021

#include "OpenDoor.h"

#define OUT

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
	FindPressurePlate();
	FindAudioComponent();
}

// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (PressurePlate && TotalMassOfActors() > this->DoorOpenMass)
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
	this->bDoorClosed = false;

	if (!AudioComponent)
		return;

	if (!this->bDoorOpened)
	{
		AudioComponent->Play();
		this->bDoorOpened = true;
	}
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
	this->bDoorOpened = false;

	if (!AudioComponent)
		return;

	if (!this->bDoorClosed)
	{
		AudioComponent->Play();
		this->bDoorClosed = true;
	}
}

float UOpenDoor::TotalMassOfActors() const
{
	float TotalMass = 0.f;
	TArray<AActor*> OverlappingActors;

	if (!PressurePlate)
	{
		UE_LOG(
			LogTemp, 
			Error, 
			TEXT("No pressure plate assigned to %s"), 
			*this->GetOwner()->GetName());

		return TotalMass;
	}

	PressurePlate->GetOverlappingActors(OUT OverlappingActors);

	for (int32 i = 0; i < OverlappingActors.Num(); i++)
	{
		TotalMass += 
			OverlappingActors[i]->
			FindComponentByClass<UPrimitiveComponent>()->
			GetMass();
	}

	return TotalMass;
}

void UOpenDoor::FindAudioComponent()
{
	AudioComponent = GetOwner()->FindComponentByClass<UAudioComponent>();

	if (!AudioComponent)
	{
		UE_LOG(
			LogTemp, 
			Error, 
			TEXT("%s missing audio component"), *GetOwner()->GetName());
	}
}

void UOpenDoor::FindPressurePlate()
{
	if (!PressurePlate)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("%s has the OpenDoor component on it, but no PressurePlate set.	"),
			*GetOwner()->GetName());
	}
}