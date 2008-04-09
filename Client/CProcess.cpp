#include "CProcess.h"
#include "NetMessages.h"

void *ClassPointer;

/***************************************************************
 * Constructor
 *
 * @param game
 **************************************************************/
CProcess::CProcess(CGame *game) {
	this->p = game;
	ClassPointer = this;
}

/***************************************************************
 * Destructor
 *
 **************************************************************/
CProcess::~CProcess() {
}

/***************************************************************
 * Function:	ProcessData
 *
 * @param TheData
 **************************************************************/
int CProcess::ProcessData(char *TheData) {

	// Switch on the type of packet
	switch((unsigned char)TheData[0]) {

		// Packet: smLoginCorrect
		case smLoginCorrect:
			this->ProcessLogin((unsigned char)TheData[1], (unsigned char)TheData[2]);
			break;

		// Packet: smAppendNews
		case smAppendNews:
			if (this->p->State == STATE_GAME)
				this->p->Admin->SetNews(&TheData[1]);
			else
				this->p->Meeting->AppendNews(&TheData[1]);
			break;

		// Packet: smChatMessage
		case smChatMessage:
			this->ProcessChatMessage(TheData, 0);
			break;

		// Packet: smChatCommand
		case smChatCommand:
			this->ProcessChatCommand(TheData[1], TheData[2]);
			break;
		
		// Packet: smEditAccount
		case smEditAccount:
			this->ProcessEditAccount(&TheData[1]);
			break;

		// Packet: smPlayerData
		case smPlayerData:
			this->ProcessPlayerData((sSMPlayer *)&TheData[1]);
			break;

		// Packet: smMayorHire
		case smMayorHire:
			this->ProcessMayorHire(TheData[1]);
			break;

		// Packet: smAddRemCity
		case smAddRemCity:
			this->p->Meeting->AddCity((unsigned char)TheData[1],(unsigned char)TheData[2], (unsigned char)TheData[3]);
			break;

		// Packet: smStateGame
		case smStateGame:
			this->ProcessEnterGame((sSMStateGame *)&TheData[1]);
			break;

		// Packet: smJoinData
		case smJoinData:
			this->ProcessJoinData((sSMJoinData *)&TheData[1]);
			break;

		// Packet: smError
		case smError:
			this->ProcessError(TheData[1]);
			break;

		// Packet: smNewBuilding
		case smNewBuilding:
			this->ProcessNewBuilding((sSMBuild *)&TheData[1],1);
			break;

		// Packet: smPlaceBuilding
		case smPlaceBuilding:
			this->ProcessNewBuilding((sSMBuild *)&TheData[1],0);
			break;

		// Packet: smUpdate
		case smUpdate:	
			this->ProcessUpdate((sSMUpdate *)&TheData[1]);
			break;

		// Packet: smCanBuild
		case smCanBuild:
			this->ProcessCanBuild(TheData);
			break;

		// Packet: smFinance
		case smFinance:
			this->ProcessFinance((sSMFinance *)&TheData[1]);
			break;

		// Packet: smInterview
		case smInterview:
			this->ProcessInterview();
			break;

		// Packet: smComms
		case smComms:
			this->ProcessComms(TheData);
			break;

		// Packet: smMayorDeclined
		case smMayorDeclined:
			this->ProcessRejected();
			break;

		// Packet: smMayorInInterview
		case smMayorInInterview:
			this->ProcessMayorInInterview();
			break;

		// Packet: smInterviewCancel
		case smInterviewCancel:
			this->ProcessInterviewCancel();
			break;

		// Packet: smAddItem
		case smAddItem:
			this->ProcessAddItem((sSMItem *) &TheData[1]);
			break;

		// Packet: smRemItem
		case smRemItem:
			this->ProcessRemoveItem((sSMItem *) &TheData[1]);
			break;

		// Packet: smItemCount
		case smItemCount:
			this->ProcessItemCount((sSMItemCount *)&TheData[1]);
			break;

		// Packet: smPickedUp
		case smPickedUp:
			this->ProcessPickUp((sCMItem *) &TheData[1]);
			break;

		// Packet: smDropped
		case smDropped:
			this->ProcessDropped((sCMItem *) &TheData[1]);
			break;

		// Packet: smUpdatePop
		case smUpdatePop:
			this->ProcessPop((sSMPop *)&TheData[1]);
			break;

		// Packet: smDeath
		case smDeath:
			this->ProcessDeath(TheData[1], TheData[2], TheData[3]);
			break;

		// Packet: smDestroyCity
		case smDestroyCity:
			this->ProcessDestroyCity((unsigned char)TheData[1]);
			break;

		// Packet: smRemBuilding
		case smRemBuilding:
			this->ProcessRemoveBuilding((sSMBuild *)&TheData[1]);
			break;

		// Packet: smUnderAttack
		case smUnderAttack:
			this->ProcessUnderAttack();
			break;

		// Packet: smOrbed
		case smOrbed:
			this->ProcessOrbed((sSMOrbedCity *)&TheData[1]);
			break;

		// Packet: smGlobal
		case smGlobal:
			this->ProcessGlobal(TheData);
			break;

		// Packet: smShoot
		case smShoot:
			this->ProcessShot((sSMShot *)&TheData[1]);
			break;

		// Packet: smTCPPong
		case smTCPPong:
			this->ProcessPong();
			break;

		// Packet: smPointsUpdate
		case smPointsUpdate:
			this->ProcessPointsUpdate((sSMPoints *)&TheData[1]);
			break;

		// Packet: smMedKit
		case smMedKit:
			this->ProcessMedKit();
			break;

		// Packet: smCloak
		case smCloak:
			this->ProcessCloak(TheData[1]);
			break;

		// Packet: smExplosion
		case smExplosion:
			this->ProcessExplosion((sSMExplode *)&TheData[1]);
			break;

		// Packet: smWarp
		case smWarp:
			this->ProcessWarp((sSMStateGame *)&TheData[1]);
			break;

		// Packet: smNextStep
		case smNextStep:
			if (TheData[1] == 65) {
				this->p->Dialog->StartDialog = 0;
				if (this->p->Meeting->hWnd) {
					EndDialog(this->p->Meeting->hWnd, 1);
				}
				if (this->p->Interview->hWnd) {
					EndDialog(this->p->Interview->hWnd, 1);
				}
				this->p->State = STATE_JOINING;
			}
			if (TheData[1] == 66) {
				this->p->InGame->ClearAllSectors();
				this->p->InGame->RefreshArea();
			}
			if (TheData[1] == 67) {
				this->p->State = STATE_GAME;
			}
			this->p->Winsock->SendData(cmNextStep, &TheData[1], 1);
			break;
		
		// Packet: smMayorUpdate
		case smMayorUpdate:
			this->ProcessMayorUpdate((sSMMayorUpdate *)&TheData[1]);
			break;

		// Packet: smItemLife
		case smItemLife:
			this->ProcessItemLife((sSMItemLife *)&TheData[1]);
			break;

		// Packet: smFired
		case smFired:
			this->ProcessFired();
			break;

		// Packet: smAdmin
		case smAdmin:
			this->ProcessAdmin((sSMAdmin *)&TheData[1]);
			break;

		// Packet: smKicked
		case smKicked:
			this->ProcessKicked();
			break;

		// Packet: smSectorSent
		case smSectorSent:
			this->ProcessSectorSent((sCMSector *)&TheData[1]);
			break;

		// Packet: smInfoButton
		case smInfoButton:
			this->ProcessInfoButton((sSMInfoButton *)&TheData[1]);
			break;

		// Packet: smClearPlayer
		case smClearPlayer:
			this->ProcessClearPlayer(TheData[1]);
			break;

		// Packet: smMayorChanged
		case smMayorChanged:
			this->ProcessMayorChanged();
			break;

		// Packet: smRightClickCity
		case smRightClickCity:
			this->ProcessRightClickCity((sSMRightClickCity *)&TheData[1]);
			break;

		// Packet: smBan
		case smBan:
			this->p->Admin->AddBan((sSMBan *)&TheData[1]);
			break;

		// Packet: smRespawn
		case smRespawn:
			this->ProcessRespawn(TheData[1]);
			break;

		// Packet: smAdminEdit
		case smAdminEdit:
			this->ProcessAdminEdit((sCMAdminEdit *)&TheData[1]);
			break;

		// Packet: smClickPlayer
		case smClickPlayer:
			this->ProcessClickPlayer((sSMClickPlayer *)&TheData[1]);
			break;

		// Packet: smWhisper
		case smWhisper:
			this->ProcessWhisper(TheData);
			break;

		// Packet: unknown!
		default:
			std::ostringstream thing;
			int i = TheData[0];
			thing << i;
			this->p->Engine->logerror("Unhandled Data Packet " + thing.str());
			return 1;
			break;
	}

	return 0;
}

