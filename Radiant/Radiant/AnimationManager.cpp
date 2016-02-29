#include "AnimationManager.h"



AnimationManager::AnimationManager()
{
	_timer.Start();
	_timer.Reset();
}


AnimationManager::~AnimationManager()
{
	for (auto& a : _EntityToAnimations)
	{
		for (auto& an : a.second)
		{
			delete an.second;
		}
	}
}

const void AnimationManager::CreateAnimation(const Entity & entity, std::string name, float duration, std::function<void(float delta, float amount)> animation, std::function<void()> animationdone)
{
	auto& animations = _EntityToAnimations[entity];
	auto find = animations.find(name);

	if (find != animations.end())
	{
		delete find->second;
	}

	animations[name] = new Animation(duration, std::move(animation), std::move(animationdone));

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

const void AnimationManager::ReleaseEntity(const Entity & entity)
{
	auto& find = _EntityToAnimations.find(entity);
	if(find != _EntityToAnimations.end())
		_ToDelete.push_back(entity);
}

const void AnimationManager::DoAnimations()
{
	_timer.Tick();

	UpdateMaps();


	std::vector<Entity> done;

	float delta = _timer.DeltaTime();
	for (auto anim : _Animating)
	{
		float d2 = (anim.second->amount / anim.second->duration)*delta;
		anim.second->delta += d2;
		anim.second->animation(d2, anim.second->delta);
		if (anim.second->delta >= anim.second->amount)
			done.push_back(anim.first);
	}

	for (auto& e : done)
	{
		_Animating[e]->amount = 0.0f;
		_Animating[e]->delta = 0.0f;
		_Animating[e]->animationdone();
		_Animating.erase(e);
	}
	return void();
}

const void AnimationManager::UpdateMaps()
{
	for (auto& entity : _ToDelete)
	{
		_Animating.erase(entity);
		_ToAnimate.erase(entity);
		auto& find = _EntityToAnimations[entity];
		for (auto& a : find)
			delete a.second;

		_EntityToAnimations.erase(entity);
	}
	_ToDelete.clear();
	for (auto& entity : _ToStop)
	{
		_Animating.erase(entity);
		_ToAnimate.erase(entity);
	}
	_ToStop.clear();
	for (auto& anim : _ToAnimate)
	{
		_Animating[anim.first] = anim.second;
	}
	_ToAnimate.clear();
	return void();
}
