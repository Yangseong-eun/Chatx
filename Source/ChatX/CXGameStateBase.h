// CXGameStateBase.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CXGameStateBase.generated.h"

/**
 * GameStateBase subclass - handles broadcasting game-wide messages
 */
UCLASS()
class CHATX_API ACXGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	// 로그인 시 메시지 브로드캐스트
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCBroadcastLoginMessage(const FString& InNameString = FString(TEXT("XXXXXXX")));
};