/***************************************************************
 * Function:	ProcessLogin
 *
 * @param Index
 * @param playerType
 **************************************************************/
void CProcess::ProcessLogin(unsigned char Index, unsigned char playerType) {
	this->p->Player[Index]->ClearPlayer();
	this->p->Winsock->MyIndex = Index;
	this->p->Player[Index]->playerType = playerType;
	this->p->Player[Index]->Name.clear();
	this->p->Player[Index]->Name = this->p->Login->user;
	this->p->Meeting->ShowMeetingDlg();
}

/***************************************************************
 * Function:	ProcessClearPlayer
 *
 * @param Index
 **************************************************************/
void CProcess::ProcessClearPlayer(unsigned char Index) {
	this->p->Player[Index]->ClearPlayer();
	if (this->p->Admin->IsOpen) {
		this->p->Admin->DrawPlayerList();
	}
}

/***************************************************************
 * Function:	ProcessError
 *
 * @param error
 **************************************************************/
void CProcess::ProcessError(int error) {
	
	switch(error) {
		case 65: //Message A
			{MessageBox(this->p->hWnd, "Account created succesfully!", "BattleCity", 0); this->p->Login->ShowLoginDlg(); break;}
		case 66: //Message B
			{MessageBox(this->p->hWnd, "Incorrect Password", "BattleCity", 0); this->p->Login->ShowLoginDlg(); break;}
		case 67: //Message C
			{MessageBox(this->p->hWnd, "Account does not exist", "BattleCity", 0); this->p->Login->ShowLoginDlg(); break;}
		case 68: //Message D
			{MessageBox(this->p->hWnd, "User name is already in use!", "BattleCity", 0); this->p->NewAccount->ShowNewDlg(); break;}
		case 69: //Message E
			{MessageBox(this->p->hWnd, "Account is currently in use", "BattleCity", 0); this->p->Login->ShowLoginDlg(); break;}
		case 70: //Message F
			{MessageBox(this->p->hWnd, "Please download the latest version of BattleCity at http://battlecity.looble.com", 0, 0); SendMessage(this->p->hWnd, WM_CLOSE, 0, 0); break;}
		case 71: //Message G
			{MessageBox(this->p->hWnd, "Banned", 0, 0); SendMessage(this->p->hWnd, WM_CLOSE, 0, 0); break;}
		case 72: //Message H
			{MessageBox(this->p->hWnd, "Only one client per computer", 0, 0); SendMessage(this->p->hWnd, WM_CLOSE, 0, 0); break;}
		case 74: //Message J
			{MessageBox(this->p->hWnd, "Invalid verification code.", 0, 0); SendMessage(this->p->hWnd, WM_CLOSE, 0, 0); break;}
		case 75: //Message K
			if (this->p->NewAccount->Editing == 1)
				{MessageBox(this->p->hWnd, "Email is already in use!", "BattleCity", 0); this->p->NewAccount->ShowEditDlg();}
				else {MessageBox(this->p->hWnd, "Email is already in use!", "BattleCity", 0); this->p->NewAccount->ShowNewDlg();}
			break;
		case 76: //Message L
			{MessageBox(this->p->hWnd, "Email address not found.  Please try again!", 0, 0); this->p->Recover->ShowRecoverDlg(); break;}
		case 77: //Message M
			{MessageBox(this->p->hWnd, "Your account information has been sent to your email address!", 0, 0); this->p->Login->ShowLoginDlg(); break;}
	}
}

/***************************************************************
 * Function:	ProcessEvent
 *
 * @param Event
 **************************************************************/
void CProcess::ProcessEvent(int Event) {

	switch (Event) {
		case 1: //Error connecting
			this->p->running = 0;
			Sleep(100);
			MessageBox(this->p->hWnd, "Unable to connect to the BattleCity server.  Make sure you are properly connected to the internet, and if you are, the server is likely down for maintenance or being updated.  Please try again later.", "BattleCity", 0);
			SendMessage(this->p->hWnd, WM_CLOSE, 0, 0);
			break;
		case 2: //Connected
			this->p->Login->ShowLoginDlg();
			this->p->Send->SendVersion();
			break;
		case 3: //Disconnected
			if (this->p->running == 0) return;
			this->p->running = 0;
			Sleep(100);
 			MessageBox(this->p->hWnd, "You have been disconnected from the BattleCity server.  Please try again later.", "BattleCity", 0);
			SendMessage(this->p->hWnd, WM_CLOSE, 0, 0);
			break;
	}
}

/***************************************************************
 * Function:	ProcessChatMessage
 *
 * @param TheData
 * @param global
 **************************************************************/
void CProcess::ProcessChatMessage(char *TheData, char global) {
	string tmpstring;
	int Index = (unsigned char)TheData[1];
	tmpstring = this->p->Player[Index]->Name + ": " + &TheData[2];
	switch (this->p->State) {
		case STATE_GAME:
			if (this->p->Player[Index]->isAdmin()) {
				this->p->InGame->AppendChat(tmpstring, COLOR_ADMIN_CHAT);
			}
			else {

				if (this->p->Player[Index]->isDead == true) {
					this->p->InGame->AppendChat(tmpstring, COLOR_DEAD_CHAT);
				}
				else {
					if (this->p->Player[Index]->City == this->p->Player[this->p->Winsock->MyIndex]->City) {
						this->p->InGame->AppendChat(tmpstring, COLOR_TEAM_CHAT);
					}
					else {
						this->p->InGame->AppendChat(tmpstring, COLOR_ENEMY_CHAT);
					}
				}
			}
			break;
		case STATE_MEETING:
			this->p->Meeting->AppendData(tmpstring);
			break;
		default:

			break;
	}
}

/***************************************************************
 * Function:	ProcessChatCommand
 *
 * @param Index
 * @param message
 **************************************************************/
