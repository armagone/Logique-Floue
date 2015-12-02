#ifndef RAVEN_GOTOGRAVE_EVALUATOR
#define RAVEN_GOTOGRAVE_EVALUATOR
#pragma warning (disable:4786)
//-----------------------------------------------------------------------------
//
//  Name:   GotoGraveGoal_Evaluator.h
//
//  Author: Mat Buckland (www.ai-junkie.com)
//
//  Desc:  class to calculate how desirable the goal of fetching a weapon item
//         is 
//-----------------------------------------------------------------------------

#include "Goal_Evaluator.h"
#include "../Raven_Bot.h"


class GotoGraveGoal_Evaluator : public Goal_Evaluator
{ 
  int   m_iWeaponType;

public:

	GotoGraveGoal_Evaluator(double bias,
                          int   WeaponType):Goal_Evaluator(bias),
                                            m_iWeaponType(WeaponType)
  {}
  
  double CalculateDesirability(Raven_Bot* pBot);

  void  SetGoal(Raven_Bot* pEnt);

  void  RenderInfo(Vector2D Position, Raven_Bot* pBot);
};

#endif