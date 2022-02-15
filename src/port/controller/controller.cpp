#include "ultra64/types.h"
#include "controller.h"
#include <ctime>
#include <fstream>
#include <filesystem>
#include "tas.h"
#include "../options.h"

#ifdef __SWITCH__
#define TAS_DIR "sdmc:/switch/sm64/tas"
#else
#define TAS_DIR "tas"
#endif

using namespace oot::hid;


N64Controller::State::State()
{
	mouse_x	  = 0;
	mouse_y	  = 0;
	has_mouse = false;

	reset();
}

void N64Controller::State::reset()
{
	button	  = 0;
	stick_x	  = 0;
	stick_y	  = 0;
	errnum	  = 0;
	r_stick_x = 0;
	r_stick_y = 0;
	has_mouse = false;
}

N64Controller::N64Controller(bool isLocal) : rawStickX(0), rawStickY(0), stickX(0), stickY(0), stickMag(0),
	buttonDown(0), buttonPressed(0), r_rawStickX(0), r_rawStickY(0), r_stickX(0), r_stickY(0), r_stickMag(0),
	m_isLocal(isLocal), m_state(), m_motorEnabled(false)
{
}



N64Controller::~N64Controller()
{
	if (m_tasFile)//Tas file is open
		fclose(m_tasFile);
	m_tasFile = nullptr;
}



void N64Controller::merge(const N64Controller& controller)
{
	m_state.button |= controller.m_state.button;

	if (abs(m_state.stick_x) < abs(controller.m_state.stick_x))
		m_state.stick_x = controller.m_state.stick_x;

	if (abs(m_state.stick_y) < abs(controller.m_state.stick_y))
		m_state.stick_y = controller.m_state.stick_y;

	if (abs(m_state.r_stick_x) < abs(controller.m_state.r_stick_x))
		m_state.r_stick_x = controller.m_state.r_stick_x;

	if (abs(m_state.r_stick_y) < abs(controller.m_state.r_stick_y))
		m_state.r_stick_y = controller.m_state.r_stick_y;

	if (controller.hasMouse())
	{
		m_state.mouse_x = controller.m_state.mouse_x;
		m_state.mouse_y = controller.m_state.mouse_y;
	}

	m_state.has_mouse |= controller.m_state.has_mouse;
}



static std::string getTasFileName()
{
	return "last_run.tas";
	/*std::error_code error;
	std::filesystem::create_directory(TAS_DIR, error);

	time_t now = time(0);
	tm* time = localtime(&now);

	if (!time)
		return TAS_DIR"/record.tas";

	char buf[64] = { 0 };
	sprintf(buf, TAS_DIR"/%04d.%02d.%02d-%04d.tas", time->tm_year, time->tm_mon + 1, time->tm_mday, time->tm_hour * 60 + time->tm_min);
	return buf;*/
}


static std::ofstream g_tas;

static std::ofstream& stream()
{
	/*if (!g_tas)
	{
		auto name = getTasFileName();
		g_tas = std::ofstream(name, std::ofstream::binary);
		if (g_tas)
			g_tas.write((const char*)&oot::config(), sizeof(oot::config()));
	}*/

	return g_tas;
}