void CProcess::ProcessChatCommand(int Index, int message) {
	string tmpString;
	if (this->p->Player[Index]->Name.length() > 0) {
		tmpString = this->p->Player[Index]->Name;
		switch(message) {
			case 65: //Message A
				{this->p->Meeting->AddPlayer(Index); break;}
			case 66: //Message B
			case 67: //Message C 
			case 68: //Message D
				{this->p->Meeting->ClearPlayer(Index); break;}
			case 69: //Message E
				{tmpString.append(" has left"); this->p->Player[Index]->InGameClear(); if (this->p->Admin->IsOpen) this->p->Admin->DrawPlayerList(); break;}
		}
		switch(this->p->State) {
			case STATE_GAME:
				this->p->InGame->AppendChat(&tmpString[0], COLOR_SYSTEM);
				break;
			default:
				break;
		}
	}
}

/***************************************************************
 * Function:	ProcessEnterGame
 *
 * @param game
 **************************************************************/
void CProcess::ProcessEnterGame(sSMStateGame *game) {
	int me = this->p->Winsock->MyIndex;

	this->p->Draw->resetPlayerOffset();
	this->p->Player[me]->X = (float)game->x;
	this->p->Player[me]->Y = (float)game->y;
	this->p->Player[me]->SectorX = (game->x/48) / SectorSize;
	this->p->Player[me]->SectorY = (game->y/48) / SectorSize;
	this->p->Player[me]->CityX = game->x;
	this->p->Player[me]->CityY = game->y;
	this->p->Player[me]->City = game->City;
	this->p->Player[me]->SetHP(40);
	this->p->Player[me]->isInGame = 1;
	this->p->Player[me]->GenerateNameString();

	if (this->p->Player[me]->isMayor) {
		this->p->InGame->NewbieTip = "You are Mayor of ";
		this->p->InGame->NewbieTip += CityList[this->p->Player[this->p->Winsock->MyIndex]->City];
		this->p->InGame->NewbieTip += ".  Right-click anywhere on the screen to start building!";
	}
	else {
		this->p->InGame->NewbieTip = "You have joined ";
		this->p->InGame->NewbieTip += CityList[this->p->Player[this->p->Winsock->MyIndex]->City];
		this->p->InGame->NewbieTip += ".  Press 'u' on top of an item to pick it up, and 'd' to drop it!";
	}

	this->p->InGame->AppendChat("Full instructions at:", COLOR_SYSTEM);
	this->p->InGame->AppendChat("http://battlecity.looble.com/", COLOR_SYSTEM);
	this->p->InGame->PrintWhoData();
	this->p->InGame->AppendChat(this->p->Player[me]->Name + " has just joined!", COLOR_SYSTEM);

	this->p->Dialog->StartDialog = 0;
	
	if (this->p->Meeting->hWnd) {
		EndDialog(this->p->Meeting->hWnd, 1);
	}
	if (this->p->Interview->hWnd) {
		EndDialog(this->p->Interview->hWnd, 1);
	}
}

/***************************************************************
 * Function:	ProcessNewBuilding
 *
 * @param data
 * @param sound
 **************************************************************/
void CProcess::ProcessNewBuilding(sSMBuild *data, int sound) {
	this->p->Build->newBuilding(data->x,data->y,data->City,buildingTypes[(unsigned char)data->type - 1], data->id);
	CBuilding *bld = this->p->Build->findBuilding(data->id);
	if (bld) {
		bld->pop = data->pop;
	}

	if (sound && this->p->State == STATE_GAME) {
		int me = this->p->Winsock->MyIndex;
		if (data->x * 48 < this->p->Player[me]->X + 1000 &&
			data->x * 48 + 1000 > this->p->Player[me]->X &&
			data->y * 48 < this->p->Player[me]->Y + 1000 &&
			data->y * 48 + 1000 > this->p->Player[me]->Y) {
				this->p->Sound->Play3dSound(this->p->Sound->s_build, 100, (float)data->x*48, (float)data->y*48);
		}
	}
}

/***************************************************************
 * Function:	ProcessUpdate
 *
 * @param data
 **************************************************************/
void CProcess::ProcessUpdate(sSMUpdate *data) {
	this->p->Player[data->id]->isDead = false;
	this->p->Player[data->id]->Y = data->y;
	this->p->Player[data->id]->X = data->x;
	this->p->Player[data->id]->isMoving = data->move - 1;
	this->p->Player[data->id]->isTurning = data->turn - 1;
	this->p->Player[data->id]->Direction = data->dir;
	this->p->Player[data->id]->lastUpdate = this->p->Tick;
}

/***************************************************************
 * Function:	ProcessPlayerData
 *
 * @param player
 **************************************************************/
void CProcess::ProcessPlayerData(sSMPlayer *player) {
	int i = player->Index;
	this->p->Player[i]->playerType = player->playerType;
	this->p->Player[i]->Red = player->Red;
	this->p->Player[i]->Green = player->Green;
	this->p->Player[i]->Blue = player->Blue;
	this->p->Player[i]->Member = player->Member;
	this->p->Player[i]->Tank = player->Tank;

	if (player->Red == 255 && player->Green == 255 && player->Blue == 255) {
		this->p->Player[i]->RainbowName = true;
	}
	
	this->p->Player[i]->Name = player->Name;
	this->p->Player[i]->Town = player->Town;
	this->p->Player[i]->GenerateNameString();
}

/***************************************************************
 * Function:	ProcessCanBuild
 *
 * @param TheData
 **************************************************************/
void CProcess::ProcessCanBuild(char TheData[255]) {
	if (TheData[2]) {
		this->p->InGame->CanBuild[(unsigned char)TheData[1]-1] = 1;
	}
	else {
		this->p->InGame->CanBuild[(unsigned char)TheData[1]-1] = 0;
	}
}

/***************************************************************
 * Function:	ProcessComms
 *
 * @param TheData
 **************************************************************/
void CProcess::ProcessComms(char TheData[255]) {
	string tmpString;
	if (this->p->Player[(unsigned char)TheData[1]]->Name.length() > 0) {
		switch (TheData[2]) {
			case 124: // |
				tmpString = "The mayor has arrived - Start talking.";
				break;
			default:
				tmpString = this->p->Player[(unsigned char)TheData[1]]->Name + ": ";
				tmpString.append(&TheData[2]);
				break;
		}
		if (this->p->State == STATE_GAME) {
			this->p->Personnel->AppendData(tmpString);
		}
		else  {
			this->p->Interview->AppendData(tmpString);
		}
	}
}

/***************************************************************
 * Function:	ProcessRejected
 *
 **************************************************************/
void CProcess::ProcessRejected() {
	MessageBox(this->p->hWnd, "The mayor did not hire you.  Try a different city.", "BattleCity", 0);
	this->p->Interview->Chat.clear();
	this->p->Meeting->ShowMeetingDlg();
	EndDialog(this->p->Interview->hWnd, 1);
}

/***************************************************************
 * Function:	ProcessInterview
 *
 **************************************************************/
void CProcess::ProcessInterview() {
	this->p->Interview->Chat.clear();
	this->p->Interview->ShowInterviewDlg();
	this->p->Interview->AppendData("Please wait here while I notify the Mayor of your arrival");
}

/***************************************************************
 * Function:	ProcessMayorHire
 *
 * @param Index
 **************************************************************/
