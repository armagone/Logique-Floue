#include "GotoGraveGoal_Evaluator.h"
#include "../Raven_ObjectEnumerations.h"
#include "misc/Stream_Utility_Functions.h"
#include "../Raven_Game.h"
#include "../Raven_Map.h"
#include "Goal_Think.h"
#include "Raven_Goal_Types.h"
#include "Raven_Feature.h"

#include <string>




//------------------- CalculateDesirability ---------------------------------
//-----------------------------------------------------------------------------
double GotoGraveGoal_Evaluator::CalculateDesirability(Raven_Bot* pBot)
{
  //grab the distance to the closest instance of the weapon type
  double Distance = 0.0;

  if (!(pBot->GetTargetSys()->seeGrave()))
  {
	  return 0.0;
  }

 // GraveMarkers::GraveRecord* grave = pBot->GetSensoryMem()->GetGrave();

  if (Distance == 1)
  {
    return 0;
  }
  else
  {
    //value used to tweak the desirability
    const double Tweaker = 0.15;

    double Health, WeaponStrength;

    Health = Raven_Feature::Health(pBot);

    WeaponStrength = Raven_Feature::IndividualWeaponStrength(pBot,
                                                             m_iWeaponType);
    
    double Desirability = (Tweaker * Health * (1-WeaponStrength)) / Distance;

    //ensure the value is in the range 0 to 1
    Clamp(Desirability, 0, 1);

    Desirability *= m_dCharacterBias;

    return Desirability;
  }
}



//------------------------------ SetGoal --------------------------------------
void GotoGraveGoal_Evaluator::SetGoal(Raven_Bot* pBot)
{
  pBot->GetBrain()->AddGoal_GetItem(m_iWeaponType); 
}

//-------------------------- RenderInfo ---------------------------------------
//-----------------------------------------------------------------------------
void GotoGraveGoal_Evaluator::RenderInfo(Vector2D Position, Raven_Bot* pBot)
{
  std::string s;
  s="GOTOG: ";
  gdi->TextAtPos(Position, s + ttos(CalculateDesirability(pBot), 2));
}