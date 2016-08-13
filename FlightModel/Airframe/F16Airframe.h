#ifndef _F16AIRFRAME_H_
#define _F16AIRFRAME_H_

#include "../stdafx.h"

namespace F16
{
	// - canopy status
	// - refueling slot status
	// - dragging chute status
	// - tail hook support?
	// - nav lights
	// - formation lights
	// - landing lights?
	// - damage in sections? 
	// - accumulated over-g stress?

	class F16Airframe
	{
	protected:
		double canopyAngle;		// Canopy status/angle {0=closed;0.9=elevated;1=no draw}
		bool canopySwitchDown; // up/down
		bool canopyGone; // simplify some code


		// TODO: support for each lamp in lights?
		//bool navigationLight[10];
		//bool formationLight[8];
		//bool landingLamp[5];

		// current 3D model has three lights:
		// left, right and back (tail)
		// 0=off, 1=on
		float leftWingLamp;
		float rightWingLamp;
		float backTailLamp;

		bool navigationLights;
		bool formationLights;
		bool landingLights;
		bool strobeLights;

		// TODO:
		// damage status of section in 0.01 increments
		double elementIntegrity[336]; // TODO: check what size we would need here
		bool isImmortal; // <- ignore damage

	public:
		F16Airframe()
			: canopyAngle(0)
			, canopySwitchDown(false)
			, canopyGone(false)
			, leftWingLamp(0)
			, rightWingLamp(0)
			, backTailLamp(0)
			, navigationLights(false)
			, formationLights(false)
			, landingLights(false)
			, strobeLights(false)
			, isImmortal(false)
		{
			// TODO: check values, size (how many we need)
			// is zero "no fault" or "fully broken"? 
			::memset(elementIntegrity, 0, 336*sizeof(double));
		}
		~F16Airframe() {}

		void initCanopyOpen()
		{
			canopySwitchDown = false;
			canopyAngle = 0.9; // up
			canopyGone = false;
		}
		void initCanopyClosed()
		{
			canopySwitchDown = true;
			canopyAngle = 0; // down
			canopyGone = false;
		}

		// canopy open/close toggle
		void canopyToggle()
		{
			canopySwitchDown = !canopySwitchDown;
		}
		void setCanopySwitchUp()
		{
			canopySwitchDown = false;
		}
		void setCanopySwitchDown()
		{
			canopySwitchDown = true;
		}

		void canopyJettison() // <- no binding yet
		{
			canopyAngle = 1.0; // gone
			canopyGone = true;
		}

		// damage to canopy (in case open in flight, failure on sealing..)
		void canopyDamage(int integrityfactor)
		{
			// just set it gone for now
			// -> pressure to ambient pressure
			canopyAngle = 1.0; // gone
			canopyGone = true;
		}

		// draw angle of canopy {0=closed;0.9=elevated;1=no draw}
		float getCanopyAngle() const
		{
			return (float)canopyAngle;
		}

		// update aero drag from canopy when gone
		// (on taxiing does not matter if halfway?)
		void updateCanopyDrag(const double frameTime)
		{
		}

		// animation support for refueling door at the back
		float getRefuelingDoorAngle() const
		{
			// not yet implemented
			return 0;
		}

		float isNavigationLight() const
		{
			return (navigationLights == true) ? 1.0f : 0.0f;
		}
		float isFormationLight() const
		{
			return (formationLights == true) ? 1.0f : 0.0f;
		}
		float isLandingLight() const
		{
			return (landingLights == true) ? 1.0f : 0.0f;
		}
		float isStrobeLight() const
		{
			return (strobeLights == true) ? 1.0f : 0.0f;
		}

		// current 3D model has three lights:
		// left, right and back (tail)
		float getLeftLight() const
		{
			return leftWingLamp;
		}
		float getRightLight() const
		{
			return rightWingLamp;
		}
		float getBackLight() const
		{
			return backTailLamp;
		}

		/*
		bool draggingChuteCap() const
		{}
		*/

		void onAirframeDamage(int Element, double element_integrity_factor)
		{
			// TODO: check what kind of amount of elements we would need here
			if (Element >= 0 && Element < 336)
			{
				elementIntegrity[Element] = element_integrity_factor;
			}
		}

		void onRepair()
		{
			// TODO: check values, size (how many we need)
			// is zero "no fault" or "fully broken"? 
			::memset(elementIntegrity, 0, 336*sizeof(double));
			canopyGone = false;
			canopyAngle = 0;
		}

		bool isRepairNeeded() const
		{
			// TODO: check values, size (how many we need)
			// is zero "no fault" or "fully broken"? 
			for (int i = 0; i < 336; i++)
			{
				if (elementIntegrity[i] > 0)
				{
					return true;
				}
			}
			return false;
		}

		void setImmortal(bool value)
		{
			isImmortal = value;
		}

		// accumulate stress?
		// update pressure?
		void updateFrame(const double frameTime)
		{
			if (canopyGone == false)
			{
				if (canopySwitchDown == false && canopyAngle < 0.9)
				{
					// move up -> increase angle
					canopyAngle += (frameTime / 10);
				}
				else if (canopySwitchDown == true && canopyAngle > 0)
				{
					// move down -> decrease angle
					canopyAngle -= (frameTime / 10);
				}
				canopyAngle = limit(canopyAngle, 0, 0.9);
			}

			// aero drag in case canopy is gone
			updateCanopyDrag(frameTime);

			// TODO: update cockpit pressure from env system
			// -> set to ambient pressure if canopy is gone


			// TODO: if in flight and canopy open -> canopy gone
			// also, if sealing is not working and internal pressure exceeds external -> canopy gone

			// TODO: some light switching logic on/off?


		}
	};
}

#endif // ifndef _F16AIRFRAME_H_