void CProcess::ProcessMayorHire(int Index) {
	string tmpString;
	tmpString.clear();

	if (this->p->Personnel->DenyApplicants == 0)  {

		if (this->p->Player[Index]->Name.length() > NULL) {
			tmpString = this->p->InGame->ReturnRank(this->p->Player[Index]->Points) + " ";
			tmpString += this->p->Player[Index]->Name;
			tmpString += " of ";
			tmpString += this->p->Player[Index]->Town;
			tmpString += + " is looking for a job";
			this->p->Personnel->ApplicantWaiting = 1;
			this->p->InGame->AppendChat("Note:  Another player is applying for a position in your city.", COLOR_SYSTEM);
			this->p->Personnel->AppendData(tmpString);
			this->p->Sound->PlayWav(sBuzz,-1);
		}
	}

	else {
		char packet[2];
		packet[0] = (char)this->p->Personnel->DenyApplicants;
		packet[1] = 0;
		this->p->Winsock->SendData(cmIsHiring, packet, 1);
		this->p->Winsock->SendData(cmHireDecline, " ", 1);
	}
}

/***************************************************************
 * Function:	ProcessAddItem
 *
 * @param item
 **************************************************************/
void CProcess::ProcessAddItem(sSMItem *item) {
	this->p->Item->newItem(item->x, item->y, item->type, item->City, item->id, item->active);
} 

/***************************************************************
 * Function:	ProcessRemoveItem
 *
 * @param item
 **************************************************************/
void CProcess::ProcessRemoveItem(sSMItem *item) {
	CItem *item2;
	item2 = this->p->Item->findItem(item->id);
	
	if (item2) {
		this->p->Item->delItem(this->p->Item->findItem(item->id));
	}
	else {
		item2 = this->p->Inventory->findItem(item->id);
		if (item2) {
			this->p->Inventory->delItem(this->p->Inventory->findItem(item->id));
			this->p->Inventory->ItemCheck();
		}
	}
}

/***************************************************************
 * Function:	ProcessItemCount
 *
 * @param count
 **************************************************************/
void CProcess::ProcessItemCount(sSMItemCount *count) {
	CBuilding *bld = this->p->Build->findBuilding(count->id);

	if (bld) bld->ItemsLeft = count->count;
	return;
}

/***************************************************************
 * Function:	ProcessJoinData
 *
 * @param join
 **************************************************************/
void CProcess::ProcessJoinData(sSMJoinData *join) {
	string tmpString;
	int i = join->id;
	this->p->Player[i]->isMayor = join->Mayor;
	this->p->Player[i]->City = join->City;
	this->p->Player[i]->isInGame = 1;
	this->p->Player[i]->X = 0;
	this->p->Player[i]->Y = 0;
	this->p->Player[i]->GenerateNameString();

	if (this->p->Player[i]->Name.length() > 0 && this->p->State == STATE_GAME) {
		tmpString = this->p->Player[i]->Name + " has just joined";
		this->p->InGame->AppendChat(tmpString, COLOR_SYSTEM);
	}
	if (this->p->Admin->IsOpen) {
		this->p->Admin->DrawPlayerList();
	}
}

/***************************************************************
 * Function:	ProcessPickUp
 *
 * @param item
 **************************************************************/
void CProcess::ProcessPickUp(sCMItem *item) {
	CItem *item2;
	item2 = this->p->Item->findItem(item->id);

	if (item2) {
		CItem *itm = this->p->Inventory->newItem(item2->X, item2->Y, item2->Type, item2->City, item->id, item->active);
		this->p->Inventory->ItemCheck();
		this->p->Inventory->SelectedItemType = itm->Type;

		if (this->p->Options->newbietips) {
			switch (item2->Type) {

				case ITEM_TYPE_CLOAK:
					this->p->InGame->NewbieTip = "Cloaks remove you from enemy radar.  Press 'c' to use a Cloaking Device.";
					break;

				case ITEM_TYPE_ROCKET:
					this->p->InGame->NewbieTip = "Cougar Missles upgrade your tank's weapon, but only fire when your tank is not moving.";
					break;

				case ITEM_TYPE_MEDKIT:
					this->p->InGame->NewbieTip = "Medkits restore your tank to full health. Press 'h' to use a Medkit.";
					break;

				case ITEM_TYPE_BOMB:
					this->p->InGame->NewbieTip = "Bombs destroy nearby enemy buildings. Press 'b' to drop an activated bomb.";
					break;

				case ITEM_TYPE_MINE:
					this->p->InGame->NewbieTip = "Mines detonate enemy tanks, and cannot be seen by the enemy.";
					break;

				case ITEM_TYPE_ORB:
					this->p->InGame->NewbieTip = "The Orb destroys an enemy city when dropped on the opponent's 'Command Center.'";
					break;

				case ITEM_TYPE_WALKIE:
					this->p->InGame->NewbieTip = "Walkies allow teammates to communicate outside of radar range.";
					break;

				case ITEM_TYPE_DFG:
					this->p->InGame->NewbieTip = "DFGs demobilize enemy tanks, and cannot be seen by the enemy.";
					break;

				case ITEM_TYPE_WALL:
					this->p->InGame->NewbieTip = "Walls serve as obstacles, and are often placed on the 'No Parking' area of a city's Command Center.";
					break;

				case ITEM_TYPE_TURRET:
					this->p->InGame->NewbieTip = "Turrets automatically fire at nearby enemy tanks.";
					break;

				case ITEM_TYPE_SLEEPER:
					this->p->InGame->NewbieTip = "Sleeper Turrets automatically fire at nearby enemy tanks, and cannot be seen by the enemy.";
					break;

				case ITEM_TYPE_PLASMA:
					this->p->InGame->NewbieTip = "Plasma Cannons automatically fire at enemy tanks, and do more damage than regular Turrets.";
					break;
			}
		}
	}
}

/***************************************************************
 * Function:	ProcessDropped
 *
 * @param item
 **************************************************************/
void CProcess::ProcessDropped(sCMItem *item) {
	CItem *item2;
	item2 = this->p->Inventory->findItem(item->id);
	
	if (item2) {
		this->p->Inventory->delItem(item2);
		this->p->Inventory->ItemCheck();
	}
}

/***************************************************************
 * Function:	ProcessPop
 *
 * @param pop
 **************************************************************/
void CProcess::ProcessPop(sSMPop *pop) {
	CBuilding *bld = this->p->Build->findBuilding(pop->id);
	if (bld) {
		bld->pop = pop->pop;
	}
}

/***************************************************************
 * Function:	ProcessDeath
 *
 * @param Index
 * @param deathType
 * @param City
 **************************************************************/
