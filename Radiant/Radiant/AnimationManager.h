#ifndef _ANIMATIONMANAGER_H_
#define _ANIMATIONMANAGER_H_

#pragma once
#include "Entity.h"
#include <string>
#include <unordered_map>
#include "Timer.h"
#include <map>
class AnimationManager
{
	struct Animation
	{
		float duration;
		std::function<void(float delta, float amount, float offset)> animation;
		std::function<void()> animationdone;
		float delta;
		float amount;
		float offset;
		Animation(float duration, std::function<void(float delta, float amount, float offset)> animation, std::function<void()> animationdone) : duration(duration), animation(std::move(animation)), animationdone(std::move(animationdone)), delta(0.0f), amount(0.0f), offset(0.0f)
		{

		}
		Animation(std::function<void()> animationdone ,float amount, float offset) : duration(0.0f), animationdone(std::move(animationdone)), amount(amount), delta(0.0f), offset(offset)
		{

		}
		Animation(float amount, float offset) : duration(0.0f), amount(amount), delta(0.0f), offset(offset)
		{

		}
	};
public:
	AnimationManager();
	~AnimationManager();

	const void CreateAnimation(const Entity& entity, std::string name, float duration, std::function<void(float delta, float amount, float offset)> animation, std::function<void()> animationdone = []() {});
	const void PlayAnimation(const Entity& entity, std::string name, float amount, std::function<void()> animationdone);
	const void PlayAnimation(const Entity& entity, std::string name, float amount, float offset, std::function<void()> animationdone);
	const void PlayAnimation(const Entity& entity, std::string name, float amount);
	const void PlayAnimation(const Entity& entity, std::string name, float amount, float offset);
	const void StopAnimation(const Entity& entity);
	const void ReleaseEntity(const Entity& entity);
	const void DoAnimations();
private:
	const void UpdateMaps();
private:
	Timer _timer;

	std::vector<Entity> _ToStop;
	std::vector<Entity> _ToDelete;
	std::unordered_map<Entity, std::unordered_map<std::string, Animation*>, EntityHasher> _ToAnimate;
	std::unordered_map<Entity, std::unordered_map<std::string, Animation*>, EntityHasher> _Animating;
	std::unordered_map<Entity, std::unordered_map<std::string, Animation*>, EntityHasher> _EntityToAnimations;
};

#endif