#include "SoundPlayer.h"


//OpenAL error checking
#define OpenAL_ErrorCheck(message)\
{\
	ALenum error = alGetError();\
	if( error != AL_NO_ERROR)\
	{\
		std::cout << "OpenAL Error: " << error << " with call for " << #message << std::endl;\
	}\
}

#define alec(FUNCTION_CALL)\
FUNCTION_CALL;\
OpenAL_ErrorCheck(FUNCTION_CALL)

int SoundPlayer::loadSound(std::string path) {

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Create buffers that hold our sound data; these are shared between contexts and ar defined at a device level
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	AudioFile<float> monoSoundFile;
	if (!monoSoundFile.load(path)) {
	//if (!monoSoundFile.load("sounds/TestSound_Mono.wav")) {
		std::cout << "failed to load the test mono sound file" << std::endl;
		throw "failed to load the test mono sound file";
	}
	std::vector<uint8_t> monoPCMDataBytes;
	monoSoundFile.writePCMToBuffer(monoPCMDataBytes); //remember, we added this function to the AudioFile library
	auto convertFileToOpenALFormat = [](const AudioFile<float>& audioFile) {
		int bitDepth = audioFile.getBitDepth();
		if (bitDepth == 16)
			return audioFile.isStereo() ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
		else if (bitDepth == 8)
			return audioFile.isStereo() ? AL_FORMAT_STEREO8 : AL_FORMAT_MONO8;
		else
			return -1; // this shouldn't happen!
	};
	//ALuint monoSoundBuffer;
	ALuint monoSoundBuffer{};
	alec(alGenBuffers(1, &monoSoundBuffer));
	m_monoSoundBuffers.push_back(monoSoundBuffer);
	alec(alBufferData(monoSoundBuffer, convertFileToOpenALFormat(monoSoundFile), monoPCMDataBytes.data(), (ALsizei)monoPCMDataBytes.size(), monoSoundFile.getSampleRate()));

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//// load a stereo file into a buffer
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//AudioFile<float> stereoSoundFile;
	//if (!stereoSoundFile.load(path)) {
	//	throw "failed to load the test stereo sound file";
	//}
	//std::vector<uint8_t> stereoPCMDataBytes;
	//stereoSoundFile.writePCMToBuffer(stereoPCMDataBytes); //remember, we added this function to the AudioFile library
	//
	////ALuint stereoSoundBuffer;
	//alec(alGenBuffers(1, &m_stereoSoundBuffer));
	//alec(alBufferData(m_stereoSoundBuffer, convertFileToOpenALFormat(stereoSoundFile), stereoPCMDataBytes.data(), (ALsizei)stereoPCMDataBytes.size(), stereoSoundFile.getSampleRate()));

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// create a sound source that play's our mono sound (from the sound buffer)
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//ALuint monoSource;
	std::vector<ALuint> monoSources{};
	for (size_t ii = 0; ii < m_maxNumParalellSounds; ii++) {
		ALuint monoSource{};
		alec(alGenSources(1, &monoSource));
		alec(alSource3f(monoSource, AL_POSITION, 1.f, 0.f, 0.f));
		alec(alSource3f(monoSource, AL_VELOCITY, 0.f, 0.f, 0.f));
		alec(alSourcef(monoSource, AL_PITCH, 1.f));
		alec(alSourcef(monoSource, AL_GAIN, 1.f));
		alec(alSourcei(monoSource, AL_LOOPING, AL_FALSE));
		alec(alSourcei(monoSource, AL_BUFFER, monoSoundBuffer));
		monoSources.push_back(monoSource);
	}
	m_monoSources.push_back(monoSources);
	m_monoSourceIndices.push_back(0);

	return (int)m_monoSources.size() - 1;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//// create a sound source for our stereo sound; note 3d positioning doesn't work with stereo files because
	//// stereo files are typically used for music. stereo files come out of both ears so it is hard to know
	//// what the sound should be doing based on 3d position data.
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////ALuint stereoSource;
	//alec(alGenSources(1, &m_stereoSource));
	////alec(alSource3f(stereoSource, AL_POSITION, 0.f, 0.f, 1.f)); //NOTE: this does not work like mono sound positions!
	////alec(alSource3f(stereoSource, AL_VELOCITY, 0.f, 0.f, 0.f)); 
	//alec(alSourcef(m_stereoSource, AL_PITCH, 1.f));
	//alec(alSourcef(m_stereoSource, AL_GAIN, 1.f));
	//alec(alSourcei(m_stereoSource, AL_LOOPING, AL_FALSE));
	//alec(alSourcei(m_stereoSource, AL_BUFFER, m_stereoSoundBuffer));
}

