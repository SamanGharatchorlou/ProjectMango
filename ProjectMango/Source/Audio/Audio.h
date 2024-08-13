#pragma once

enum class AudioType
{
	Sound,
	Music
};


class Audio
{
public:
	virtual ~Audio() { };
	virtual bool load(const char* filePath) = 0;

	virtual void play(int channel) const = 0;
	virtual void playNext(int channel) { };
	virtual void playNext(int channel) const { };

	virtual void pause(int channel) const = 0;
	virtual void resume(int channel) const = 0;
	virtual void stop(int channel) { };
	virtual void stop(int channel) const { };

	virtual void fadeIn(int channel, int ms) const = 0;
	virtual void fadeOut(int channel, int ms) const = 0;

	virtual bool isPlaying(int channel) const = 0;

	virtual uintptr_t id() const { return reinterpret_cast<uintptr_t>(this); }

	virtual AudioType type() const = 0;

#if AUDIO_LOGGING
	const BasicString name() const { return FileManager::Get()->getItemName(mFilePath); }
	BasicString mFilePath;
#endif
};



class Sound : public Audio
{
public:
	Sound() : mChunk(nullptr) { }
	~Sound();

	bool load(const char* filePath) override;

	void play(int channel) const override;
	void playNext(int channel) const override { play(channel); }

	void fadeIn(int channel, int ms) const override;
	void fadeOut(int channel, int ms) const override;

	void resume(int channel) const override;
	void pause(int channel) const override;
	void stop(int channel) const override;

	bool isPlaying(int channel) const override;

	AudioType type() const override { return AudioType::Sound; }

private:
	Mix_Chunk *mChunk;
};


class SoundGroup : public Audio
{
public:
	SoundGroup() : playingIndex(0) { }
	~SoundGroup();

	bool load(const char* directoryPath) override;

	void play(int channel) const override;
	void playNext(int channel) override;

	void fadeIn(int channel, int ms) const override;
	void fadeOut(int channel, int ms) const override;

	void resume(int channel) const override;
	void pause(int channel) const override;
	void stop(int channel) override;

	bool isPlaying(int channel) const override;

	AudioType type() const override { return AudioType::Sound; }

private:
	std::vector<Audio*> group;
	int playingIndex;
};


class Music : public Audio
{
public:
	Music() : mMusic(nullptr) { }
	~Music();

	bool load(const char* filePath) override;

	void play(int channel) const override;
	void playNext(int channel) const override { (channel); }

	void fadeIn(int channel, int ms) const override;
	void fadeOut(int channel, int ms) const override;

	void resume(int channel) const override;
	void pause(int channel) const override;
	void stop(int channel) const override;

	bool isPlaying(int channel) const override;

	AudioType type() const override { return AudioType::Music; }

public:
	Mix_Music *mMusic;
};


