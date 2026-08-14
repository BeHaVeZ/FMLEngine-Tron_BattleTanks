#include "SoundSystem.h"
#include <SDL_mixer.h>
#include <atomic>
#include <cmath>
#include <thread>
#include <condition_variable>
#include <queue>
#include <algorithm>
#include <map>
#include <mutex>

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
			if (m_IsShutdown.load()) return;

			{
				std::lock_guard<std::mutex> lk(m_CvMutex);
				m_Pending.push({ id, volume });
			}

			m_Cv.notify_one();
		}

		void AddSound(const std::string& path, const SoundId id, bool doLoop = false)
		{
			if (m_IsShutdown.load()) return;

			std::lock_guard<std::mutex> lock(m_SoundsMutex);
			const auto insertionResult = m_Sounds.emplace(id, Sound{ "data/sounds/" + path, nullptr, false, doLoop });
			if (!insertionResult.second)
			{
				Logger::Log(LogLevel::Error, "Sound ID [%d] is already registered", static_cast<unsigned int>(id));
			}
		}

		void StartUp()
		{
			if (!m_IsShutdown.load()) return;

			if (Mix_Init(MIX_INIT_OGG | MIX_INIT_MP3) == 0) {
				Logger::Log(LogLevel::Error, "Failed to initialize SDL_mixer: %s\n", Mix_GetError());
				return;
			}

			if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096) != 0) {
				Logger::Log(LogLevel::Error, "Failed to open audio: %s\n", Mix_GetError());
				return;
			}

			m_IsShutdown.store(false);
			m_UpdateThread = std::jthread(&SDL_SoundSystemImpl::Update, this);
		}

		void Shutdown()
		{
			if (m_IsShutdown.exchange(true)) return;

			ClearQueue();
			m_Cv.notify_all();
			if (m_UpdateThread.joinable())
			{
				m_UpdateThread.join();
			}

			{
				std::lock_guard<std::mutex> lock(m_SoundsMutex);
				Mix_HaltChannel(-1);
				for (auto& soundEntry : m_Sounds)
				{
					auto& sound = soundEntry.second;
					if (sound.isLoaded)
					{
						Mix_FreeChunk(sound.pChunk);
						sound.pChunk = nullptr;
						sound.isLoaded = false;
					}
				}
				m_Sounds.clear();
			}

			Mix_CloseAudio();
			Mix_Quit();
		}

		bool IsShutdown()
		{
			return m_IsShutdown.load();
		}

		void ClearSounds()
		{
			ClearQueue();
			std::lock_guard<std::mutex> lock(m_SoundsMutex);
			Mix_HaltChannel(-1);
			for (auto& soundEntry : m_Sounds)
			{
				auto& sound = soundEntry.second;
				if (sound.isLoaded)
				{
					Mix_FreeChunk(sound.pChunk);
				}
			}
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

		void ClearQueue()
		{
			std::lock_guard<std::mutex> lk(m_CvMutex);
			std::queue<PlayMessage> empty;
			std::swap(m_Pending, empty);
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
		std::mutex m_SoundsMutex;

		std::jthread m_UpdateThread;

		std::atomic_bool m_IsShutdown{ true };
		bool m_IsMuted{ false };
		float m_CurrentVolume{ .5f };

		void Update()
		{
			while (true)
			{
				PlayMessage message{};
				{
					std::unique_lock<std::mutex> lock(m_CvMutex);
					m_Cv.wait(lock, [&] { return !m_Pending.empty() || m_IsShutdown.load(); });

					if (m_IsShutdown.load()) return;

					message = m_Pending.front();
					m_Pending.pop();
				}

				std::string path;
				{
					std::lock_guard<std::mutex> lock(m_SoundsMutex);
					const auto soundIt = m_Sounds.find(message.id);
					if (soundIt == m_Sounds.end())
					{
						Logger::Log(LogLevel::Error, "Cannot play unknown sound ID [%d]", static_cast<unsigned int>(message.id));
						continue;
					}
					if (!soundIt->second.isLoaded)
					{
						path = soundIt->second.path;
					}
				}

				if (!path.empty())
				{
					Mix_Chunk* loadedChunk = Mix_LoadWAV(path.c_str());
					if (!loadedChunk)
					{
						Logger::Log(LogLevel::Error, "Failed to load sound ID [%d]: %s", static_cast<unsigned int>(message.id), Mix_GetError());
						continue;
					}

					std::lock_guard<std::mutex> lock(m_SoundsMutex);
					const auto soundIt = m_Sounds.find(message.id);
					if (soundIt == m_Sounds.end() || m_IsShutdown.load())
					{
						Mix_FreeChunk(loadedChunk);
						continue;
					}
					soundIt->second.pChunk = loadedChunk;
					soundIt->second.isLoaded = true;
				}

				{
					std::lock_guard<std::mutex> lock(m_SoundsMutex);
					const auto soundIt = m_Sounds.find(message.id);
					if (soundIt == m_Sounds.end() || !soundIt->second.isLoaded)
					{
						continue;
					}

					const int volume = std::clamp(static_cast<int>(message.volume * MIX_MAX_VOLUME), 0, MIX_MAX_VOLUME);
					Mix_VolumeChunk(soundIt->second.pChunk, volume);
					const int channel = Mix_PlayChannel(-1, soundIt->second.pChunk, soundIt->second.doLoop ? -1 : 0);
					if (channel == -1)
					{
						Logger::Log(LogLevel::Error, "Failed to play sound ID [%d]: %s", static_cast<unsigned int>(message.id), Mix_GetError());
					}
				}
			}
		};
	};
#pragma endregion

#pragma region SDL_SoundSystem
	SDL_SoundSystem::SDL_SoundSystem()
	{
		m_pImpl = std::make_unique<SDL_SoundSystemImpl>();
	}
	SDL_SoundSystem::~SDL_SoundSystem() = default;
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
	void SDL_SoundSystem::ClearQueue()
	{
		m_pImpl->ClearQueue();
	}
#pragma endregion

}


