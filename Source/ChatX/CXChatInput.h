// CXChatInput.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CXChatInput.generated.h"

class UEditableTextBox;

/**
 * 채팅 입력 위젯 클래스 (ChatInput Widget)
 */
UCLASS()
class CHATX_API UCXChatInput : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected:
	// 입력 완료 시 호출되는 함수
	UFUNCTION()
	void OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

public:
	// 에디터에서 바인딩된 채팅 입력 상자
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> EditableTextBox_ChatInput;
};
