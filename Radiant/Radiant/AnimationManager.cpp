#include "AnimationManager.h"



AnimationManager::AnimationManager()
{
	_timer.Start();
	_timer.Reset();
}


AnimationManager::~AnimationManager()
{
	// Clean up everything
	for (auto& a : _EntityToAnimations)
	{
		for (auto& an : a.second)
		{
			delete an.second;
		}
	}
	for (auto& a : _ToAnimate)
	{
		for (auto& an : a.second)
		{
			delete an.second;
		}
	}
}

const void AnimationManager::CreateAnimation(const Entity & entity, std::string name, float duration, std::function<void(float delta, float amount, float offset)> animation, std::function<void()> animationdone)
{
	// Create a new animation
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
	// Check if animation exists
	auto& animations = _EntityToAnimations.find(entity);
	if (animations == _EntityToAnimations.end())
		return;

	auto& find = animations->second.find(name);
	if (find == animations->second.end())
		return;

	// Check if animation is already buffered to avoid memory leak
	auto& find2 = _ToAnimate.find(entity);
	if (find2 != _ToAnimate.end())
	{
		auto& find3 = find2->second.find(name);
		if (find3 != find2->second.end())
		{
			delete find3->second;
		}
	}
	_ToAnimate[entity][find->first] = new Animation(animationdone, amount,0.0f);

	return void();
}

const void AnimationManager::PlayAnimation(const Entity & entity, std::string name, float amount, float offset, std::function<void()> animationdone)
{
	// Check if animation exists
	auto& animations = _EntityToAnimations.find(entity);
	if (animations == _EntityToAnimations.end())
		return;

	auto& find = animations->second.find(name);
	if (find == animations->second.end())
		return;

	// Check if animation is already buffered to avoid memory leak
	auto& find2 = _ToAnimate.find(entity);
	if (find2 != _ToAnimate.end())
	{
		auto& find3 = find2->second.find(name);
		if (find3 != find2->second.end())
		{
			delete find3->second;
		}
	}
	_ToAnimate[entity][find->first] = new Animation(animationdone, amount, offset);

	return void();
}

const void AnimationManager::PlayAnimation(const Entity & entity, std::string name, float amount)
{
	// 	// Check if animation exists
	auto animations = _EntityToAnimations.find(entity);
	if (animations == _EntityToAnimations.end())
		return;

	auto find = animations->second.find(name);
	if (find == animations->second.end())
		return;

	// Check if animation is already buffered to avoid memory leak
	auto& find2 = _ToAnimate.find(entity);
	if (find2 != _ToAnimate.end())
	{
		auto& find3 = find2->second.find(name);
		if (find3 != find2->second.end())
		{
			delete find3->second;
		}
	}
	_ToAnimate[entity][find->first] = new Animation(amount, 0.0f);

	return void();
}

const void AnimationManager::PlayAnimation(const Entity & entity, std::string name, float amount, float offset)
{
	// 	// Check if animation exists
	auto animations = _EntityToAnimations.find(entity);
	if (animations == _EntityToAnimations.end())
		return;

	auto find = animations->second.find(name);
	if (find == animations->second.end())
		return;

	// Check if animation is already buffered to avoid memory leak
	auto& find2 = _ToAnimate.find(entity);
	if (find2 != _ToAnimate.end())
	{
		auto& find3 = find2->second.find(name);
		if (find3 != find2->second.end())
		{
			delete find3->second;
		}
	}
	_ToAnimate[entity][find->first] = new Animation(amount, offset);

	return void();
}

const void AnimationManager::StopAnimation(const Entity & entity)
{
	// Buffer which entity to stop animating
	_ToStop.push_back(entity);
	
	return void();
}

const void AnimationManager::ReleaseEntity(const Entity & entity)
{
	// Buffer which entity to release
	auto& find = _EntityToAnimations.find(entity);
	if(find != _EntityToAnimations.end())
		_ToDelete.push_back(entity);
}

const void AnimationManager::DoAnimations()
{
	// Update all the maps
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

	// vectir for animations that are finsished
	std::vector<dI> done;

	// Loop through all animations and execute them
	float delta = _timer.DeltaTime();
	for (auto& animations : _Animating)
	{
		for (auto& anim : animations.second)
		{
			float d2 = (anim.second->amount / anim.second->duration)*delta;
			anim.second->delta += d2;
			if (fabs(anim.second->delta) >= fabs( anim.second->amount))
			{
				done.push_back(dI(animations.first, anim.first));
				d2 = anim.second->amount - (anim.second->delta-d2);
				anim.second->delta = anim.second->amount;
			}
			anim.second->animation(d2, anim.second->delta, anim.second->offset);

		}
	}

	// If an animation finished call the anim done and remove it.
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
	// Delete the entities in buffer
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
			for (auto& a : animations2->second)
				delete a.second;

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


	// Stop animations in buffer
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
			for (auto& a : animations2->second)
				delete a.second;
			_ToAnimate.erase(entity);
		}	
	}
	_ToStop.clear();

	// Add new entities to be animated
	for (auto& anim : _ToAnimate)
	{
		auto& find = _Animating.find(anim.first);
		if (find != _Animating.end())
		{
			for (auto& a : anim.second)
			{
				auto& find2 = find->second.find(a.first);
				if (find2 != find->second.end())
				{
					float d2 = find2->second->amount - find2->second->delta;
					find2->second->animation(d2, find2->second->amount, find2->second->offset);
					find->second.erase(find2->first);
				}
			}
		}

		auto& r = _Animating[anim.first];
		for (auto& a : anim.second)
		{
			/*auto& find = r.find(a.first);
			if (find != r.end())
			{
				float d2 = find->second->amount - find->second->delta;
				find->second->animation(d2, find->second->amount);
				r.erase(find->first);
			}*/

			r[a.first] = _EntityToAnimations[anim.first][a.first];
			r[a.first]->amount = a.second->amount;
			r[a.first]->offset = a.second->offset;
			r[a.first]->delta = 0.0f;
			if(a.second->animationdone)
				r[a.first]->animationdone = a.second->animationdone;
			delete a.second;
		}
	}
	_ToAnimate.clear();
	return void();
}
