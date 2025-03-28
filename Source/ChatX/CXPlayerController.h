// ACXPlayerController.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CXPlayerController.generated.h"

class UUserWidget;
class UCXChatInput;

/**
 * PlayerController for handling chat input and notification UI
 */
UCLASS()
class CHATX_API ACXPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ACXPlayerController();

	virtual void BeginPlay() override;

	// Set and broadcast chat message
	void SetChatMessageString(const FString& InChatMessageString);

	// Local print function
	void PrintChatMessageString(const FString& InChatMessageString);

	// Replication
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	// Server RPC: broadcast chat
	UFUNCTION(Server, Reliable)
	void ServerRPCPrintChatMessageString(const FString& InChatMessageString);

	// Client RPC: receive chat
	UFUNCTION(Client, Reliable)
	void ClientRPCPrintChatMessageString(const FString& InChatMessageString);

protected:
	// Chat input widget
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCXChatInput> ChatInputWidgetClass;

	UPROPERTY()
	TObjectPtr<UCXChatInput> ChatInputWidgetInstance;

	// Notification widget (chat log, etc.)
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> NotificationTextWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> NotificationTextWidgetInstance;

	// Local message
	FString ChatMessageString;

public:
	// Notification text replicated across clients
	UPROPERTY(Replicated, BlueprintReadOnly)
	FText NotificationText;
};
