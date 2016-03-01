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
	auto& find = animations.find(name);

	if (find != animations.end())
	{
		delete find->second;
	}

	animations[name] = new Animation(duration, std::move(animation), std::move(animationdone));

	return void();
}

const void AnimationManager::PlayAnimation(const Entity & entity, std::string name, float amount, std::function<void()> animationdone)
{
	auto& animations = _EntityToAnimations.find(entity);
	if (animations == _EntityToAnimations.end())
		return;

	auto& find = animations->second.find(name);
	if (find == animations->second.end())
		return;

	auto& find2 = _ToAnimate.find(entity);
	if (find2 != _ToAnimate.end())
	{
		auto& find3 = find2->second.find(name);
		if (find3 != find2->second.end())
		{
			delete find3->second;
		}
	}
	_ToAnimate[entity][find->first] = new Animation(animationdone, amount);// find->second;

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

	_ToAnimate[entity][find->first] = new Animation(amount);// find->second;

	return void();
}

const void AnimationManager::StopAnimation(const Entity & entity)
{
	_ToStop.push_back(entity);
	
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
	UpdateMaps();
	_timer.Tick();
	struct dI
	{
		Entity ent;
		std::string name;
		dI(const Entity& ent, const std::string& name) :ent(ent), name(name)
		{

		}
	};
	std::vector<dI> done;

	float delta = _timer.DeltaTime();
	for (auto& animations : _Animating)
	{
		for (auto& anim : animations.second)
		{
			float d2 = (anim.second->amount / anim.second->duration)*delta;
			anim.second->delta += d2;
			anim.second->animation(d2, anim.second->delta);
			if (anim.second->delta >= anim.second->amount)
				done.push_back(dI(animations.first, anim.first));
		}
	}

	for (auto& e : done)
	{
		_Animating[e.ent][e.name]->animationdone();
		_Animating[e.ent].erase(e.name);
		if (_Animating[e.ent].size() == 0)
			_Animating.erase(e.ent);
	}
	return void();
}

const void AnimationManager::UpdateMaps()
{
	for (auto& entity : _ToDelete)
	{
		auto& animations = _Animating.find(entity);
		if (animations != _Animating.end())
		{
			_Animating.erase(entity);
		}

		auto& animations2 = _ToAnimate.find(entity);
		if (animations2 != _ToAnimate.end())
		{
			_ToAnimate.erase(entity);
		}
		auto& find = _EntityToAnimations.find(entity);
		if (find != _EntityToAnimations.end())
		{
			for (auto& a : find->second)
				delete a.second;

			_EntityToAnimations.erase(entity);
		}

	}
	_ToDelete.clear();
	for (auto& entity : _ToStop)
	{
		auto& animations = _Animating.find(entity);
		if (animations != _Animating.end())
		{
			_Animating.erase(entity);
		}

		auto& animations2 = _ToAnimate.find(entity);
		if (animations2 != _ToAnimate.end())
		{
			_ToAnimate.erase(entity);
		}	
	}
	_ToStop.clear();
	for (auto& anim : _ToAnimate)
	{
		auto& r = _Animating[anim.first];
		for (auto& a : anim.second)
		{
			r[a.first] = _EntityToAnimations[anim.first][a.first];
			r[a.first]->amount = a.second->amount;
			if(a.second->animationdone)
				r[a.first]->animationdone = a.second->animationdone;
			delete a.second;
		}
	}
	_ToAnimate.clear();
	return void();
}
