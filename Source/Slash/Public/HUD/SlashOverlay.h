#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SlashOverlay.generated.h"

class UProgressBar;
class UTextBlock;

UCLASS()
class SLASH_API USlashOverlay : public UUserWidget
{
    GENERATED_BODY()

public:
	void SetHealthPercent(float Percent);
	void SetStaminaPercent(float Percent);
	void SetGold(int32 Amount);
	void SetSouls(int32 Amount);

private:
    UPROPERTY(meta = (BindWidget))
    UProgressBar* HealthProgressBar;

    UPROPERTY(meta = (BindWidget))
    UProgressBar* StaminaProgressBar;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* GoldText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* SoulsText;
};