void CProcess::ProcessDeath(int Index, char deathType, char City) {
	string tmpString;
    srand((unsigned int)this->p->Tick);
    int random_integer = rand()%13;
	int me = this->p->Winsock->MyIndex;

	this->p->Player[Index]->isDead = true;
	this->p->Player[Index]->isMoving = false;
	this->p->Player[Index]->isTurning = false;

	if (Index != this->p->Winsock->MyIndex) {

		if ((abs(this->p->Player[Index]->X - this->p->Player[me]->X) < 1000) && (abs(this->p->Player[Index]->Y - this->p->Player[me]->Y) < 1000)) {

			this->p->Explode->newExplosion((int)this->p->Player[Index]->X, (int)this->p->Player[Index]->Y, 1);
			this->p->Sound->Play3dSound(this->p->Sound->s_eXplode, 100, this->p->Player[Index]->X, this->p->Player[Index]->Y);
			this->p->Sound->Play3dSound(this->p->Sound->s_die, 100, this->p->Player[Index]->X, this->p->Player[Index]->Y);
		}
	}
	else {
		this->p->Sound->StopWav(12);
	}

	switch (random_integer) {
		case 0:
			tmpString = this->p->Player[Index]->Name + " no longer exists as a single entity";
			break;
		case 1:
			tmpString = this->p->Player[Index]->Name + " just had a TNT experience";
			break;
		case 2:
			tmpString = this->p->Player[Index]->Name + " has shattered into many pieces";
			break;
		case 3:
			tmpString = this->p->Player[Index]->Name + " has been blown up";
			break;
		case 4:
			tmpString = this->p->Player[Index]->Name + " is no more";
			break;
		case 5:
			tmpString = this->p->Player[Index]->Name + " is worm food";
			break;
		case 6:
			tmpString = this->p->Player[Index]->Name + " just ran out of luck";
			break;
		case 7:
			tmpString = "Alas poor " + this->p->Player[Index]->Name + " we knew him well";
			break;
		case 8:
			tmpString = "A funeral service will shortly be held for " + this->p->Player[Index]->Name;
			break;
		case 9:
			tmpString = this->p->Player[Index]->Name + " has bitten the dust";
			break;
		case 10:
			tmpString = this->p->Player[Index]->Name + " has gone to meet his maker";
			break;
		case 11:
			tmpString = this->p->Player[Index]->Name + " is dead meat";
			break;
		case 12:
			tmpString = this->p->Player[Index]->Name + " was useless at this game anyway!";
			break;
	}

	if (this->p->Player[this->p->Winsock->MyIndex]->City == City) {
		tmpString += " (2)";
	}

	this->p->InGame->AppendChat(tmpString, COLOR_SYSTEM);
}

/***************************************************************
 * Function:	ProcessShot
 *
 * @param shotsfired
 **************************************************************/
void CProcess::ProcessShot(sSMShot *shotsfired) {
	int me = this->p->Winsock->MyIndex;
	float fDir;
	int FlashY;
	int FlashX; 

	if (this->p->Player[shotsfired->id]->isAdmin()) {
		fDir = (float)-this->p->Player[shotsfired->id]->Direction+32;
		FlashY = (int)this->p->Player[shotsfired->id]->Y-24+10 + (int)(cos((float)(fDir)/16*3.14)*20);
		FlashX = (int)this->p->Player[shotsfired->id]->X-24+6 + (int)(sin((float)(fDir)/16*3.14)*20);
		this->p->Explode->newExplosion(FlashX, FlashY, 3);
		this->p->Bullet->newBullet(shotsfired->x, shotsfired->y, shotsfired->type, shotsfired->dir, shotsfired->id);

		if ((abs(this->p->Player[shotsfired->id]->X - this->p->Player[me]->X) < 1000) && (abs(this->p->Player[shotsfired->id]->Y - this->p->Player[me]->Y) < 1000)) {
			this->p->Sound->Play3dSound(this->p->Sound->s_turret, 100, this->p->Player[shotsfired->id]->X, this->p->Player[shotsfired->id]->Y);
		}
	}

	else {
		fDir = (float)-this->p->Player[shotsfired->id]->Direction+32;
		FlashY = (int)this->p->Player[shotsfired->id]->Y-24+10 + (int)(cos((float)(fDir)/16*3.14)*20);
		FlashX = (int)this->p->Player[shotsfired->id]->X-24+6 + (int)(sin((float)(fDir)/16*3.14)*20);
		this->p->Explode->newExplosion(FlashX, FlashY, 3);
		this->p->Bullet->newBullet(shotsfired->x, shotsfired->y, shotsfired->type, shotsfired->dir, shotsfired->id);

		if ((abs(this->p->Player[shotsfired->id]->X - this->p->Player[me]->X) < 1000) && (abs(this->p->Player[shotsfired->id]->Y - this->p->Player[me]->Y) < 1000)) {

			switch(shotsfired->type) {

				case 0:
					this->p->Sound->Play3dSound(this->p->Sound->s_tanklaser, 100, this->p->Player[shotsfired->id]->X, this->p->Player[shotsfired->id]->Y);
					break;
				case 1:
					this->p->Sound->Play3dSound(this->p->Sound->s_bigturret, 100, this->p->Player[shotsfired->id]->X, this->p->Player[shotsfired->id]->Y);
					break;
			}
		}
	}
}

/***************************************************************
 * Function:	ProcessDestroyCity
 *
 * @param City
 **************************************************************/
void CProcess::ProcessDestroyCity(char City) {
	this->p->Item->deleteItemsByCity(City);
	this->p->Build->deleteBuildingsByCity(City);
}

/***************************************************************
 * Function:	ProcessRemoveBuilding
 *
 * @param build
 **************************************************************/
void CProcess::ProcessRemoveBuilding(sSMBuild *build) {
	CBuilding *bld = this->p->Build->findBuilding(build->id);
	int me = this->p->Winsock->MyIndex;

	// If the building was found,
	if (bld) {

		// ???
		if (build->x == 1) {
			this->p->Explode->newExplosion(bld->X * 48, bld->Y * 48, 2);
		}

		// If the building is within sound range,
		if (bld->X * 48 < this->p->Player[this->p->Winsock->MyIndex]->X + 400 &&
			bld->X * 48 + 400 > this->p->Player[this->p->Winsock->MyIndex]->X &&
			bld->Y * 48 < this->p->Player[this->p->Winsock->MyIndex]->Y + 400 &&
			bld->Y * 48 + 400 > this->p->Player[this->p->Winsock->MyIndex]->Y) {

				// Play the Demolish sound
				this->p->Sound->Play3dSound(this->p->Sound->s_demolish, 100, (float)bld->X*48, (float)bld->Y*48);
		}
		
		// If the building is from this player's city,
		if (bld->City == this->p->Player[me]->City) {
			this->p->InGame->setIsUnderAttack(true);
		}

		// Delete the building
		this->p->Build->delBuilding(bld);
	}
}

/***************************************************************
 * Function:	ProcessOrbed
 *
 * @param orbed
 **************************************************************/
void CProcess::ProcessOrbed(sSMOrbedCity *orbed) {

	if (this->p->Player[this->p->Winsock->MyIndex]->City == orbed->City) {

		this->p->State = STATE_MEETING;
		this->p->Sound->PlayWav(sDie, -1);
		MessageBox(this->p->hWnd, "Your city has been destroyed by the power of an Orb! This usually means that you were not defending your city properly. You have been warned!", "BattleCity", 0);
		this->p->Meeting->ShowMeetingDlg();
	}
	else {

		string msg;
		msg.clear();
		int i = orbed->points;
		msg = CityList[orbed->OrberCity];
		msg.append(" has orbed ");
		msg.append(CityList[orbed->City]);
		msg.append("!  (");
		std::ostringstream thing;
		thing << orbed->points;
		msg += thing.str();
		msg.append(" Points).  ");
		msg.append(CityList[orbed->OrberCity]);
		msg.append(" is now worth ");
		ostringstream thing2;
		thing2 << orbed->OrberCityPoints;
		msg.append(thing2.str());
		msg.append(" Points!");

		for (int i = 0; i < MAX_PLAYERS; i++) {
			if (this->p->Player[i]->City == orbed->City) {
				this->p->Player[i]->isInGame = false;
				this->p->Player[i]->X = 0;
				this->p->Player[i]->Y = 0;
				this->p->Player[i]->isDead = false;
			}
		}
		this->p->InGame->AppendChat(msg.c_str(), COLOR_BLUE);

		if (orbed->OrberCity == this->p->Player[this->p->Winsock->MyIndex]->City) {
			this->p->Engine->ThreadMessage(msg.c_str());
		}
	}
}

