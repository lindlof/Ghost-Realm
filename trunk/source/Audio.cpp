/* ========================================================================= *
 * Ghost Hunter Project                                                      *
 * https://ghosthuntermobile.wordpress.com/                                  *
 * ========================================================================= *
 * Copyright (C) 2013 Ghost Hunter Project                                   *
 *                                                                           *
 * Licensed under the Apache License, Version 2.0 (the "License");           *
 * you may not use this file except in compliance with the License.          *
 * You may obtain a copy of the License at                                   *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 * Unless required by applicable law or agreed to in writing, software       *
 * distributed under the License is distributed on an "AS IS" BASIS,         *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
 * See the License for the specific language governing permissions and       *
 * limitations under the License.                                            *
 * ========================================================================= */

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
void Audio::PlayAmbientMapInstantly() {
	Instance().playAmbient("sounds/map_ambient.mp3");
	s3eAudioSetInt(S3E_AUDIO_VOLUME, s3eAudioGetInt(S3E_AUDIO_VOLUME_DEFAULT));
}
void Audio::StopAmbient() {
	Instance().playAmbient(NULL);
}


Audio::Audio() {
	maxChannel = s3eAudioGetInt(S3E_AUDIO_NUM_CHANNELS) - 1;
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
