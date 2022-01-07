#pragma once
#ifndef ES_CORE_INPUT_CONFIG_H
#define ES_CORE_INPUT_CONFIG_H

#include <CECInput.h>
#include <SDL_joystick.h>
#include <SDL_keyboard.h>
#include <map>
#include <sstream>
#include <vector>

namespace pugi { class xml_node; }

#define DEVICE_KEYBOARD -1
#define DEVICE_CEC      -2

enum InputType
{
	TYPE_AXIS,
	TYPE_BUTTON,
	TYPE_HAT,
	TYPE_KEY,
	TYPE_CEC_BUTTON,
	TYPE_COUNT
};

struct Input
{
public:
	int device;
	InputType type;
	int id;
	int value;
	int rawvalue;
	bool configured;

	Input()
	{
		device = DEVICE_KEYBOARD;
		configured = false;
		id = -1;
		value = -999;
		rawvalue = -999;
		type = TYPE_COUNT;
	}

	Input(int dev, InputType t, int i, int val, bool conf) : device(dev), type(t), id(i), value(val), rawvalue(val), configured(conf)
	{
	}

	Input(int dev, InputType t, int i, int val, int rawval, bool conf) : device(dev), type(t), id(i), value(val), rawvalue(rawval), configured(conf)
	{
	}

	std::string getHatDir(int val, bool cardinal = false)
	{
		const int upOrDown = SDL_HAT_UP | SDL_HAT_DOWN;
		const int leftOrRight = SDL_HAT_LEFT | SDL_HAT_RIGHT;
		std::stringstream stream;

		if(val == SDL_HAT_CENTERED) // == 0
			return "centered";

		// if there's an up or down, drop left and right to make it cardinal;
		// if there's no up or down, it can't be diagonal anyway
		if(cardinal && (val & upOrDown))
			val = val & upOrDown;

		if(val & SDL_HAT_UP)
		{
			stream << "up";
			if(val & SDL_HAT_DOWN)
				stream << "&down";
		}
		else if (val & SDL_HAT_DOWN)
		{
			stream << "down";
		}

		if((val & upOrDown) && (val & leftOrRight))
			stream << "-";

		if(val & SDL_HAT_LEFT)
		{
			stream << "left";
			if(val & SDL_HAT_RIGHT)
				stream << "&right";
		}
		else if (val & SDL_HAT_RIGHT)
		{
			stream << "right";
		}

		return stream.str();
	}

	std::string getCECButtonName(int keycode)
	{
		return CECInput::getKeyCodeString(keycode);
	}

	std::string string()
	{
		std::stringstream stream;
		switch(type)
		{
			case TYPE_BUTTON:
				stream << "Button " << id;
				break;
			case TYPE_AXIS:
				stream << "Axis " << id << (value > 0 ? "+" : "-");
				break;
			case TYPE_HAT:
				stream << "Hat " << id << " " << getHatDir(value);
				break;
			case TYPE_KEY:
				stream << "Key " << SDL_GetKeyName((SDL_Keycode)id);
				break;
			case TYPE_CEC_BUTTON:
				stream << "CEC-Button " << getCECButtonName(id);
				break;
			default:
				stream << "Input to string error";
				break;
		}

		return stream.str();
	}
};

class InputConfig
{
public:
	InputConfig(int deviceId, const std::string& deviceName, const std::string& deviceGUID);

	void clear();
	void mapInput(const std::string& name, Input input);
	void unmapInput(const std::string& name); // unmap all Inputs mapped to this name

	inline int getDeviceId() const { return mDeviceId; };
	inline const std::string& getDeviceName() { return mDeviceName; }
	inline const std::string& getDeviceGUIDString() { return mDeviceGUID; }

	//Returns true if Input is mapped to this name, false otherwise.
	bool isMappedTo(const std::string& name, Input input);
	bool isMappedLike(const std::string& name, Input input);

	//Returns a list of names this input is mapped to.
	std::vector<std::string> getMappedTo(Input input);

	// Returns true if there is an Input mapped to this name, false otherwise.
	// Writes Input mapped to this name to result if true.
	bool getInputByName(const std::string& name, Input* result);

	void loadFromXML(pugi::xml_node& root);
	void writeToXML(pugi::xml_node& parent);

	bool isConfigured();

private:
	std::map<std::string, Input> mNameMap;
	const int mDeviceId;
	const std::string mDeviceName;
	const std::string mDeviceGUID;
};

#endif // ES_CORE_INPUT_CONFIG_H
