/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "Audio.h"

#include "s3eAudio.h"

Audio* Audio::instance = 0;

Audio& Audio::Instance() {
	if (instance == NULL) instance = new Audio();
	return *instance;
}

Audio& Audio::GetInstance() {
	return *instance;
}

void Audio::Play(char* file) {
	Instance().playFile(file);
}

void Audio::PlayAmbientCamera() {
	Instance().playAmbient("sounds/camera_ambient.mp3");
}
void Audio::PlayAmbientCameraCombat() {
	Instance().playAmbient("sounds/camera_combat_ambient.mp3");
}
void Audio::PlayAmbientMap() {
	Instance().playAmbient("sounds/map_ambient.mp3");
}
void Audio::StopAmbient() {
	Instance().playAmbient(NULL);
}


Audio::Audio() {
	maxChannel = s3eAudioGetInt(S3E_AUDIO_NUM_CHANNELS);
	channel = 2;
	ambientChan = 0;
}

void Audio::Update() {
	int ambienChan2 = ambientChan == 0 ? 1 : 0;

	int32 defaultVol = s3eAudioGetInt(S3E_AUDIO_VOLUME_DEFAULT);
	int32 volChange = defaultVol / 12; // 12 frames = 500 ms

	{
		s3eAudioSetInt(S3E_AUDIO_CHANNEL, ambientChan);
		int32 vol = s3eAudioGetInt(S3E_AUDIO_VOLUME);
		if (vol+volChange < defaultVol) {
			s3eAudioSetInt(S3E_AUDIO_VOLUME, vol+volChange);
		} else if (vol != defaultVol) {
			s3eAudioSetInt(S3E_AUDIO_VOLUME, defaultVol);
		}
	}

	{
		s3eAudioSetInt(S3E_AUDIO_CHANNEL, ambienChan2);
		int32 vol = s3eAudioGetInt(S3E_AUDIO_VOLUME);
		if (s3eAudioGetInt(S3E_AUDIO_STATUS) == S3E_AUDIO_PLAYING) {
			int32 newVol = vol-volChange;
			if (newVol > 0) {
				s3eAudioSetInt(S3E_AUDIO_VOLUME, vol-volChange);
			} else {
				s3eAudioStop();
			}
		}
	}
}

void Audio::playFile(char* file) {
	s3eAudioSetInt(S3E_AUDIO_CHANNEL, channel);
	s3eAudioPlay(file);

	if (++channel > maxChannel)
		channel = 2;
}

void Audio::playAmbient(char* file) {
	ambientChan = ambientChan == 0 ? 1 : 0;
	s3eAudioSetInt(S3E_AUDIO_CHANNEL, ambientChan);
	s3eAudioSetInt(S3E_AUDIO_VOLUME, 0);
	if (file != NULL) s3eAudioPlay(file, 0);
	else s3eAudioStop();
}
