#include "Weapon_RocketLauncher.h"
#include "../Raven_Bot.h"
#include "misc/Cgdi.h"
#include "../Raven_Game.h"
#include "../Raven_Map.h"
#include "../lua/Raven_Scriptor.h"
#include "fuzzy/FuzzyOperators.h"

//--------------------------- ctor --------------------------------------------
//-----------------------------------------------------------------------------
RocketLauncher::RocketLauncher(Raven_Bot*   owner) :

Raven_Weapon(type_rocket_launcher,
script->GetInt("RocketLauncher_DefaultRounds"),
script->GetInt("RocketLauncher_MaxRoundsCarried"),
script->GetDouble("RocketLauncher_FiringFreq"),
script->GetDouble("RocketLauncher_IdealRange"),
script->GetDouble("Rocket_MaxSpeed"),
owner)
{
	//setup the vertex buffer
	const int NumWeaponVerts = 8;
	const Vector2D weapon[NumWeaponVerts] = { Vector2D(0, -3),
		Vector2D(6, -3),
		Vector2D(6, -1),
		Vector2D(15, -1),
		Vector2D(15, 1),
		Vector2D(6, 1),
		Vector2D(6, 3),
		Vector2D(0, 3)
	};
	for (int vtx = 0; vtx < NumWeaponVerts; ++vtx)
	{
		m_vecWeaponVB.push_back(weapon[vtx]);
	}

	//setup the fuzzy module
	InitializeFuzzyModule();

}


//------------------------------ ShootAt --------------------------------------
//-----------------------------------------------------------------------------
inline void RocketLauncher::ShootAt(Vector2D pos)
{
	if (NumRoundsRemaining() > 0 && isReadyForNextShot())
	{
		//fire off a rocket!
		m_pOwner->GetWorld()->AddRocket(m_pOwner, pos);

		m_iNumRoundsLeft--;

		UpdateTimeWeaponIsNextAvailable();

		//add a trigger to the game so that the other bots can hear this shot
		//(provided they are within range)
		m_pOwner->GetWorld()->GetMap()->AddSoundTrigger(m_pOwner, script->GetDouble("RocketLauncher_SoundRange"));
	}
}

//---------------------------- Desirability -----------------------------------
//
//-----------------------------------------------------------------------------
double RocketLauncher::GetDesirability(double DistToTarget)
{
	if (m_iNumRoundsLeft == 0)
	{
		m_dLastDesirabilityScore = 0;
	}
	else
	{
		//fuzzify distance and amount of ammo
		m_FuzzyModule.Fuzzify("DistToTarget", DistToTarget);
		m_FuzzyModule.Fuzzify("AmmoStatus", (double)m_iNumRoundsLeft);

		m_dLastDesirabilityScore = m_FuzzyModule.DeFuzzify("Desirability", FuzzyModule::max_av);
	}

	return m_dLastDesirabilityScore;
}

