/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

class Audio {
private:
	static Audio& Instance();
	static Audio* instance;

	int maxChannel;
	int channel;

	Audio();
	~Audio();
	Audio(Audio const&){};
	Audio& operator=(Audio const&){};

	void playFile(char* file);
	void playAmbient(char* file);

	int ambientChan;

public:
	static Audio& GetInstance();

	static void Play(char* file);

	static void PlayAmbientCamera();
	static void PlayAmbientCameraCombat();
	static void PlayAmbientMap();
	static void StopAmbient();

	void Update();
};
