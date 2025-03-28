#include "CXGameModeBase.h"
#include "CXGameStateBase.h"
#include "CXPlayerController.h"
#include "CXPlayerState.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Containers/Set.h"

void ACXGameModeBase::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	ACXPlayerController* CXPC = Cast<ACXPlayerController>(NewPlayer);
	if (IsValid(CXPC))
	{
		CXPC->NotificationText = FText::FromString(TEXT("Connected to the game server."));
		AllPlayerControllers.Add(CXPC);

		ACXPlayerState* CXPS = CXPC->GetPlayerState<ACXPlayerState>();
		if (IsValid(CXPS))
		{
			CXPS->PlayerNameString = FString::Printf(TEXT("Player%d"), AllPlayerControllers.Num());
		}

		ACXGameStateBase* CXGS = GetGameState<ACXGameStateBase>();
		if (IsValid(CXGS))
		{
			CXGS->MulticastRPCBroadcastLoginMessage(CXPS->PlayerNameString);
		}
	}
}

void ACXGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	SecretNumberString = GenerateSecretNumber();
}

void ACXGameModeBase::ResetGame()
{
	SecretNumberString = GenerateSecretNumber();

	for (ACXPlayerController* CXPC : AllPlayerControllers)
	{
		if (IsValid(CXPC))
		{
			ACXPlayerState* CXPS = CXPC->GetPlayerState<ACXPlayerState>();
			if (IsValid(CXPS))
			{
				CXPS->CurrentGuessCount = 0;
			}
		}
	}
}

void ACXGameModeBase::IncreaseGuessCount(ACXPlayerController* InChattingPlayerController)
{
	ACXPlayerState* CXPS = InChattingPlayerController->GetPlayerState<ACXPlayerState>();
	if (IsValid(CXPS))
	{
		CXPS->CurrentGuessCount++;
	}
}

void ACXGameModeBase::JudgeGame(ACXPlayerController* InChattingPlayerController, int InStrikeCount)
{
	if (InStrikeCount == 3)
	{
		ACXPlayerState* WinnerPS = InChattingPlayerController->GetPlayerState<ACXPlayerState>();
		if (IsValid(WinnerPS))
		{
			for (ACXPlayerController* CXPC : AllPlayerControllers)
			{
				FString WinMessage = WinnerPS->PlayerNameString + TEXT(" has won the game.");
				CXPC->NotificationText = FText::FromString(WinMessage);
			}
			ResetGame();
		}
	}
	else
	{
		bool bIsDraw = true;
		for (ACXPlayerController* CXPC : AllPlayerControllers)
		{
			ACXPlayerState* CXPS = CXPC->GetPlayerState<ACXPlayerState>();
			if (IsValid(CXPS) && CXPS->CurrentGuessCount < CXPS->MaxGuessCount)
			{
				bIsDraw = false;
				break;
			}
		}

		if (bIsDraw)
		{
			for (ACXPlayerController* CXPC : AllPlayerControllers)
			{
				CXPC->NotificationText = FText::FromString(TEXT("Draw..."));
			}
			ResetGame();
		}
	}
}

void ACXGameModeBase::PrintChatMessageString(ACXPlayerController* InChattingPlayerController, const FString& InChatMessageString)
{
	FString GuessNumberString;
	if (InChatMessageString.Split(TEXT(":"), nullptr, &GuessNumberString))
	{
		GuessNumberString = GuessNumberString.TrimStartAndEnd();

		if (IsGuessNumberString(GuessNumberString))
		{
			FString JudgeResultString = JudgeResult(SecretNumberString, GuessNumberString);
			IncreaseGuessCount(InChattingPlayerController);

			for (TActorIterator<ACXPlayerController> It(GetWorld()); It; ++It)
			{
				ACXPlayerController* CXPC = *It;
				if (IsValid(CXPC))
				{
					FString CombinedMessageString = InChatMessageString + TEXT(" -> ") + JudgeResultString;
					CXPC->ClientRPCPrintChatMessageString(CombinedMessageString);

					int32 StrikeCount = FCString::Atoi(*JudgeResultString.Left(1));
					JudgeGame(InChattingPlayerController, StrikeCount);
				}
			}
			return;
		}
	}

	// 유효하지 않은 숫자는 일반 채팅 처리
	for (TActorIterator<ACXPlayerController> It(GetWorld()); It; ++It)
	{
		ACXPlayerController* CXPC = *It;
		if (IsValid(CXPC))
		{
			CXPC->ClientRPCPrintChatMessageString(InChatMessageString);
		}
	}
}

FString ACXGameModeBase::GenerateSecretNumber()
{
	TArray<int32> Numbers;
	for (int32 i = 1; i <= 9; ++i)
	{
		Numbers.Add(i);
	}

	FMath::RandInit(FDateTime::Now().GetTicks());

	FString Result;
	for (int32 i = 0; i < 3; ++i)
	{
		int32 Index = FMath::RandRange(0, Numbers.Num() - 1);
		Result.Append(FString::FromInt(Numbers[Index]));
		Numbers.RemoveAt(Index);
	}

	return Result;
}

bool ACXGameModeBase::IsGuessNumberString(const FString& InNumberString)
{
	if (InNumberString.Len() != 3)
		return false;

	TSet<TCHAR> UniqueDigits;
	for (TCHAR C : InNumberString)
	{
		if (!FChar::IsDigit(C) || C == '0')
			return false;

		UniqueDigits.Add(C);
	}

	return UniqueDigits.Num() == 3;
}

FString ACXGameModeBase::JudgeResult(const FString& Secret, const FString& Guess)
{
	int32 Strike = 0;
	int32 Ball = 0;

	for (int32 i = 0; i < 3; ++i)
	{
		if (Secret[i] == Guess[i])
		{
			Strike++;
		}
		else if (Secret.Contains(FString::Printf(TEXT("%c"), Guess[i])))
		{
			Ball++;
		}
	}

	if (Strike == 0 && Ball == 0)
	{
		return TEXT("OUT");
	}

	return FString::Printf(TEXT("%dS%dB"), Strike, Ball);
}
