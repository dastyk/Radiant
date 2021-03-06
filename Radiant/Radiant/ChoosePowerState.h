#ifndef _CHOOSE_POWER_STATE_H_
#define _CHOOSE_POWER_STATE_H_
#include "PowersHeader.h"
#include "State.h"
#include "GameState.h"

class ChoosePowerState : public State
{
public:
	ChoosePowerState();
	~ChoosePowerState();

	void Init();
	void Shutdown();

	void Update();
	void Render();

	
private:
	std::vector<Power*> _allPowers;
	Entity _choice1;
	Entity _choice2;
	Entity _choice1Text;
	Entity _choice2Text;
	Entity _powerLabel;
	power_id_t _powerToGive;

	Entity _camera;
	Entity _backgroundObject;
	Entity _light;
	
};


#endif
