#include "AnimationManager.h"



AnimationManager::AnimationManager()
{
	_timer.Start();
	_timer.Reset();
}


AnimationManager::~AnimationManager()
{
}

const void AnimationManager::CreateAnimation(const Entity & entity, std::string name, float duration, std::function<void(float delta)> animation)
{
	auto& animations = _EntityToAnimations[entity];
	auto find = animations.find(name);

	if (find != animations.end())
	{
		delete find->second;
	}

	animations[name] = new Animation(duration, std::move(animation));

	return void();
}

const void AnimationManager::PlayAnimation(const Entity & entity, std::string name, float amount)
{
	auto animations = _EntityToAnimations.find(entity);
	if (animations == _EntityToAnimations.end())
		return;

	auto find = animations->second.find(name);
	if (find == animations->second.end())
		return;

	_ToAnimate[entity] = find->second;
	_ToAnimate[entity]->amount = amount;

	return void();
}

const void AnimationManager::StopAnimation(const Entity & entity)
{
	auto animations = _Animating.find(entity);
	if (animations != _Animating.end())
	{
		_ToStop.push_back(entity);
		return;
	}

	auto animations2 = _ToAnimate.find(entity);
	if (animations2 != _ToAnimate.end())
	{
		_ToStop.push_back(entity);
		return;
	}

	return void();
}

const void AnimationManager::DoAnimations()
{
	_timer.Tick();

	std::vector<Entity> done;

	float delta = _timer.DeltaTime();
	for (auto anim : _Animating)
	{
		float d2 = (anim.second->amount / anim.second->duration)*delta;
		anim.second->delta += d2;
		anim.second->animation(d2);
		if (anim.second->delta >= anim.second->amount)
			done.push_back(anim.first);
	}

	for (auto& e : done)
		_Animating.erase(e);

	return void();
}

const void AnimationManager::UpdateMaps()
{
	for (auto& entity : _ToStop)
	{
		_Animating.erase(entity);
		_ToAnimate.erase(entity);
	}

	for (auto& anim : _ToAnimate)
	{
		_Animating[anim.first] = anim.second;
	}
	return void();
}
