#include "Power.h"

std::string Power::GetDescription(int textWidth) const
{
	{
		std::string spacing = "";
		std::string powerName = _powerName + "\n";
		std::string desc = _description;
		int spacesToAdd = (textWidth - powerName.size()) / 2;
		spacing.append(spacesToAdd, ' ');
		for (int i = textWidth; i < desc.size(); i += textWidth)
		{
			int offset = 0;
			while (desc[i + offset] != ' ' && i + offset >= 0)
				--offset;
			desc[i + offset] = '\n';
			i += offset;
		}
		return spacing + powerName + desc;
	};
}

power_id_t Power::GetType() const
{
	return _type;
}