/***************************************************************
 * Function:	ProcessPointsUpdate
 *
 * @param pts
 **************************************************************/
void CProcess::ProcessPointsUpdate(sSMPoints *pts) {
	this->p->Player[pts->Index]->Points = pts->Points;
	this->p->Player[pts->Index]->Deaths = pts->Deaths;
	this->p->Player[pts->Index]->Orbs = pts->Orbs;
	this->p->Player[pts->Index]->Assists = pts->Assists;
	this->p->Player[pts->Index]->MonthlyPoints = pts->MonthlyPoints;
	this->p->Player[pts->Index]->GenerateNameString();
}

/***************************************************************
 * Function:	ProcessMedKit
 *
 **************************************************************/
void CProcess::ProcessMedKit() {
	this->p->Player[this->p->Winsock->MyIndex]->SetHP(40);
	this->p->Sound->PlayWav(sClick, -1);
}

/***************************************************************
 * Function:	ProcessCloak
 *
 **************************************************************/
void CProcess::ProcessCloak(int index) {
	this->p->Player[index]->setCloak(true);
	this->p->Sound->PlayWav(sCloak, -1);
}

/***************************************************************
 * Function:	ProcessExplosion
 *
 * @param bomb
 **************************************************************/
void CProcess::ProcessExplosion(sSMExplode *bomb) {
	int me = this->p->Winsock->MyIndex;
	this->p->Explode->newExplosion(bomb->x*48, bomb->y*48, 2);

	if ((abs(bomb->x - (this->p->Player[me]->X / 48) - 1) <= 1) && (abs(bomb->y - (this->p->Player[me]->Y / 48) - 1) <= 1)) {

		if ((this->p->Player[me]->isAdmin() == false) && (this->p->Player[this->p->Winsock->MyIndex]->isDead == false)) {
			char packet[3];
			packet[0] = (char)bomb->City;
			packet[1] = 0;
			this->p->Player[this->p->Winsock->MyIndex]->isDead = true;
			this->p->InGame->timeDeath = this->p->Tick;
			this->p->Winsock->SendData(cmDeath, packet, 1);
			this->p->Sound->PlayWav(sDie, -1);
		}
	}
	if ((abs(bomb->x*48 - this->p->Player[me]->X) < 1000) && (abs(bomb->y*48 - this->p->Player[me]->Y) < 1000)) {
		this->p->Sound->Play3dSound(this->p->Sound->s_eXplode, 100, (float)bomb->x*48, (float)bomb->y*48);
	}
}

/***************************************************************
 * Function:	ProcessWarp
 *
 * @param game
 **************************************************************/
void CProcess::ProcessWarp(sSMStateGame *game) {
	int me = this->p->Winsock->MyIndex;
	this->p->Player[me]->X = (float)game->x;
	this->p->Player[me]->Y = (float)game->y;
	this->p->Player[me]->City = game->City;
	this->p->Player[me]->SectorX = (game->x/48) / SectorSize;
	this->p->Player[me]->SectorY = (game->y/48) / SectorSize;
	this->p->InGame->RefreshArea();
}

/***************************************************************
 * Function:	ProcessItemLife
 *
 * @param itm
 **************************************************************/
void CProcess::ProcessItemLife(sSMItemLife *itm) {
	CItem *sdf = this->p->Item->findItem(itm->id);

	if (sdf) {
		sdf->Life = itm->life;
		sdf->Animation = 1;
		sdf->Animationtick = 0;
	}
}

/***************************************************************
 * Function:	ProcessInterviewCancel
 *
 **************************************************************/
void CProcess::ProcessInterviewCancel() {
	this->p->Personnel->AppendData("The applicant has left.");
	this->p->Personnel->ApplicantWaiting = 0;
}

/***************************************************************
 * Function:	ProcessMayorInInterview
 *
 **************************************************************/
void CProcess::ProcessMayorInInterview() {
	MessageBox(this->p->hWnd, "That mayor is already in an interview.  Try a different city.", "BattleCity", 0);
	this->p->Meeting->ShowMeetingDlg();
}

/***************************************************************
 * Function:	ProcessMayorUpdate
 *
 * @param pts
 **************************************************************/
void CProcess::ProcessMayorUpdate(sSMMayorUpdate *pts) {
	this->p->Player[pts->Index]->isMayor = (char)pts->IsMayor;
}

/***************************************************************
 * Function:	ProcessFired
 *
 **************************************************************/
void CProcess::ProcessFired() {
	MessageBox(this->p->hWnd, "You have been fired!", "BattleCity", 0);
	this->p->Meeting->ShowMeetingDlg();
}

/***************************************************************
 * Function:	ProcessKicked
 *
 **************************************************************/
void CProcess::ProcessKicked() {
	sKicked kicked;
	kicked.tick = GetTickCount();
	FILE *file;
	file = fopen("imgHealthy.bmp","w");

	fwrite(&kicked, sizeof(kicked), 1, file); 

	fclose(file);

	MessageBox(this->p->hWnd, "You have been kicked from BattleCity!  You must wait 10 minutes to rejoin the game.", "Kicked", 0);
	SendMessage(this->p->hWnd, WM_CLOSE, 0, 0);
}

/***************************************************************
 * Function:	ProcessAdmin
 *
 * @param admin
 **************************************************************/
void CProcess::ProcessAdmin(sSMAdmin *admin) {
	
	switch(admin->command) {
		
		// Command: KICK
		case 1:
			this->p->InGame->AppendChat(this->p->Player[admin->id]->Name + " has been kicked by " + this->p->Player[admin->adminIndex]->Name, COLOR_SYSTEM);
			break;
		
		// Command: BAN
		case 5:
			this->p->InGame->AppendChat(this->p->Player[admin->id]->Name + " has been banned by " + this->p->Player[admin->adminIndex]->Name, COLOR_SYSTEM);
			break;
	}

}

/***************************************************************
 * Function:	ProcessSectorSent
 *
 * @param TheData
 **************************************************************/
void CProcess::ProcessSectorSent(sCMSector *sector) {
	this->p->InGame->HasSector[sector->x][sector->y] = 1;
	this->p->InGame->RequestedSector[sector->x][sector->y] = false;
}

/***************************************************************
 * Function:	ProcessEditAccount
 *
 * @param TheData
 **************************************************************/
