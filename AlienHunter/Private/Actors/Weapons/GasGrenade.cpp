// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Weapons/GasGrenade.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AISense_Hearing.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/AudioComponent.h"

AGasGrenade::AGasGrenade()
{
	
}

void AGasGrenade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GrenadeEffectsTimeline.IsPlaying())
	{
		GrenadeEffectsTimeline.TickTimeline(DeltaTime);
	}

}

void AGasGrenade::BeginPlay()
{
	Super::BeginPlay();

	InitializeScaleEffectsTimeline();
	
	TargetRadialDamageParams = FRadialDamageParams(MaxDamage, MinDamage, DamageInnerRadius, DamageOuterRadius, 
		DamageTypeClass.GetDefaultObject()->DamageFalloff);
}

void AGasGrenade::Activate()
{
	SetLifeSpan(SmokeCloudLifeSpan);

	GetMesh()->SetSimulatePhysics(false);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetMobility(EComponentMobility::Stationary);

	TriggerSoundAndVisuals();

	GetWorldTimerManager().SetTimer(RadialDamageHandle, this, &AGasGrenade::ApplyDamage, 1, true);

	GrenadeEffectsTimeline.PlayFromStart();
}

void AGasGrenade::LifeSpanExpired()
{
	GetWorldTimerManager().ClearAllTimersForObject(this);

	if (GrenadeVisualComponent)
	{
		GrenadeVisualComponent->DestroyComponent();
	}
	if (GrenadeSoundComponent)
	{
		GrenadeSoundComponent->DestroyComponent();
	}

	Super::LifeSpanExpired();
}

void AGasGrenade::SetEffectScale(float Scale)
{
	const FVector ScaleVector(Scale * VisualEffectMaxScale);
	if (GrenadeVisualComponent)
	{
		GrenadeVisualComponent->SetWorldScale3D(ScaleVector);
	}

	if (GrenadeSoundComponent)
	{
		GrenadeSoundComponent->SetVolumeMultiplier(Scale);
	}

	MinDamage = TargetRadialDamageParams.MinimumDamage * Scale;
	MaxDamage = TargetRadialDamageParams.BaseDamage * Scale;
	DamageInnerRadius = TargetRadialDamageParams.InnerRadius * Scale;
	DamageOuterRadius = TargetRadialDamageParams.OuterRadius * Scale;
}

void AGasGrenade::InitializeScaleEffectsTimeline()
{
	FOnTimelineFloat ProgressFunction;
	ProgressFunction.BindUFunction(this, TEXT("SetEffectScale"));
	GrenadeEffectsTimeline.AddInterpFloat(ScaleEffectsCurve, ProgressFunction);
	GrenadeEffectsTimeline.SetTimelineLengthMode(TL_LastKeyFrame);

	/** Because the base timeline lasts 1 seconds */
	const float TimelinePlayRate = 1.0f / SmokeCloudLifeSpan;
	GrenadeEffectsTimeline.SetPlayRate(TimelinePlayRate);
}