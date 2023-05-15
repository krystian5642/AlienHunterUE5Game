// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifys/MeleeAttackNotifyState.h"
#include "AlienHunter/AlienHunterCharacter.h"

void UMeleeAttackNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	if (!MeshComp)
	{
		return;
	}
	if (AAlienHunterCharacter* Character = Cast<AAlienHunterCharacter>(MeshComp->GetOwner()))
	{
		Character->ActivateMeleeBoxes();
	}
}

void UMeleeAttackNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (!MeshComp)
	{
		return;
	}
	if (AAlienHunterCharacter* Character = Cast<AAlienHunterCharacter>(MeshComp->GetOwner()))
	{
		Character->DeactivateMeleeBoxes();
	}
}
