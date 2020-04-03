#include "Sound.h"

Implementation::Implementation()
{
	mpStudioSystem = NULL;
	Sound::ErrorCheck(FMOD::Studio::System::create(&mpStudioSystem));
	Sound::ErrorCheck(mpStudioSystem->initialize(32, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_PROFILE_ENABLE, NULL));

	mpSystem = NULL;
	Sound::ErrorCheck(mpStudioSystem->getCoreSystem(&mpSystem));
}

Implementation::~Implementation()
{
	Sound::ErrorCheck(mpStudioSystem->unloadAll());
	Sound::ErrorCheck(mpStudioSystem->release());
}

void Implementation::Update()
{
	std::vector<ChannelMap::iterator> pStoppedChannels;
	for (auto it = mChannels.begin(), itEnd = mChannels.end(); it != itEnd; ++it)
	{
		bool bIsPlaying = false;
		it->second->isPlaying(&bIsPlaying);
		if (!bIsPlaying)
		{
			pStoppedChannels.push_back(it);
		}
	}

	for (auto& it : pStoppedChannels)
	{
		mChannels.erase(it);
	}

	Sound::ErrorCheck(mpStudioSystem->update());
}

Implementation* implementation = nullptr;

void Sound::Init()
{
	implementation = new Implementation;
}

void Sound::Update()
{
	implementation->Update();
}

void Sound::LoadSound(const std::string& strSoundName, bool b3d, bool bLooping, bool bStream)
{
	auto tFoundIt = implementation->mSounds.find(strSoundName);
	if (tFoundIt != implementation->mSounds.end())
		return;

	FMOD_MODE eMode = FMOD_DEFAULT;
	eMode |= b3d ? FMOD_3D : FMOD_2D;
	eMode |= bLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
	eMode |= bStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

	FMOD::Sound* pSound = nullptr;

	Sound::ErrorCheck(implementation->mpSystem->createSound(strSoundName.c_str(), eMode, nullptr, &pSound));
	if (pSound)
	{
		implementation->mSounds[strSoundName] = pSound;
	}
}

void Sound::UnloadSound(const std::string& strSoundName)
{
	auto tFoundIt = implementation->mSounds.find(strSoundName);
	if (tFoundIt == implementation->mSounds.end())
		return;

	Sound::ErrorCheck(tFoundIt->second->release());
	implementation->mSounds.erase(tFoundIt);
}

int Sound::PlaySound(const std::string& strSoundName, const glm::vec3& vPosition, float fVolumedB)
{
	int nChannelId = implementation->mnNextChannelId++;
	auto tFoundIt = implementation->mSounds.find(strSoundName);
	if (tFoundIt == implementation->mSounds.end())
	{
		// If the sound cannot be found try to load it
		LoadSound(strSoundName);
		tFoundIt = implementation->mSounds.find(strSoundName);
		if (tFoundIt == implementation->mSounds.end())
		{
			return nChannelId;
		}
	}

	FMOD::Channel* pChannel = nullptr;
	Sound::ErrorCheck(implementation->mpSystem->playSound(tFoundIt->second, nullptr, true, &pChannel));
	if (pChannel)
	{
		FMOD_MODE currMode;
		tFoundIt->second->getMode(&currMode);
		if (currMode & FMOD_3D)
		{
			FMOD_VECTOR position = VectorToFmod(vPosition);
			Sound::ErrorCheck(pChannel->set3DAttributes(&position, nullptr));
		}

		Sound::ErrorCheck(pChannel->setVolume(dbToVolume(fVolumedB)));
		Sound::ErrorCheck(pChannel->setPaused(false));
		implementation->mChannels[nChannelId] = pChannel;

	}

	return nChannelId;

}

void Sound::SetChannel3dPosition(int nChannelId, const glm::vec3& vPosition)
{
	auto tFoundIt = implementation->mChannels.find(nChannelId);
	if (tFoundIt == implementation->mChannels.end())
		return;

	FMOD_VECTOR position = VectorToFmod(vPosition);
	Sound::ErrorCheck(tFoundIt->second->set3DAttributes(&position, NULL));
}

void Sound::SetChannelVolume(int nChannelId, float fVolumedB)
{
	auto tFoundIt = implementation->mChannels.find(nChannelId);
	if (tFoundIt == implementation->mChannels.end())
		return;

	Sound::ErrorCheck(tFoundIt->second->setVolume(dbToVolume(fVolumedB)));
}

