#include "SoundSystem.h"
#include "SoundSystem.h"
#include <SDL_mixer.h>
#include "Logger.h"
#include <thread>
#include <condition_variable>
#include <queue>
#include <map>

namespace FML
{

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
			Logger::Log(LogLevel::Info, "Queued sound with ID [%d] at [%f] volume\n", id, volume);
		}

		void AddSound(const std::string& path, const SoundId id, bool doLoop = false)
		{
			if (m_IsShutdown) return;

			m_Sounds.emplace(id, Sound{ "data/sounds/" + path, nullptr, false, doLoop });
			Logger::Log(LogLevel::Info, "Added sound with ID [%d] and path data/sounds/%s]\n", id, path.c_str());
		}

		void StartUp()
		{
			if (!m_IsShutdown) return;

			if (Mix_Init(MIX_INIT_OGG | MIX_INIT_MP3) == 0) {
				Logger::Log(LogLevel::Error, "Failed to initialize SDL_mixer: %s\n", Mix_GetError());
				return;
			}

			if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096) != 0) {
				Logger::Log(LogLevel::Error, "Failed to open audio: %s\n", Mix_GetError());
				return;
			}

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

		void ClearSounds()
		{
			Mix_HaltChannel(-1);
			m_Sounds.clear();
		}

		float GetCurrentVolume() const
		{
			return m_CurrentVolume;
		}

		void MuteSound()
		{
			if (!m_IsMuted)
			{
				Mix_Volume(-1, 0);
				m_IsMuted = true;
			}
		}

		void UnmuteSound()
		{
			if (m_IsMuted)
			{
				int sdlVolume = static_cast<int>(std::round(m_CurrentVolume * MIX_MAX_VOLUME));
				sdlVolume = std::clamp(sdlVolume, 0, MIX_MAX_VOLUME);
				Mix_Volume(-1, sdlVolume);
				m_IsMuted = false;
			}
		}

		void SetVolume(float newVolume)
		{
			if (newVolume < .0f || newVolume > 1.f) return;
			m_CurrentVolume = newVolume;

			if (!m_IsMuted)
			{
				int sdlVolume = static_cast<int>(std::round(m_CurrentVolume * MIX_MAX_VOLUME));
				sdlVolume = std::clamp(sdlVolume, 0, MIX_MAX_VOLUME);
				Mix_Volume(-1, sdlVolume);
			}
		}

		bool IsMuted()
		{
			return m_IsMuted;
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
		bool m_IsMuted{ false };
		float m_CurrentVolume{ .5f };

		void Update()
		{
			while (true)
			{
				std::unique_lock<std::mutex> lk(m_CvMutex);
				m_Cv.wait(lk, [&] { return !m_Pending.empty() || m_IsShutdown; });

				if (m_IsShutdown) return;

				auto& sound = m_Sounds[m_Pending.front().id];
				if (!sound.isLoaded) {
					sound.pChunk = Mix_LoadWAV(sound.path.c_str());
					if (!sound.pChunk) {
						std::cerr << "Failed to load sound: " << Mix_GetError() << std::endl;
						m_Pending.pop();
						continue;
					}
					sound.isLoaded = true;
				}

				sound.pChunk->volume = static_cast<uint8_t>(m_Pending.front().volume * MIX_MAX_VOLUME);
				int channel = Mix_PlayChannel(-1, sound.pChunk, sound.doLoop ? -1 : 0);
				if (channel == -1)
				{
					Logger::Log(LogLevel::Error, "Failed to play sound with ID [%d] at [%f] volume\n", m_Pending.front().id, m_Pending.front().volume);
				}
				else
				{
					Logger::Log(LogLevel::Info, "Playing sound from update with ID [%d] at [%f] volume\n", m_Pending.front().id, m_Pending.front().volume);
				}

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
	bool SDL_SoundSystem::IsMuted() const
	{
		return m_pImpl->IsMuted();
	}
	void SDL_SoundSystem::MuteSound()
	{
		m_pImpl->MuteSound();
	}
	void SDL_SoundSystem::UnmuteSound()
	{
		m_pImpl->UnmuteSound();
	}
	void SDL_SoundSystem::ClearSounds()
	{
		m_pImpl->ClearSounds();
	}

	float FML::SDL_SoundSystem::GetCurrentVolume() const
	{
		return m_pImpl->GetCurrentVolume();
	}

	void SDL_SoundSystem::SetVolume(float newVolume)
	{
		m_pImpl->SetVolume(newVolume);
	}
#pragma endregion

#pragma region Logging_SoundSystem
	void Logging_SoundSystem::PlaySound(const SoundId id, const float volume)
	{
		m_pSS->PlaySound(id, volume);
		Logger::Log(LogLevel::Info, "Queued sound with ID [%d] at [%f] volume\n", id, volume);
	}

	void Logging_SoundSystem::AddSound(const std::string& path, const SoundId id, bool doLoop)
	{
		m_pSS->AddSound(path, id, doLoop);
		Logger::Log(LogLevel::Info, "Added sound with ID [%d] and path data/sounds/%s]\n", id, path.c_str());
	}

	void Logging_SoundSystem::StartUp()
	{
		m_pSS->StartUp();
		Logger::Log(LogLevel::Info, "Starting up SoundSystem...\n");
	};

	void Logging_SoundSystem::Shutdown()
	{
		m_pSS->Shutdown();
		Logger::Log(LogLevel::Info, "Shutting down SoundSystem...\n");
	};

	bool Logging_SoundSystem::IsShutdown()
	{
		bool isShutdown{ m_pSS->IsShutdown() };
		Logger::Log(LogLevel::Info, "SoundSystem is %s\n", isShutdown ? "shutdown" : "not shutdown");
		return isShutdown;
	}
#pragma endregion

}


