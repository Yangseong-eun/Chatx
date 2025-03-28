// ACXGameModeBase.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CXGameModeBase.generated.h"

class ACXPlayerController;

/**
 * GameModeBase for ChatX - handles number judgment logic and round control
 */
UCLASS()
class CHATX_API ACXGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	// Called when a new player joins
	virtual void OnPostLogin(AController* NewPlayer) override;

	// Called once at game start
	virtual void BeginPlay() override;

	// Reset secret number and players
	void ResetGame();

	// 판단 후 승리 or 무승부 처리
	void JudgeGame(ACXPlayerController* InChattingPlayerController, int InStrikeCount);

	// 채팅 메시지 수신 → 스트라이크/볼 판단 포함
	void PrintChatMessageString(ACXPlayerController* InChattingPlayerController, const FString& InChatMessageString);

	// 플레이어의 시도 횟수 증가
	void IncreaseGuessCount(ACXPlayerController* InChattingPlayerController);

	// 정답 숫자 생성
	FString GenerateSecretNumber();

	// 입력 문자열이 유효한 3자리 추측 숫자인지 판단
	bool IsGuessNumberString(const FString& InNumberString);

	// 정답과 추측을 비교하여 결과 문자열 생성
	FString JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString);

protected:
	// 서버에서 유지되는 정답 숫자 문자열
	FString SecretNumberString;

	// 현재 접속 중인 모든 플레이어 컨트롤러
	TArray<TObjectPtr<ACXPlayerController>> AllPlayerControllers;
};
