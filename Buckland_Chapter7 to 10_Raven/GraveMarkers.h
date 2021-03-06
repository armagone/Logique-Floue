#ifndef GRAVE_MARKERS_H
#define GRAVE_MARKERS_H
#pragma warning (disable : 4786)
//-----------------------------------------------------------------------------
//
//  Name:   GraveMarkers.h
//
//  Author: Mat Buckland (ai-junkie.com)
//
//  Desc:   Class to record and render graves at the site of a bot's death
//
//-----------------------------------------------------------------------------
#include <list>
#include <vector>
#include "2d/vector2d.h"
#include "time/crudetimer.h"

class GraveMarkers
{
public:
  
  struct GraveRecord
  {
    Vector2D Position;
    double    TimeCreated;
	int teamId;
	int lastWeaponUsed;

	GraveRecord(Vector2D pos, int teamId, int lastWeaponUsed) :Position(pos),
							TimeCreated(Clock->GetCurrentTime()), 
							teamId(teamId),
							lastWeaponUsed(lastWeaponUsed)
    {}
  };

private:
  
  typedef std::list<GraveRecord> GraveList;

private:

  //how long a grave remains on screen
  double m_dLifeTime;
  // 
  int m_teamId;

  //when a bot dies, a grave is rendered to mark the spot.
  std::vector<Vector2D>   m_vecRIPVB;
  std::vector<Vector2D>   m_vecRIPVBTrans;
  GraveList               m_GraveList;


public:

  GraveMarkers(double lifetime);

  void Update();
  void Render();
  GraveMarkers::GraveRecord AddGrave(Vector2D pos, int teamId, int lastWeaponUsed);
  GraveList GetList(){ return m_GraveList; };


};

#endif