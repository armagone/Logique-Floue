#ifndef GOAL_GOTO_GRAVE_H
#define GOAL_GOTO_GRAVE_H
#pragma warning (disable:4786)

#include "Goals/Goal_Composite.h"
#include "Raven_Goal_Types.h"
#include "../Raven_Bot.h"
#include "triggers/trigger.h"
#include "../GraveMarkers.h"


class Goal_GoToGrave : public Goal_Composite<Raven_Bot>
{
private:

	GraveMarkers::GraveRecord* m_grave;

	Trigger<Raven_Bot>*     m_pGiverTrigger;

	//true if a path to the item has been formulated
	bool                    m_bFollowingPath;

	//returns true if the bot sees that the grave it is heading for has been
	//picked up by an ally
	bool hasGraveBeenStolen()const;

public:

	Goal_GoToGrave(Raven_Bot* pBot, GraveMarkers::GraveRecord* m_grave) :Goal_Composite<Raven_Bot>(pBot, goal_goto_grave),
		m_grave(m_grave),
		m_bFollowingPath(false)
	{}


	void Activate();

	int  Process();

	bool HandleMessage(const Telegram& msg);

	void Terminate(){ m_iStatus = completed; }
};






#endif
