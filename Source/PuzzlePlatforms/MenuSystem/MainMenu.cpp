// Fill out your copyright notice in the Description page of Project Settings.

#include "MainMenu.h"

#include "UObject/ConstructorHelpers.h"

#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"

#include "ServerRow.h"


UMainMenu::UMainMenu(const FObjectInitializer & ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UUserWidget> ServerRowBPClass(TEXT("/Game/MenuSystem/WBP_ServerRow"));
	if (!ensure(ServerRowBPClass.Class != nullptr)) return;

	ServerRowClass = ServerRowBPClass.Class;
}


bool UMainMenu::Initialize(){
 

    bool Success = Super::Initialize();
    if (!Success) return false;
    
    if (!ensure(cmdHost != nullptr)) return false;
	cmdHost->OnClicked.AddDynamic(this, &UMainMenu::OpenHostMenu);


	
	if (!ensure(cmdCancelHostMenu != nullptr)) return false;
	cmdCancelHostMenu->OnClicked.AddDynamic(this, &UMainMenu::OpenMainMenu);

	if (!ensure(cmdConfirmHostMenu != nullptr)) return false;
	cmdConfirmHostMenu->OnClicked.AddDynamic(this, &UMainMenu::hostServer);

	if (!ensure(cmdJoin != nullptr)) return false;	
	cmdJoin->OnClicked.AddDynamic(this, &UMainMenu::OpenJoinMenu);

	if (!ensure(cmdCancelJoinMenuButton != nullptr)) return false;
	cmdCancelJoinMenuButton->OnClicked.AddDynamic(this, &UMainMenu::OpenMainMenu);

	if (!ensure(cmdConfirmJoinMenuButton != nullptr)) return false;
	cmdConfirmJoinMenuButton->OnClicked.AddDynamic(this, &UMainMenu::joinServer);

	if (!ensure(cmdQuitGame != nullptr)) return false;
	cmdQuitGame->OnClicked.AddDynamic(this, &UMainMenu::QuitPressed);


    return true;

	
}




void UMainMenu::hostServer(){

	

    if (MenuInterface != nullptr)
    {
       // MenuInterface->Host();
		FString ServerName = ServerHostName->Text.ToString();
		MenuInterface->Host(ServerName);

    }

}

void UMainMenu::SetServerList(TArray<FServerData> ServerNames)
{
	UWorld* World = this->GetWorld();
	if (!ensure(World != nullptr)) return;

	ServerList->ClearChildren();
	uint32 i = 0;
	for (const FServerData& ServerData : ServerNames)
	{
		UServerRow* Row = CreateWidget<UServerRow>(World, ServerRowClass);
		if (!ensure(Row != nullptr)) return;

		Row->ServerName->SetText(FText::FromString(ServerData.Name));
		Row->HostUser->SetText(FText::FromString(ServerData.HostUsername));
		FString FractionText = FString::Printf(TEXT("%d/%d"), ServerData.CurrentPlayers, ServerData.MaxPlayers);
		Row->ConnectionFraction->SetText(FText::FromString(FractionText));

		Row->Setup(this, i);
		++i;
		ServerList->AddChild(Row);
	}
}

void UMainMenu::SelectIndex(uint32 Index)
{
	SelectedIndex = Index;
	UpdateChildren();
}

void UMainMenu::UpdateChildren()
{
	for (int32 i = 0; i < ServerList->GetChildrenCount(); ++i)
	{
		auto Row = Cast<UServerRow>(ServerList->GetChildAt(i));
		if (Row != nullptr)
		{
			Row->Selected = (SelectedIndex.IsSet() && SelectedIndex.GetValue() == i);
		}
	}
}


void UMainMenu::OpenJoinMenu()
{
	if (!ensure(MenuSwitcher != nullptr)) return;
	if (!ensure(JoinMenu != nullptr)) return;
	MenuSwitcher->SetActiveWidget(JoinMenu);
	if (MenuInterface != nullptr) {
		MenuInterface->RefreshServerList();
	}
}


void UMainMenu::OpenMainMenu()
{
	if (!ensure(MenuSwitcher != nullptr)) return;
	if (!ensure(JoinMenu != nullptr)) return;
	MenuSwitcher->SetActiveWidget(MainMenu);
}



void UMainMenu::joinServer(){

	

	if (SelectedIndex.IsSet() && MenuInterface != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected index %d."), SelectedIndex.GetValue());
		MenuInterface->Join(SelectedIndex.GetValue());

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected index not set."));
	}
	

}


void UMainMenu::QuitPressed(){

	//UE_LOG(LogTemp, Warning, TEXT("I quit game!!"));

    UWorld* World = GetWorld();
    if (!ensure(World != nullptr)) return;

    APlayerController* PlayerController = World->GetFirstPlayerController();
    if (!ensure(PlayerController != nullptr)) return;

    PlayerController->ConsoleCommand("quit");


}

void UMainMenu::OpenHostMenu()
{
	MenuSwitcher->SetActiveWidget(HostMenu);

}

