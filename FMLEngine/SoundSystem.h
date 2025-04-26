#pragma once
#include <memory>
#include <string>
#include "Logger.h"

namespace FML
{

	using SoundId = unsigned short;

	class SoundSystem
	{
	public:
		virtual ~SoundSystem() = default;
		virtual void PlaySound(const SoundId id, const float volume) = 0;
		virtual void AddSound(const std::string& path, const SoundId id, bool doLoop = false) = 0;
		virtual void StartUp() = 0;
		virtual void Shutdown() = 0;
		virtual bool IsShutdown() = 0;
		virtual bool IsMuted() const = 0;
		virtual void MuteSound() = 0;
		virtual void UnmuteSound() = 0;
		virtual void ClearSounds() = 0;
		virtual float GetCurrentVolume() const = 0;
		virtual void SetVolume(float newVolume) = 0;
		virtual void ClearQueue() = 0;
	};

	class NullSoundSystem final : public SoundSystem
	{
	public:
		void PlaySound(const SoundId, const float) override {};
		void AddSound(const std::string&, const SoundId, bool = false) override {};
		void StartUp() override { Logger::Log(LogLevel::Info, "Nullsystem has been initialized"); };
		void Shutdown() override {};
		bool IsShutdown() override { return false; };
		virtual bool IsMuted() const { return false; };
		virtual void MuteSound() override {};
		virtual void UnmuteSound() override {};
		virtual void ClearSounds() override {};
		virtual float GetCurrentVolume() const override { return 0.0f; }
		virtual void SetVolume(float) override {};
		virtual void ClearQueue() override {};
	};

	class SDL_SoundSystem final : public SoundSystem
	{
	public:
		explicit SDL_SoundSystem();
		~SDL_SoundSystem() = default;

		virtual void PlaySound(const SoundId id, const float volume) override;
		virtual void AddSound(const std::string& path, const SoundId id, bool doLoop = false) override;
		virtual void StartUp() override;
		virtual void Shutdown() override;
		virtual bool IsShutdown() override;
		virtual bool IsMuted() const override;
		virtual void MuteSound() override;
		virtual void UnmuteSound() override;
		virtual void ClearSounds() override;
		virtual float GetCurrentVolume() const override;
		virtual void SetVolume(float volume) override;
		virtual void ClearQueue() override;

	private:
		class SDL_SoundSystemImpl;
		std::unique_ptr<SDL_SoundSystemImpl> m_pImpl;
	};

	class Logging_SoundSystem final : public SoundSystem
	{
	public:
		Logging_SoundSystem(SoundSystem* ss) : m_pSS(ss) {};

		virtual void PlaySound(const SoundId id, const float volume) override;
		virtual void AddSound(const std::string& path, const SoundId id, bool doLoop = false) override;
		virtual void StartUp() override;
		virtual void Shutdown() override;
		virtual bool IsShutdown() override;
		virtual bool IsMuted() const override { return m_pSS->IsMuted(); };
		virtual void MuteSound() override {};
		virtual void UnmuteSound() override {};
		virtual void ClearSounds() override {}
		virtual void ClearQueue() override {};

	private:
		std::unique_ptr<SoundSystem> m_pSS;
	};

}


