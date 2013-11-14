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

void Audio::Play(char* file) {
	Instance().playFile(file);
}

Audio::Audio() {
	maxChannel = s3eAudioGetInt(S3E_AUDIO_NUM_CHANNELS);
	channel = 0;
}

void Audio::playFile(char* file) {
	s3eAudioSetInt(S3E_AUDIO_CHANNEL, channel);
	s3eAudioPlay(file);

	if (++channel > maxChannel)
		channel = 0;
}