//-------------------------  InitializeFuzzyModule ----------------------------
//
//  set up some fuzzy variables and rules
//-----------------------------------------------------------------------------
void RocketLauncher::InitializeFuzzyModule()
{
	/* Fuzzy Variables */
	FuzzyVariable& DistToTarget = m_FuzzyModule.CreateFLV("DistToTarget");
	FuzzyVariable& Desirability = m_FuzzyModule.CreateFLV("Desirability");
	FuzzyVariable& AmmoStatus = m_FuzzyModule.CreateFLV("AmmoStatus");

	/* Fuzzy Sets */
	FzSet& Target_VeryClose = DistToTarget.AddLeftShoulderSet("Target_VeryClose", 0, 25, 50);
	FzSet& Target_Close = DistToTarget.AddTriangularSet("Target_Close", 25, 50, 100);
	FzSet& Target_Medium = DistToTarget.AddTriangularSet("Target_Medium", 100, 150, 200);
	FzSet& Target_Far = DistToTarget.AddTriangularSet("Target_Far", 150, 200, 250);
	FzSet& Target_VeryFar = DistToTarget.AddRightShoulderSet("Target_VeryFar", 200, 250, 1000);

	FzSet& ReallyDesirable = Desirability.AddRightShoulderSet("ReallyDesirable", 75, 87.5, 100);
	FzSet& MoreDesirable = Desirability.AddTriangularSet("MoreDesirable", 50, 75, 87.5);
	FzSet& Desirable = Desirability.AddTriangularSet("Desirable", 37.5, 50, 75);
	FzSet& LessDesirable = Desirability.AddTriangularSet("LessDesirable", 25, 37.5, 50);
	FzSet& Undesirable = Desirability.AddLeftShoulderSet("Undesirable", 0, 25, 37.5);

	FzSet& Ammo_Full = AmmoStatus.AddRightShoulderSet("Ammo_Full", 30, 35, 100);
	FzSet& Ammo_AlmostFull = AmmoStatus.AddTriangularSet("Ammo_AlmostFull", 15, 25, 30);
	FzSet& Ammo_Okay = AmmoStatus.AddTriangularSet("Ammo_Okay", 5, 15, 25);
	FzSet& Ammo_Low = AmmoStatus.AddTriangularSet("Ammo_Low", 0, 5, 15);
	FzSet& Ammo_AlmostEmpty = AmmoStatus.AddTriangularSet("Ammo_AlmostEmpty", 0, 0, 5);

	/* Fuzzy Rules */
	m_FuzzyModule.AddRule(FzAND(Target_VeryClose, Ammo_AlmostEmpty), Undesirable);
	m_FuzzyModule.AddRule(FzAND(Target_VeryClose, Ammo_Low), Undesirable);
	m_FuzzyModule.AddRule(FzAND(Target_VeryClose, Ammo_Okay), Undesirable);
	m_FuzzyModule.AddRule(FzAND(Target_VeryClose, Ammo_AlmostFull), Undesirable);
	m_FuzzyModule.AddRule(FzAND(Target_VeryClose, Ammo_Full), Undesirable);

	m_FuzzyModule.AddRule(FzAND(Target_Close, Ammo_AlmostEmpty), Undesirable);
	m_FuzzyModule.AddRule(FzAND(Target_Close, Ammo_Low), Undesirable);
	m_FuzzyModule.AddRule(FzAND(Target_Close, Ammo_Okay), Undesirable);
	m_FuzzyModule.AddRule(FzAND(Target_Close, Ammo_AlmostFull), LessDesirable);
	m_FuzzyModule.AddRule(FzAND(Target_Close, Ammo_Full), LessDesirable);

	m_FuzzyModule.AddRule(FzAND(Target_Medium, Ammo_AlmostEmpty), LessDesirable);
	m_FuzzyModule.AddRule(FzAND(Target_Medium, Ammo_Low), Desirable);
	m_FuzzyModule.AddRule(FzAND(Target_Medium, Ammo_Okay), Desirable);
	m_FuzzyModule.AddRule(FzAND(Target_Medium, Ammo_AlmostFull), MoreDesirable);
	m_FuzzyModule.AddRule(FzAND(Target_Medium, Ammo_Full), MoreDesirable);

	m_FuzzyModule.AddRule(FzAND(Target_Far, Ammo_AlmostEmpty), Desirable);
	m_FuzzyModule.AddRule(FzAND(Target_Far, Ammo_Low), MoreDesirable);
	m_FuzzyModule.AddRule(FzAND(Target_Far, Ammo_Okay), MoreDesirable);
	m_FuzzyModule.AddRule(FzAND(Target_Far, Ammo_AlmostFull), ReallyDesirable);
	m_FuzzyModule.AddRule(FzAND(Target_Far, Ammo_Full), ReallyDesirable);

	m_FuzzyModule.AddRule(FzAND(Target_VeryFar, Ammo_AlmostEmpty), MoreDesirable);
	m_FuzzyModule.AddRule(FzAND(Target_VeryFar, Ammo_Low), ReallyDesirable);
	m_FuzzyModule.AddRule(FzAND(Target_VeryFar, Ammo_Okay), ReallyDesirable);
	m_FuzzyModule.AddRule(FzAND(Target_VeryFar, Ammo_AlmostFull), ReallyDesirable);
	m_FuzzyModule.AddRule(FzAND(Target_VeryFar, Ammo_Full), ReallyDesirable);

	/*FzSet& Target_Close = DistToTarget.AddLeftShoulderSet("Target_Close",0,25,150);
	FzSet& Target_Medium = DistToTarget.AddTriangularSet("Target_Medium",25,150,300);
	FzSet& Target_Far = DistToTarget.AddRightShoulderSet("Target_Far",150,300,1000);

	FzSet& VeryDesirable = Desirability.AddRightShoulderSet("VeryDesirable", 50, 75, 100);
	FzSet& Desirable = Desirability.AddTriangularSet("Desirable", 25, 50, 75);
	FzSet& Undesirable = Desirability.AddLeftShoulderSet("Undesirable", 0, 25, 50);

	FzSet& Ammo_Loads = AmmoStatus.AddRightShoulderSet("Ammo_Loads", 10, 30, 100);
	FzSet& Ammo_Okay = AmmoStatus.AddTriangularSet("Ammo_Okay", 0, 10, 30);
	FzSet& Ammo_Low = AmmoStatus.AddTriangularSet("Ammo_Low", 0, 0, 10);

	m_FuzzyModule.AddRule(FzAND(Target_Close, Ammo_Loads), Undesirable);
	m_FuzzyModule.AddRule(FzAND(Target_Close, Ammo_Okay), Undesirable);
	m_FuzzyModule.AddRule(FzAND(Target_Close, Ammo_Low), Undesirable);

	m_FuzzyModule.AddRule(FzAND(Target_Medium, Ammo_Loads), VeryDesirable);
	m_FuzzyModule.AddRule(FzAND(Target_Medium, Ammo_Okay), VeryDesirable);
	m_FuzzyModule.AddRule(FzAND(Target_Medium, Ammo_Low), Desirable);

	m_FuzzyModule.AddRule(FzAND(Target_Far, Ammo_Loads), Desirable);
	m_FuzzyModule.AddRule(FzAND(Target_Far, Ammo_Okay), Undesirable);
	m_FuzzyModule.AddRule(FzAND(Target_Far, Ammo_Low), Undesirable);*/
}


//-------------------------------- Render -------------------------------------
//-----------------------------------------------------------------------------
void RocketLauncher::Render()
{
	m_vecWeaponVBTrans = WorldTransform(m_vecWeaponVB,
		m_pOwner->Pos(),
		m_pOwner->Facing(),
		m_pOwner->Facing().Perp(),
		m_pOwner->Scale());

	gdi->RedPen();

	gdi->ClosedShape(m_vecWeaponVBTrans);
}