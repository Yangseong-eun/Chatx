#include "CXGameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "CXPlayerController.h"

void ACXGameStateBase::MulticastRPCBroadcastLoginMessage_Implementation(const FString& InNameString)
{
	// 클라이언트에서만 실행되도록 (서버는 실행 안함)
	if (!HasAuthority())
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (IsValid(PC))
		{
			ACXPlayerController* CXPC = Cast<ACXPlayerController>(PC);
			if (IsValid(CXPC))
			{
				FString NotificationString = InNameString + TEXT(" has joined the game.");
				CXPC->PrintChatMessageString(NotificationString);
			}
		}
	}
}
