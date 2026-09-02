#pragma once

#include<iostream>
#include<AL/al.h>
#include <AL/alc.h>
#include<AudioFile/AudioFile.h>

#include <vector>


class SoundPlayer {
public:

   int loadSound(std::string path);
   //void loadAllSounds();
   void playSound(int sound, double pitch, double gain);

   SoundPlayer();
   ~SoundPlayer();


   ALCdevice* m_device{};
   int m_maxNumParalellSounds{20};
   std::vector<std::vector<ALuint>> m_monoSources{};
   std::vector<int> m_monoSourceIndices{};
   //ALuint m_stereoSource{};
   std::vector<ALuint> m_monoSoundBuffers{};
   //ALuint m_stereoSoundBuffer{};
   ALCcontext* m_context{};
};

