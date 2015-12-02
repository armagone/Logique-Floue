#ifndef TEAM_GIVER_H
#define TEAM_GIVER_H
#pragma warning (disable:4786)
//-----------------------------------------------------------------------------
//
//  Name:     Trigger_TeamGiver.h
//
//  Author:   Mat Buckland
//
//  Desc:     This trigger 'gives' the triggering bot a weapon of the
//            specified type if he is in the team
//
//-----------------------------------------------------------------------------
#include "Triggers/Trigger_LimitedLifeTime.h"
#include "../Raven_Bot.h"
#include "../GraveMarkers.h"
#include <iosfwd>


class Trigger_TeamGiver : public Trigger_LimitedLifetime<Raven_Bot>
{
private:

  //vrtex buffers for rocket shape
  std::vector<Vector2D>         m_vecRLVB;
  std::vector<Vector2D>         m_vecRLVBTrans;


  int         m_idTeam;
  GraveMarkers::GraveRecord*      m_graveAssociated;
  
public:

  //this type of trigger is created when reading a map file
	Trigger_TeamGiver();

	void SetIdTeam(int idTeam){
		m_idTeam = idTeam;
	}

	void SetRangeAndTrigger(int nodeIndex);

	void SetGrave(GraveMarkers::GraveRecord &gravePtr);
	
	
  //if triggered, this trigger will call the PickupWeapon method of the
  //bot. PickupWeapon will instantiate a weapon of the appropriate type.
  void Try(Raven_Bot*);
  
  //draws a symbol representing the weapon type at the trigger's location
  void Render();

  void Read (std::ifstream& is);
};




#endif