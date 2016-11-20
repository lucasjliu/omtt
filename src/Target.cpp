/*
* by Jiahuan.Liu
* 04/29/2016
* @SCUT
*/
#include "Target.h"
#include "Motion.h"

Target::Target()
	:_id(0), _totalCount(0), _stateCount(0), _assignedCount(0), 
	_stateAssignedCount(0), _speed(), _exConf(0), _ocConf(0) {}

unsigned int Target::getId() const
{
	return _id;
}

unsigned int Target::getTotalCount() const
{ 
	return _totalCount;
}

unsigned int Target::getStateCount() const
{
	return _stateCount;
}

unsigned int Target::getAssignedCount() const
{
	return _assignedCount;
}

unsigned int Target::getStateAssignedCount() const
{
	return _stateAssignedCount;
}

Speed Target::getSpeed() const
{ 
	return _speed; 
}

void Target::copy(const Detection& det)
{
	_position = det.getPos();
	_size = det.getSize();
	_appearance = det.getAppearance();
}

void Target::setId(unsigned int id) 
{ 
	_id = id; 
}

void Target::_updateSpeed(Object& det, unsigned int interval)
{
	_speed = Motion::calSpeed(*this, det, interval);
}

void Target::updateCount(bool isAssigned)
{ 
	_totalCount++;
	_stateCount++;
	_assignedCount += isAssigned;
	_stateAssignedCount += isAssigned;
}

float Target::calTrackletConf() const
{
	return 0;
}

float Target::getTermWeight() const
{
	return Max(_exConf, _ocConf);
}

TrackedTarget::TrackedTarget(const Target& target)
	:Target(target)
{
	_stateCount = 1;
	_stateAssignedCount = 1;
}

void TrackedTarget::updateSpeed(Object& det)
{
	Target::_updateSpeed(det, 1);
}

void TrackedTarget::updateSceneConf(Scene &scene)
{
	_exConf = scene.getEx(getStandPos().x + _speed.x, getStandPos().y + _speed.y);
	_ocConf = scene.getOc(getStandPos().x + _speed.x, getStandPos().y + _speed.y);
}

void PotentialTarget::updateSpeed(Object& det)
{
	Target::_updateSpeed(det, 1);
}

void PotentialTarget::updateInitWeight(Scene &scene)
{
	_initWeight = scene.getEn(getStandPos().x, getStandPos().y);
}

void PotentialTarget::updateSceneConf(Scene &scene)
{
	_exConf = scene.getEx(getStandPos().x + _speed.x * _consecMissedCount,
		getStandPos().y + _speed.y * _consecMissedCount);
	_ocConf = scene.getOc(getStandPos().x + _speed.x * _consecMissedCount,
		getStandPos().y + _speed.y * _consecMissedCount);
}

unsigned int PotentialTarget::getConsecMissedCount() const
{
	return _consecMissedCount;
}

unsigned int PotentialTarget::getConsecAssignedCount() const
{
	return _consecAssignedCount;
}

void PotentialTarget::updateCount(bool isAssigned)
{
	Target::updateCount(isAssigned);
	if (isAssigned)
	{
		_consecAssignedCount++;
		_consecMissedCount = 0;
	}
	else
	{
		_consecAssignedCount = 0;
		_consecMissedCount++;
	}
}

float PotentialTarget::calInitConf() const
{
	return _initWeight;
}

LostTarget::LostTarget(const Target& target)
	:Target(target), _lostWeight(0),
	_consecMissedCount(0), _consecAssignedCount(0)
{
	_stateCount = 0;
	_stateAssignedCount = 0;
}

void LostTarget::updateSpeed(Object& det)
{
	Target::_updateSpeed(det, _consecMissedCount + 1);
}

void LostTarget::updateLostWeight(Scene &scene)
{
	_lostWeight = Max(scene.getEx(getStandPos().x + _speed.x, getStandPos().y + _speed.y),
		scene.getOc(getStandPos().x + _speed.x, getStandPos().y + _speed.y));
}

void LostTarget::updateSceneConf(Scene &scene)
{
	_exConf = scene.getEx(getStandPos().x + _speed.x * _consecMissedCount,
		getStandPos().y + _speed.y * _consecMissedCount);
	_ocConf = scene.getOc(getStandPos().x + _speed.x * _consecMissedCount,
		getStandPos().y + _speed.y * _consecMissedCount);
}

unsigned int LostTarget::getConsecMissedCount() const
{
	return _consecMissedCount;
}

unsigned int LostTarget::getConsecAssignedCount() const
{
	return _consecAssignedCount;
}

void LostTarget::updateCount(bool isAssigned)
{
	Target::updateCount(isAssigned);
	if (isAssigned)
	{
		_consecAssignedCount++;
		_consecMissedCount = 0;
	}
	else
	{
		_consecAssignedCount = 0;
		_consecMissedCount++;
	}
}

float LostTarget::calTermConf() const
{
	return _lostWeight;
	//return 0;
}