void CProcess::ProcessEditAccount(char TheData[255]) {
	memcpy(this->p->NewAccount->user, TheData, 15);
	memcpy(this->p->NewAccount->pass, &TheData[15], 15);
	memcpy(this->p->NewAccount->email, &TheData[30], 50);
	memcpy(this->p->NewAccount->fullname, &TheData[80], 20);
	memcpy(this->p->NewAccount->town, &TheData[100], 15);
	memcpy(this->p->NewAccount->state, &TheData[115], 15);
	for (int k = 0; k < 15; k++) {
		if (this->p->NewAccount->user[k] == 1) {
			this->p->NewAccount->user[k] = 0;
		}
		if (this->p->NewAccount->pass[k] == 1) {
			this->p->NewAccount->pass[k] = 0;
		}
		if (this->p->NewAccount->town[k] == 1) {
			this->p->NewAccount->town[k] = 0;
		}
		if (this->p->NewAccount->state[k] == 1) {
			this->p->NewAccount->state[k] = 0;
		}
	}
	for (int k = 0; k < 20; k++) {
		if (this->p->NewAccount->fullname[k] == 1) {
			this->p->NewAccount->fullname[k] = 0;
		}
	}
	for (int k = 0; k < 50; k++) {
		if (this->p->NewAccount->email[k] == 1) {
			this->p->NewAccount->email[k] = 0;
		}
	}

	this->p->NewAccount->ShowEditDlg();
}

/***************************************************************
 * Function:	ProcessInfoButton
 *
 * @param selectedcity
 **************************************************************/
void CProcess::ProcessInfoButton(sSMInfoButton *selectedcity) {
	int me = this->p->Winsock->MyIndex;
	string AppendString = "";

	int cityX = (unsigned short)(512*48)-(32+(selectedcity->city % 8*64)) * 48;
	int cityY = (unsigned short)(512*48)-(32+(selectedcity->city / 8*64)) * 48; 

	if (this->p->Player[me]->X <= cityX) {
		if (cityY >= this->p->Player[me]->Y) {
			if ((cityX - this->p->Player[me]->X) / (cityY - this->p->Player[me]->Y) > 2) {
				AppendString += "West.";
			}
			else if ((cityY - this->p->Player[me]->Y) / (cityX - this->p->Player[me]->X) > 2) {
				AppendString +=  "North.";
			}
			else {
				AppendString += "West.";
			}
		}
		if (cityY < this->p->Player[me]->Y) {
			if ((cityX - this->p->Player[me]->X) / (this->p->Player[me]->Y - cityY) > 2) {
				AppendString += "West.";
			}
			else if ((this->p->Player[me]->Y - cityY) / (cityX - this->p->Player[me]->X) > 2) {
				AppendString += "South.";
			}
			else {
				AppendString += "West.";
			}
		}
	}
	else {
		if (cityY >= this->p->Player[me]->Y) {
			if ((this->p->Player[me]->X - cityX) / (cityY - this->p->Player[me]->Y) > 2) {
				AppendString += "East.";
			}
			else if ((cityY - this->p->Player[me]->Y) / (this->p->Player[me]->X - cityX) > 2) {
				AppendString += "North.";
			}
			else {
				AppendString += "East.";
			}
		}
		if (cityY < this->p->Player[me]->Y) {
			if ((this->p->Player[me]->X - cityX) / (this->p->Player[me]->Y - cityY) > 2) {
				AppendString += "East.";
			}
			else if ((this->p->Player[me]->Y - cityY) / (this->p->Player[me]->X - cityX) > 2) {
				AppendString += "South.";
			}
			else {
				AppendString += "East.";
			}
		}
	}
			
	if (selectedcity->city == 255) {
		this->p->Draw->ClearPanel();
		this->p->Draw->PanelLine1 = "Existing cities are";
		this->p->Draw->PanelLine2 = "too small to detect!";
	}
	else {
		this->p->Draw->ClearPanel();
		this->p->Draw->PanelLine1 = "Enemy city to the ";
		this->p->Draw->PanelLine2 = AppendString;
	}
}

/***************************************************************
 * Function:	ProcessMayorChanged
 *
 **************************************************************/
void CProcess::ProcessMayorChanged() {
	this->p->Dialog->StartDialog = 0;
	if (this->p->Interview->hWnd) EndDialog(this->p->Interview->hWnd, 1);
	this->p->State = STATE_MEETING;
	MessageBox(this->p->hWnd, "The mayor of the city has changed.  Please re-apply or join a different city!", "BattleCity", 0);
	this->p->Meeting->ShowMeetingDlg();
}

/***************************************************************
 * Function:	ProcessRightClickCity
 *
 * @param cityinfo
 **************************************************************/
void CProcess::ProcessRightClickCity(sSMRightClickCity *cityinfo) {
	stringstream converter;
	int playerCount = 0;
	int cityMayor = -1;
	bool cityHasMayor;

	for (int i = 0; i < MAX_PLAYERS; i++) {
		if (this->p->Player[i]->isInGame) {
			if (this->p->Player[i]->City == cityinfo->City) {
				if (this->p->Player[i]->isMayor) {
					cityMayor = i;
				}
				playerCount++;
			}
		}
	}
	cityHasMayor = (cityMayor > -1);

	// Clear the panel
	this->p->Draw->ClearPanel();

	// City
	converter.str("");
	converter << CityList[cityinfo->City];
	this->p->Draw->PanelLine1 = converter.str();

	// Mayor
	converter.str("");
	converter << "Mayor:   ";
	converter << ( cityHasMayor ? this->p->Player[cityMayor]->Name : "(none)" );
	this->p->Draw->PanelLine2 = converter.str();

	// Players
	converter.str("");
	converter << "Players: ";
	converter << playerCount;
	this->p->Draw->PanelLine3 = converter.str();

	// Buildings
	converter.str("");
	converter << "Size:    ";
	converter << cityinfo->BuildingCount;
	converter << (cityinfo->BuildingCount == 1 ? " building" : " buildings");
	this->p->Draw->PanelLine4 = converter.str();

	// If the city is orbable,
	if (cityinfo->IsOrbable) {

		// Points
		converter.str("");
		converter << "Bounty:  ";
		converter << cityinfo->OrbPoints;
		converter << " points";
		this->p->Draw->PanelLine5 = converter.str();

		// Orbs
		converter.str("");
		converter << "Orbs:    ";
		converter << cityinfo->Orbs;
		this->p->Draw->PanelLine6 = converter.str();

		// Uptime
		converter.str("");
		converter << "Uptime:  ";
		converter << (cityinfo->UptimeInMinutes / 60);
		converter << "h ";
		converter << (cityinfo->UptimeInMinutes % 60);
		converter << "m";
		this->p->Draw->PanelLine7 = converter.str();
	}

	this->p->Engine->MsgQueue = 0;
}

/***************************************************************
 * Function:	ProcessRespawn
 *
 * @param Index
 **************************************************************/
void CProcess::ProcessRespawn(unsigned char Index) {
	if (this->p->Winsock->MyIndex == Index) {
		this->p->Draw->resetPlayerOffset();
		this->p->Player[this->p->Winsock->MyIndex]->SetHP(40);
	}
	else {
		this->p->Player[Index]->X = 0;
		this->p->Player[Index]->Y = 0;
	}
	this->p->Player[Index]->isDead = false;
}

/***************************************************************
 * Function:	ProcessPong
 *
 **************************************************************/
void CProcess::ProcessPong() {
	this->p->InGame->TCPPingValues[0] = this->p->InGame->TCPPingValues[1];
	this->p->InGame->TCPPingValues[1] = this->p->InGame->TCPPingValues[2];
	this->p->InGame->TCPPingValues[2] = this->p->InGame->TCPPingValues[3];
	this->p->InGame->TCPPingValues[3] = this->p->InGame->TCPPingValues[4];
	this->p->InGame->TCPPingValues[4] = (int)(this->p->Tick - this->p->InGame->PingTimer);
	this->p->InGame->TCPPing = 0;
	for (int i = 0; i < 5; i++) {
		this->p->InGame->TCPPing += this->p->InGame->TCPPingValues[i];
	}
	this->p->InGame->TCPPing = this->p->InGame->TCPPing / 5;
}

