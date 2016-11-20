/*
* by Jiahuan.Liu
* 04/29/2016
* @SCUT
*/
#ifndef TARGET_H_
#define TARGET_H_

#include "Object.h"
#include "Detection.h"
#include "Scene.h"

class Target : public Object
{
public:
	Target();
	Target(Object obj)
		:Object(obj) {}
	unsigned int	getId() const;
	unsigned int	getTotalCount() const;
	unsigned int	getStateCount() const;
	unsigned int	getAssignedCount() const;
	unsigned int	getStateAssignedCount() const;
	Speed			getSpeed() const;
	virtual void	copy(const Detection& det);
	void			setId(unsigned int id);
	virtual void	updateCount(bool isAssigned);
	//void			updateSimConf(float sim);
	float			calTrackletConf() const;
	float			getTermWeight() const;
protected:
	void			_updateSpeed(Object& det, unsigned int interval);
protected:
	unsigned int	_id;
	unsigned int	_totalCount;//
	unsigned int	_stateCount;//
	unsigned int	_assignedCount;//
	unsigned int	_stateAssignedCount;//
	Speed			_speed;//
	float			_exConf;
	float			_ocConf;
	//float			_simConf;
};

class TargetIF
{
public:
	virtual void	updateSceneConf(Scene &scene) = 0;
};

class TrackedTarget : public Target, public TargetIF
{
public:
	TrackedTarget() {}
	TrackedTarget(const Target& Target);
	void	updateSpeed(Object& det);
	void	updateSceneConf(Scene &scene);
};

class PotentialTarget : public Target, public TargetIF
{
public:
	PotentialTarget()
		:_initWeight(0), _consecMissedCount(0), _consecAssignedCount(0){}
	//void			copy(const Detection& det);//det confidence
	void			updateSpeed(Object& det);
	void			updateInitWeight(Scene &scene);
	void			updateSceneConf(Scene &scene);
	unsigned int	getConsecMissedCount() const;
	unsigned int	getConsecAssignedCount() const;
	void			updateCount(bool isAssigned);
	void			setInitPos(Position &pos) { _initPosition = pos; }
	Position		getInitPos() { return _initPosition; }
	float			calInitConf() const;
private:
	float			_initWeight;
	Position		_initPosition;
	unsigned int	_consecMissedCount;//
	unsigned int	_consecAssignedCount;//
	//float			_detConf;
};

class LostTarget : public Target, public TargetIF
{
public:
	LostTarget()
		:_lostWeight(0), _consecMissedCount(0), _consecAssignedCount(0){}
	LostTarget(const Target& tTarget);
	void			updateSpeed(Object& det);
	void			updateLostWeight(Scene &scene);
	void			updateSceneConf(Scene &scene);
	unsigned int	getConsecMissedCount() const;
	unsigned int	getConsecAssignedCount() const;
	void			updateCount(bool isAssigned);
	void			setLostPos(Position &pos) { _lostPosition = pos; }
	Position		getLostPos() { return _lostPosition; }
	float			calTermConf() const;
private:
	float			_lostWeight;
	Position		_lostPosition;
	unsigned int	_consecMissedCount;//
	unsigned int	_consecAssignedCount;//
};

#endif