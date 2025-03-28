#include "CXPlayerController.h"
#include "CXChatInput.h"
#include "CXPlayerState.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Blueprint/UserWidget.h"
#include "Net/UnrealNetwork.h"
#include "EngineUtils.h"
#include "CXGameModeBase.h"
#include "Components/TextBlock.h"


ACXPlayerController::ACXPlayerController()
{
	bReplicates = true;
}

void ACXPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// UI 전용 입력 모드 설정
	FInputModeUIOnly InputModeUIOnly;
	SetInputMode(InputModeUIOnly);
	bShowMouseCursor = true;

	if (IsLocalController())
	{
		// 채팅 입력 위젯 생성 및 추가
		if (IsValid(ChatInputWidgetClass))
		{
			ChatInputWidgetInstance = CreateWidget<UCXChatInput>(this, ChatInputWidgetClass);
			if (IsValid(ChatInputWidgetInstance))
			{
				ChatInputWidgetInstance->AddToViewport();
			}
		}

		// 알림 텍스트 위젯 생성 및 추가
		if (IsValid(NotificationTextWidgetClass))
		{
			NotificationTextWidgetInstance = CreateWidget<UUserWidget>(this, NotificationTextWidgetClass);
			if (IsValid(NotificationTextWidgetInstance))
			{
				NotificationTextWidgetInstance->AddToViewport();
			}
		}
	}
}

void ACXPlayerController::SetChatMessageString(const FString& InChatMessageString)
{
	ChatMessageString = InChatMessageString;

	if (IsLocalController())
	{
		ACXPlayerState* CXPS = GetPlayerState<ACXPlayerState>();
		if (IsValid(CXPS))
		{
			FString CombinedMessage = CXPS->GetPlayerInfoString() + TEXT(": ") + InChatMessageString;

			// 서버에 메시지 전송
			ServerRPCPrintChatMessageString(CombinedMessage);
		}
	}
}

void ACXPlayerController::PrintChatMessageString(const FString& InChatMessageString)
{
	// 콘솔에도 출력
	UKismetSystemLibrary::PrintString(this, InChatMessageString, true, true, FLinearColor::Red, 5.0f);

	// 화면에 띄우기 (UMG 내 NotificationText 사용 예시)
	if (IsValid(NotificationTextWidgetInstance))
	{
		UTextBlock* TextBlock = Cast<UTextBlock>(NotificationTextWidgetInstance->GetWidgetFromName(TEXT("NotificationText")));
		if (IsValid(TextBlock))
		{
			TextBlock->SetText(FText::FromString(InChatMessageString));
		}
	}
}

void ACXPlayerController::ClientRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	PrintChatMessageString(InChatMessageString);
}

void ACXPlayerController::ServerRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	// 서버에서 모든 플레이어에게 메시지를 브로드캐스트
	for (TActorIterator<ACXPlayerController> It(GetWorld()); It; ++It)
	{
		ACXPlayerController* PC = *It;
		if (IsValid(PC))
		{
			PC->ClientRPCPrintChatMessageString(InChatMessageString);
		}
	}

	// 게임 로직에 전달 (예: 숫자 판정)
	ACXGameModeBase* GM = GetWorld()->GetAuthGameMode<ACXGameModeBase>();
	if (IsValid(GM))
	{
		GM->PrintChatMessageString(this, InChatMessageString);
	}
}

void ACXPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, NotificationText);
}