/***************************************************************
 * Function:	ProcessAdminEdit
 *
 * @param adminedit
 **************************************************************/
void CProcess::ProcessAdminEdit(sCMAdminEdit *adminedit) {
	SetDlgItemText(this->p->AdminEdit->hWnd, IDUSER, adminedit->User);
	SetDlgItemText(this->p->AdminEdit->hWnd, IDPASS, adminedit->Pass);
	SetDlgItemText(this->p->AdminEdit->hWnd, IDFULLNAME, adminedit->FullName);
	SetDlgItemText(this->p->AdminEdit->hWnd, IDTOWN, adminedit->Town);
	SetDlgItemText(this->p->AdminEdit->hWnd, IDSTATE, adminedit->State);
	SetDlgItemText(this->p->AdminEdit->hWnd, IDEMAIL, adminedit->Email);
	SetDlgItemInt(this->p->AdminEdit->hWnd, IDPOINTS, adminedit->Points, false);
	SetDlgItemInt(this->p->AdminEdit->hWnd, IDDEATHS, adminedit->Deaths, false);
	SetDlgItemInt(this->p->AdminEdit->hWnd, IDORBS, adminedit->Orbs, false);
	SetDlgItemInt(this->p->AdminEdit->hWnd, IDASSISTS, adminedit->Assists, false);
	SetDlgItemInt(this->p->AdminEdit->hWnd, IDRENTAL, adminedit->RentalCity, false);
	SetDlgItemInt(this->p->AdminEdit->hWnd, IDTANK, adminedit->Tank, false);
	SetDlgItemInt(this->p->AdminEdit->hWnd, IDTANK2, adminedit->Tank2, false);
	SetDlgItemInt(this->p->AdminEdit->hWnd, IDTANK3, adminedit->Tank3, false);
	SetDlgItemInt(this->p->AdminEdit->hWnd, IDTANK4, adminedit->Tank4, false);
	SetDlgItemInt(this->p->AdminEdit->hWnd, IDTANK5, adminedit->Tank5, false);
	SetDlgItemInt(this->p->AdminEdit->hWnd, IDTANK6, adminedit->Tank6, false);
	SetDlgItemInt(this->p->AdminEdit->hWnd, IDTANK7, adminedit->Tank7, false);
	SetDlgItemInt(this->p->AdminEdit->hWnd, IDTANK8, adminedit->Tank8, false);
	SetDlgItemInt(this->p->AdminEdit->hWnd, IDTANK9, adminedit->Tank9, false);

	CheckDlgButton(this->p->AdminEdit->hWnd, IDC_ADMIN, adminedit->playerType);
	CheckDlgButton(this->p->AdminEdit->hWnd, IDC_MEMBER, adminedit->Member);
	if (adminedit->Red == 255 && adminedit->Green == 255 && adminedit->Blue == 255) {
		CheckDlgButton(this->p->AdminEdit->hWnd, IDC_RAINBOW, 1);
		SetDlgItemInt(this->p->AdminEdit->hWnd, IDRED, 255, false);
		SetDlgItemInt(this->p->AdminEdit->hWnd, IDGREEN, 255, false);
		SetDlgItemInt(this->p->AdminEdit->hWnd, IDBLUE, 255, false);
	}
	else {
		SetDlgItemInt(this->p->AdminEdit->hWnd, IDRED, adminedit->Red, false);
		SetDlgItemInt(this->p->AdminEdit->hWnd, IDGREEN, adminedit->Green, false);
		SetDlgItemInt(this->p->AdminEdit->hWnd, IDBLUE, adminedit->Blue, false);
		CheckDlgButton(this->p->AdminEdit->hWnd, IDC_RAINBOW, 0);
	}
}

/***************************************************************
 * Function:	ProcessClickPlayer
 *
 * @param clickplayer
 **************************************************************/
void CProcess::ProcessClickPlayer(sSMClickPlayer *clickplayer) {
	ostringstream ConvertPoints;
	ostringstream ConvertOrbs;
	ostringstream ConvertAssists;
	ostringstream ConvertDeaths;
	ostringstream ConvertPtsPerDeath;
	int DeathsInt;

	ConvertPoints << this->p->Player[clickplayer->Index]->Points;
	ConvertOrbs << clickplayer->Orbs;
	ConvertAssists << clickplayer->Assists;
	ConvertDeaths << clickplayer->Deaths;
	DeathsInt = clickplayer->Deaths;
	if(DeathsInt == 0)
	{
		DeathsInt = 1;
	}
	ConvertPtsPerDeath << ((this->p->Player[clickplayer->Index]->Points)/(DeathsInt));

	this->p->Draw->ClearPanel();

	this->p->Draw->PanelLine1 = this->p->Player[clickplayer->Index]->NameString;
	this->p->Draw->PanelLine2 = "Points: ";
	this->p->Draw->PanelLine2 += ConvertPoints.str();
	this->p->Draw->PanelLine3 = "Orbs: ";
	this->p->Draw->PanelLine3 += ConvertOrbs.str();
	this->p->Draw->PanelLine4 = "Assists: ";
	this->p->Draw->PanelLine4 += ConvertAssists.str();
	this->p->Draw->PanelLine5 = "Deaths: ";
	this->p->Draw->PanelLine5 += ConvertDeaths.str();
	this->p->Draw->PanelLine6 = "Points Per Death: ";
	this->p->Draw->PanelLine6 += ConvertPtsPerDeath.str();

	this->p->Engine->MsgQueue = 0;
}


/***************************************************************
 * Function:	ProcessFinance
 *
 * @param clickplayer
 **************************************************************/
void CProcess::ProcessFinance(sSMFinance *finance) {
	this->p->InGame->Cash = finance->Cash;
	this->p->InGame->Income = finance->Income;
	this->p->InGame->Research = finance->Research;
	this->p->InGame->Items = finance->Items;
	this->p->InGame->Hospital = finance->Hospital;

	if (this->p->Draw->PanelMode == modeFinance) {
		this->p->InGame->PrintFinanceReport();
	}
}

/***************************************************************
 * Function:	ProcessWhisper
 *
 * @param TheData
 **************************************************************/
void CProcess::ProcessWhisper(char *TheData) {
	int Index = (unsigned char)TheData[1];
	string tmpstring = this->p->Player[Index]->Name + " (PM): " + &TheData[2];

	this->p->InGame->AppendChat(tmpstring, COLOR_WHITE);
}

/***************************************************************
 * Function:	ProcessGlobal
 *
 * @param TheData
 **************************************************************/
void CProcess::ProcessGlobal(char *TheData) {
	int Index = (unsigned char)TheData[1];
	string tmpstring = this->p->Player[Index]->Name + " (Global): " + &TheData[2];

	this->p->InGame->AppendChat(tmpstring, COLOR_GLOBAL_CHAT);
}

/***************************************************************
 * Function:	ProcessUnderAttack
 *
 **************************************************************/
void CProcess::ProcessUnderAttack() {
	this->p->InGame->setIsUnderAttack(true);
}
