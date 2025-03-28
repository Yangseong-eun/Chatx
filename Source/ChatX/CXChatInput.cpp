#include "CXChatInput.h"
#include "Components/EditableTextBox.h"
#include "CXPlayerController.h"

void UCXChatInput::NativeConstruct()
{
	Super::NativeConstruct();

	// 입력 확정 이벤트 바인딩
	if (EditableTextBox_ChatInput && !EditableTextBox_ChatInput->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatInputTextCommitted))
	{
		EditableTextBox_ChatInput->OnTextCommitted.AddDynamic(this, &ThisClass::OnChatInputTextCommitted);
	}
}

void UCXChatInput::NativeDestruct()
{
	Super::NativeDestruct();

	// 이벤트 바인딩 해제
	if (EditableTextBox_ChatInput && EditableTextBox_ChatInput->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatInputTextCommitted))
	{
		EditableTextBox_ChatInput->OnTextCommitted.RemoveDynamic(this, &ThisClass::OnChatInputTextCommitted);
	}
}

void UCXChatInput::OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	// 엔터 키로 입력 확정 시
	if (CommitMethod == ETextCommit::OnEnter)
	{
		APlayerController* PC = GetOwningPlayer();
		if (IsValid(PC))
		{
			ACXPlayerController* CXPC = Cast<ACXPlayerController>(PC);
			if (IsValid(CXPC))
			{
				// 입력된 텍스트를 PlayerController로 전달
				CXPC->SetChatMessageString(Text.ToString());

				// 입력창 비우기
				EditableTextBox_ChatInput->SetText(FText::GetEmpty());
			}
		}
	}
}
