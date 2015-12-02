#include "Trigger_TeamGiver.h"
#include "misc/Cgdi.h"
#include "misc/Stream_Utility_Functions.h"
#include <fstream>
#include "../lua/Raven_Scriptor.h"
#include "../constants.h"
#include "../Raven_ObjectEnumerations.h"
#include "../Raven_WeaponSystem.h"

#include "debug/DebugConsole.h"


///////////////////////////////////////////////////////////////////////////////

Trigger_TeamGiver::Trigger_TeamGiver() :

Trigger_LimitedLifetime<Raven_Bot>(250)
{
  //create the vertex buffer for the rocket shape
  const int NumRocketVerts = 8;
  const Vector2D rip[NumRocketVerts] = {Vector2D(0, 3),
                                       Vector2D(1, 2),
                                       Vector2D(1, 0),
                                       Vector2D(2, -2),
                                       Vector2D(-2, -2),
                                       Vector2D(-1, 0),
                                       Vector2D(-1, 2),
                                       Vector2D(0, 3)};
  
  for (int i=0; i<NumRocketVerts; ++i)
  {
    m_vecRLVB.push_back(rip[i]);
  }


  
  
}


void Trigger_TeamGiver::SetRangeAndTrigger(int nodeIndex)
{
	SetBRadius(10);
	AddCircularTriggerRegion(Pos(), BRadius());
	SetGraphNodeIndex(nodeIndex);
}

void Trigger_TeamGiver::SetGrave(GraveMarkers::GraveRecord &gravePtr)
{
	m_graveAssociated = &gravePtr;
}


void Trigger_TeamGiver::Try(Raven_Bot* pBot)
{
  if (this->isActive() && this->isTouchingTrigger(pBot->Pos(), pBot->BRadius()) && pBot->GetTeamId() == m_idTeam)
  {
	  debug_con << "Add weapon from grave !" << "";
	  pBot->GetWeaponSys()->AddWeapon(EntityType());
	  if (m_graveAssociated)
	  {
		  m_graveAssociated->lastWeaponUsed = 0;
		  m_graveAssociated->teamId = 0;
	  }
      SetToBeRemovedFromGame(); // Permet au trigger d'etre definitivement detruit après utilisation
  } 
}

void Trigger_TeamGiver::Read(std::ifstream& in)
{
  double x, y, r;
  int GraphNodeIndex;
  
  in >>  x >> y  >> r >> GraphNodeIndex;

  SetPos(Vector2D(x,y)); 
  SetBRadius(r);
  SetGraphNodeIndex(GraphNodeIndex);

  //create this trigger's region of fluence
  AddCircularTriggerRegion(Pos(), script->GetDouble("DefaultGiverTriggerRange"));
}



void Trigger_TeamGiver::Render()
{
	if (isActive())
	{
		if (m_idTeam == 1)
		{
			gdi->BluePen();
			//gdi->BlueBrush();
		}
		else
		{
			gdi->GreenPen();
			//gdi->GreenBrush();
		}

		gdi->Circle(Pos(), 2);


		if (m_idTeam == 1) gdi->TextColor(0, 0, 200);
		else if (m_idTeam == 2) gdi->TextColor(0, 200, 0);
		else gdi->TextColor(133, 90, 0);

		gdi->TextAtPos(Pos().x - 10, Pos().y - 5, "RIP");

		if (EntityType() != 0)
		{
			gdi->TextColor(133, 90, 0);
			gdi->TextAtPos(Pos().x - 5, Pos().y + 8, (ttos(EntityType())));
		}
	}
	else // trigger vide ?
	{
		gdi->RedPen();
		gdi->RedBrush();
		gdi->Circle(Pos(), 20);
	}

}