//void SoundPlayer::loadAllSounds() {
//	loadSound("flaunch.wav");
//}

void SoundPlayer::playSound(int sound, double pitch, double gain) {


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// play the mono sound source
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	std::vector<ALuint>* monoSources{ &m_monoSources[sound]};
	ALuint monoSource = (*monoSources)[m_monoSourceIndices[sound]];
	m_monoSourceIndices[sound] = (m_monoSourceIndices[sound] + 1) % monoSources->size();
	//m_lazerMonoSourceIndex = 0;

	alec(alSourcef(monoSource, AL_PITCH, (float)pitch));
	alec(alSourcef(monoSource, AL_GAIN, (float)gain));

	alec(alSourcePlay(monoSource));
	//ALint sourceState;
	//alec(alGetSourcei(monoSource, AL_SOURCE_STATE, &sourceState));
	//while (sourceState == AL_PLAYING) {
	//	//basically loop until we're done playing the mono sound source
	//	alec(alGetSourcei(monoSource, AL_SOURCE_STATE, &sourceState));
	//}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// play the stereo sound source after the mono!
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//alec(alSourcePlay(m_stereoSource));
	//alec(alGetSourcei(m_stereoSource, AL_SOURCE_STATE, &sourceState));
	//while (sourceState == AL_PLAYING) {
	//	//basically loop until we're done playing the mono sound source
	//	alec(alGetSourcei(m_stereoSource, AL_SOURCE_STATE, &sourceState));
	//}
}

SoundPlayer::SoundPlayer() {
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// find the default audio device
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const ALCchar* defaultDeviceString = alcGetString(/*device*/nullptr, ALC_DEFAULT_DEVICE_SPECIFIER);
	//ALCdevice* device = alcOpenDevice(defaultDeviceString);
	m_device = alcOpenDevice(defaultDeviceString);
	if (!m_device) {
		std::cout << "failed to get the default device for OpenAL" << std::endl;
		throw "failed to get the default device for OpenAL";
	}
	std::cout << "OpenAL Device: " << alcGetString(m_device, ALC_DEVICE_SPECIFIER) << std::endl;
	//OpenAL_ErrorCheck(device);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Create an OpenAL audio context from the device
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//ALCcontext* context = alcCreateContext(device, /*attrlist*/ nullptr);
	m_context = alcCreateContext(m_device, /*attrlist*/ nullptr);
	//OpenAL_ErrorCheck(context);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Activate this context so that OpenAL state modifications are applied to the context
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (!alcMakeContextCurrent(m_context)) {
		std::cout << "failed to make the OpenAL context the current context" << std::endl;
		throw "failed to make the OpenAL context the current context";
	}
	//OpenAL_ErrorCheck("Make context current");

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Create a listener in 3d space (ie the player); (there always exists as listener, you just configure data on it)
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	alec(alListener3f(AL_POSITION, 0.f, 0.f, 0.f));
	alec(alListener3f(AL_VELOCITY, 0.f, 0.f, 0.f));
	ALfloat forwardAndUpVectors[] = {
		/*forward = */ 1.f, 0.f, 0.f,
		/* up = */ 0.f, 1.f, 0.f
	};
	alec(alListenerfv(AL_ORIENTATION, forwardAndUpVectors));

	//loadAllSounds();
}

SoundPlayer::~SoundPlayer() {
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// clean up our resources!
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	for (size_t ii = 0; ii < m_monoSources.size(); ii++) {
		std::vector<ALuint>* monoSources{ &m_monoSources[ii] };
		for (size_t jj = 0; jj < monoSources->size(); jj++) {
			alec(alDeleteSources(1, &(*monoSources)[jj]));
		}
		alec(alDeleteBuffers(1, &m_monoSoundBuffers[ii]));
	}
	//alec(alDeleteSources(1, &m_stereoSource));
	//alec(alDeleteBuffers(1, &m_stereoSoundBuffer));
	alcMakeContextCurrent(nullptr);
	alcDestroyContext(m_context);
	alcCloseDevice(m_device);
}