void N64Controller::resolveInputs()
{
#define SRAM_SIZE 0x8000//But this in ultra_reimplementation.h?

	//Recording TAS?
	if (!Tas::isTasPlaying() && oot::config().game().recordTas())
	{
		//stream().write((const char*)&m_state, sizeof(m_state));

		//Write TAS data

		if (!m_tasFile)//Not open yet?
		{
			fopen_s(&m_tasFile, "last-run.tas", "wb");

			if (!m_tasFile)
				return;

			//Write our current config
			//fwrite(&oot::config(), sizeof(oot::config()), 1, m_tasOutput);

			//Write save data
			uint8_t* sram = new uint8_t[SRAM_SIZE];
			FILE* save = nullptr;
			fopen_s(&save, "oot.sav", "wb");
			if (save)
			{
				fwrite(sram, sizeof(uint8_t), SRAM_SIZE, save);
				fclose(save);
			}

			fwrite(sram, sizeof(uint8_t), SRAM_SIZE, m_tasFile);
			delete[] sram;
		}

		if (m_tasFile)//Tas file is open
		{
			//fwrite(&m_state, sizeof(m_state), 1, m_tasFile);
			StateCompressed compressed(m_state);//Compress the state
			compressed.write(m_tasFile);//Write to TAS file
#ifdef _DEBUG
			fflush(m_tasFile);//Flush every frame to keep all the data when the game crashes
#endif
		}
	}

	//Playing back a TAS?
	else if (Tas::isTasPlaying())
	{
		if (!m_tasFile)
		{
			m_tasFile = fopen("last-run.tas", "rb");

			if (!m_tasFile)
				return;

			//fread(&oot::config(), 1, sizeof(oot::config()), fp);

			uint8_t* sram = new uint8_t[SRAM_SIZE];
			fread(sram, sizeof(uint8_t), SRAM_SIZE, m_tasFile);

			FILE* save = nullptr;
			fopen_s(&save, "oot.sav", "wb");
			if (save)
			{
				fwrite(sram, sizeof(uint8_t), SRAM_SIZE, save);
				fclose(save);
			}

			delete[] sram;
		}
		
		if (m_tasFile)
		{
			//fread(&m_state, sizeof(m_state), 1, m_tasFile);//Uncompressed read
			StateCompressed compressed;
			compressed.read(m_tasFile);//Read the compressed state
			m_state = compressed;//Uncompress
		}
	}

	rawStickX     = m_state.stick_x;
	rawStickY     = m_state.stick_y;
	r_rawStickX   = m_state.r_stick_x;
	r_rawStickY   = m_state.r_stick_y;
	buttonPressed = m_state.button & (m_state.button ^ buttonDown);
	
	buttonDown = m_state.button;

	if (oot::config().game().mirror())
	{
		rawStickX   = -rawStickX;
		r_rawStickX = -r_rawStickX;
	}

	// reset the controller's x and y floats.
	this->stickX = 0;
	this->stickY = 0;

	// modulate the rawStickX and rawStickY to be the new f32 values by adding/subtracting 6.
	if (this->rawStickX <= -8)
		this->stickX = this->rawStickX + 6;

	if (this->rawStickX >= 8)
		this->stickX = this->rawStickX - 6;

	if (this->rawStickY <= -8)
		this->stickY = this->rawStickY + 6;

	if (this->rawStickY >= 8)
		this->stickY = this->rawStickY - 6;

	// calculate f32 magnitude from the center by vector length.
	this->stickMag = sqrtf(this->stickX * this->stickX + this->stickY * this->stickY);

	// magnitude cannot exceed 64.0f: if it does, modify the values appropriately to
	// flatten the values down to the allowed maximum value.
	if (this->stickMag > 64)
	{
		this->stickX *= 64 / this->stickMag;
		this->stickY *= 64 / this->stickMag;
		this->stickMag = 64;
	}

	this->r_stickX = 0;
	this->r_stickY = 0;

	if (this->r_rawStickX <= -8)
		this->r_stickX = this->r_rawStickX + 6;

	if (this->r_rawStickX >= 8)
		this->r_stickX = this->r_rawStickX - 6;

	if (this->r_rawStickY <= -8)
		this->r_stickY = this->r_rawStickY + 6;

	if (this->r_rawStickY >= 8)
		this->r_stickY = this->r_rawStickY - 6;

	// calculate f32 magnitude from the center by vector length.
	this->r_stickMag = sqrtf(this->r_stickX * this->r_stickX + this->r_stickY * this->r_stickY);

	// magnitude cannot exceed 64.0f: if it does, modify the values appropriately to
	// flatten the values down to the allowed maximum value.
	if (this->r_stickMag > 64)
	{
		this->r_stickX *= 64 / this->r_stickMag;
		this->r_stickY *= 64 / this->r_stickMag;
		this->r_stickMag = 64;
	}
}

s64 N64Controller::mouse_x() const
{
	return m_state.mouse_x * (oot::config().camera().mousexInvert() ? -1 : 1) * oot::config().camera().mousexScaler();
}

s64 N64Controller::mouse_y() const
{
	return m_state.mouse_y * (oot::config().camera().mouseyInvert() ? -1 : 1) * oot::config().camera().mouseyScaler();
}

/*bool N64Controller::updateRebind(int input)
{
	return false;
}*/