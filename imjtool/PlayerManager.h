#pragma once

#include <iostream>

using namespace std;

class Player;

struct PlayerState
{
	float x = -1;
	float y = -1;
	float face = 1;
	float grav = 1;
	float hspeed = 0;
	float vspeed = 0;
	float gravity = 0;
	bool djump = false;

	float imageSpeed = -1;
	float imageIndex = -1;
};

struct PlayerSave
{
	float x = -1;
	float y = -1;
	float face = 1;
	float grav = 1;
};

enum class ShowMask
{
	OnlyPlayer,
	OnlyMask,
	PlayerAndMask,
};

enum class SaveType
{
	OnlyShoot,
	ShootOrBullet,
};

enum class DeathBorder
{
	Killer,
	Solid,
};

enum class JumpState
{
	FirstHold,
	FirstWait,
	SecondHold,
	SecondWait,
	Done,
};

class PlayerManager
{
public:
	PlayerSave currentSave;
	weak_ptr<Player> player;

	float face = 1;
	float grav = 1;
	bool dotkid = false;
	bool dotkidOutline = false;
	bool deathEnable = true;
	bool infjump = false;
	ShowMask showMask = ShowMask::OnlyPlayer;
	SaveType saveType = SaveType::OnlyShoot;
	DeathBorder deathBorder = DeathBorder::Killer;

	// jump analysis
	float x = 0;
	float y = 0;

	JumpState jumpState = JumpState::SecondWait;
	bool frameActionJump = false;
	bool frameActionDjump = false;
	bool frameActionJumpSlow = false;
	bool lastJumpFrameNext = false;
	int jumpCount = 0;
	int frameCountJump1 = 0;
	int frameCountPause = 0;
	int frameCountJump2 = 0;
	int frameCountPause2 = 0;

	bool jcLateWaiting = false;
	bool jcEarlyWaiting = false;
	int jcCount = 0;
	int jcDuration = 0;
	bool jcLeftShiftLast = false;
	bool jcRightShiftLast = false;
	bool jcShow = false;
	int jcCutOff = 3;
	bool jcSuccessWaitingForRelease = false;
	int jcSuccessDuration = 0;

	bool bhopShow = false;
	int bhopOffset = 0;
	bool bhopWaitingLate = false;
	int bhopCount = 0;
	int bhopVspeedPrevious = 0;
	int bhopHintCutOff = 3;


	void update();
	void doAnalysis();
	void save();
	void load();

	shared_ptr<PlayerState> saveState();
	void loadState(shared_ptr<PlayerState> state);
	void updatePlayerImage();
};

