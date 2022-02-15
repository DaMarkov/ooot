#pragma once
#include <stdio.h>
#include "controllers.h"



namespace oot
{
	namespace hid
	{
		class Tas : public N64Controller, public InputDevice
		{
		public:
			static bool isTasPlaying();
			static void playTas(bool enabled);

			Tas();
			virtual ~Tas();
			void scan() override;
			void update();

		private:
			FILE* fp = nullptr;
		};
	}
}