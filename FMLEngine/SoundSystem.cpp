#include "SoundSystem.h"
#include <SDL_mixer.h>
#include <iostream>
#include <thread>
#include <condition_variable>
#include <queue>
#include <map>


#pragma region pImpl
class SDL_SoundSystem::SDL_SoundSystemImpl final
{
public:
	SDL_SoundSystemImpl()
	{
		StartUp();
	};
	~SDL_SoundSystemImpl()
	{
		Shutdown();
	};

	void PlaySound(const SoundId id, const float volume)
	{
		if (m_IsShutdown) return;

		m_Pending.push({ id, volume });
		m_Cv.notify_all();
	}

	void AddSound(const std::string& path, const SoundId id, bool doLoop = false)
	{
		if (m_IsShutdown) return;

		m_Sounds.emplace(id, Sound{ "../data/sounds/" + path, nullptr, false, doLoop });
	}

	void StartUp()
	{
		if (!m_IsShutdown) return;

		Mix_Init(MIX_INIT_OGG | MIX_INIT_MP3);
		Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096);
		m_UpdateThread = std::jthread(&SDL_SoundSystemImpl::Update, this);

		m_IsShutdown = false;
	}

	void Shutdown()
	{
		if (m_IsShutdown) return;

		m_IsShutdown = true;

		for (auto& sound : m_Sounds)
		{
			if (sound.second.isLoaded)
			{
				Mix_FreeChunk(sound.second.pChunk);
				sound.second.isLoaded = false;
			}
		}

		Mix_CloseAudio();
		Mix_Quit();

		m_Cv.notify_all();
	}

	bool IsShutdown()
	{
		return m_IsShutdown;
	}

private:
	struct PlayMessage
	{
		SoundId id;
		float volume;
	};

	struct Sound
	{
		std::string path;
		Mix_Chunk* pChunk;
		bool isLoaded;
		bool doLoop;
	};

	std::map<SoundId, Sound> m_Sounds;
	std::queue<PlayMessage> m_Pending;

	std::condition_variable m_Cv;
	std::mutex m_CvMutex;

	std::jthread m_UpdateThread;

	bool m_IsShutdown{ true };

	void Update()
	{
		while (true)
		{
			// Wait for pending queue to have something in it
			std::unique_lock<std::mutex> lk(m_CvMutex);
			m_Cv.wait(lk, [&] {return !m_Pending.empty() || m_IsShutdown; });

			if (m_IsShutdown) return;

			// Load audio clip if not loaded and play it
			auto& sound = m_Sounds[m_Pending.front().id];
			if (!sound.isLoaded)
			{
				sound.pChunk = Mix_LoadWAV(sound.path.c_str());
				sound.isLoaded = true;
			}

			// Play sound
			sound.pChunk->volume = static_cast<uint8_t>(m_Pending.front().volume);
			Mix_PlayChannel((m_Pending.front().id + 1) % MIX_CHANNELS, sound.pChunk, -(int)sound.doLoop);

			// Pop from pending queue
			m_Pending.pop();
		}
	};
};
#pragma endregion

#pragma region SDL_SoundSystem
SDL_SoundSystem::SDL_SoundSystem()
{
	m_pImpl = std::make_unique<SDL_SoundSystemImpl>();
}
void SDL_SoundSystem::PlaySound(const SoundId id, const float volume)
{
	m_pImpl->PlaySound(id, volume);
}

void SDL_SoundSystem::AddSound(const std::string& path, const SoundId id, bool doLoop)
{
	m_pImpl->AddSound(path, id, doLoop);
}

void SDL_SoundSystem::StartUp()
{
	m_pImpl->StartUp();
}

void SDL_SoundSystem::Shutdown()
{
	m_pImpl->Shutdown();
}

bool SDL_SoundSystem::IsShutdown()
{
	return m_pImpl->IsShutdown();
}
#pragma endregion

#pragma region Logging_SoundSystem
void Logging_SoundSystem::PlaySound(const SoundId id, const float volume)
{
	m_pSS->PlaySound(id, volume);
	std::cout << "Queued sound with ID [" << id << "] at [" << volume << "] volume\n";
}

void Logging_SoundSystem::AddSound(const std::string& path, const SoundId id, bool doLoop)
{
	m_pSS->AddSound(path, id, doLoop);
	std::cout << "Adding sound with ID [" << id << "] and path [../Data/Audio/" << path << "]\n";
}

void Logging_SoundSystem::StartUp()
{
	m_pSS->StartUp();
	std::cout << "Starting up SoundSystem...\n";
};

void Logging_SoundSystem::Shutdown()
{
	m_pSS->Shutdown();
	std::cout << "Shutting down SoundSystem...\n";
};

bool Logging_SoundSystem::IsShutdown()
{
	bool isShutdown{ m_pSS->IsShutdown() };
	std::cout << "SoundSystem is " << (isShutdown ? "shutdown" : "not shutdown") << "\n";
	return isShutdown;
}
#pragma endregion