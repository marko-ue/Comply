// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#define ECC_Enemy ECollisionChannel::ECC_GameTraceChannel2
#define ECC_Shield ECollisionChannel::ECC_GameTraceChannel3
#define ECC_Player ECollisionChannel::ECC_EngineTraceChannel4

/** Main log category used across the project */
DECLARE_LOG_CATEGORY_EXTERN(LogComply, Log, All);