void Sound::LoadBank(const std::string& strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags)
{
	auto tFoundIt = implementation->mBanks.find(strBankName);
	if (tFoundIt != implementation->mBanks.end())
		return;

	FMOD::Studio::Bank* pBank;
	Sound::ErrorCheck(implementation->mpStudioSystem->loadBankFile((strBankName + ".bank").c_str(), flags, &pBank));

	if (pBank)
	{
		implementation->mBanks[strBankName] = pBank;
	}
}

void Sound::LoadEvent(const std::string& strEventName, const std::string& strEventNumber)
{
	auto tFoundit = implementation->mEvents.find(strEventName);
	if (tFoundit != implementation->mEvents.end())
		return;

	FMOD::Studio::EventDescription* pEventDescription = NULL;
	Sound::ErrorCheck(implementation->mpStudioSystem->getEvent(strEventNumber.c_str(), &pEventDescription));
	if (pEventDescription)
	{
		FMOD::Studio::EventInstance* pEventInstance = NULL;
		Sound::ErrorCheck(pEventDescription->createInstance(&pEventInstance));
		if (pEventInstance)
		{
			implementation->mEvents[strEventName] = pEventInstance;
		}
	}
}

void Sound::PlayEvent(const std::string& strEventName)
{
	// If event not found, load the event
	auto tFoundIt = implementation->mEvents.find(strEventName);
	if (tFoundIt == implementation->mEvents.end())
		return;

	tFoundIt->second->start(); 
}

void Sound::StopEvent(const std::string& strEventName, bool bImmediate)
{
	auto tFoundIt = implementation->mEvents.find(strEventName);
	if (tFoundIt == implementation->mEvents.end())
		return;

	FMOD_STUDIO_STOP_MODE eMode;
	eMode = bImmediate ? FMOD_STUDIO_STOP_IMMEDIATE : FMOD_STUDIO_STOP_ALLOWFADEOUT;
	Sound::ErrorCheck(tFoundIt->second->stop(eMode));
}

bool Sound::isEventPlaying(const std::string& strEventName) const
{
	auto tFoundIt = implementation->mEvents.find(strEventName);
	if (tFoundIt == implementation->mEvents.end())
		return false;

	FMOD_STUDIO_PLAYBACK_STATE* state = NULL;
	if (tFoundIt->second->getPlaybackState(state) == FMOD_STUDIO_PLAYBACK_PLAYING) //help
	{
		return true;
	}

	return false;
}

void Sound::GetEventParameter(const std::string& strEventName, const std::string& strParameterName, float* parameter)
{
	auto tFoundIt = implementation->mEvents.find(strEventName);
	if (tFoundIt == implementation->mEvents.end())
		return;

	Sound::ErrorCheck(tFoundIt->second->getParameterByName(strParameterName.c_str(), parameter));
}

void Sound::SetEventParameter(const std::string& strEventName, const std::string& strParameterName, float fValue)
{
	auto tFoundIt = implementation->mEvents.find(strEventName);
	if (tFoundIt == implementation->mEvents.end())
		return;

	Sound::ErrorCheck(tFoundIt->second->setParameterByName(strParameterName.c_str(), fValue));
}
//// Put new function here
void Sound::SetEventPosition(const std::string& strEventName, const glm::vec3 vPosition)
{
	auto tFoundIt = implementation->mEvents.find(strEventName);
	if (tFoundIt == implementation->mEvents.end())
		return;

	FMOD_3D_ATTRIBUTES newAttributes;

	Sound::ErrorCheck(tFoundIt->second->get3DAttributes(&newAttributes));

	newAttributes.position = VectorToFmod(vPosition);

	Sound::ErrorCheck(tFoundIt->second->set3DAttributes(&newAttributes));

	//AudioEngine::ErrorCheck();
}

FMOD_VECTOR Sound::VectorToFmod(const glm::vec3& vPosition)
{
	FMOD_VECTOR fVec;
	fVec.x = vPosition.x;
	fVec.y = vPosition.y;
	fVec.z = vPosition.z;
	return fVec;
}

void Sound::SetGlobalParameter(const std::string& strParameterName, float fvalue)
{
	Sound::ErrorCheck(implementation->mpStudioSystem->setParameterByName(strParameterName.c_str(), fvalue));
}

float Sound::dbToVolume(float db)
{
	return powf(10.0f, 0.05f * db);
}

float Sound::VolumeTodb(float volume)
{
	return 20.0f * log10f(volume);
}

int Sound::ErrorCheck(FMOD_RESULT result)
{
	if (result != FMOD_OK)
	{
		std::cout << "FMOD ERROR: " << FMOD_ErrorString(result) << std::endl;
		return 1;
	}

	// All good
	return 0;
}

void Sound::Shutdown()
{
	delete implementation;
}

