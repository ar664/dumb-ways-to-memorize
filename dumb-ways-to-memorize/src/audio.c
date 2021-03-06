#include "audio.h"
#include "globals.h"
#ifdef __WIN32
	#include <SDL.h>
#else
	#include <SDL2/SDL.h>
#endif
#include <stdio.h>
#include <stdlib.h>

SDL_AudioSpec *gAudioSpec = NULL;
char *SoundVariableNames[] = {"sound_player", "sound_enemy", "sound_music", "sound_other", 0};

sound_t* LoadSound(char** files, sound_mixer_group group)
{
	int sound_count, i;
	struct Mix_Chunk *chunks, *temp_chunk;
	sound_t *sound;
	if(!files)
	{
		return NULL;
	}
	if(!files[0])
	{
		return NULL;
	}
	sound = (sound_t*) malloc(sizeof(sound_t));
	memset(sound, 0, sizeof(sound_t));
	if(group == SOUND_GROUP_MUSIC)
	{
		sound->music = Mix_LoadMUS(files[0]);
		sound->sounds = NULL;
	} else
	{
		sound_count = CountMem(files, sizeof(char*));
		chunks = (struct Mix_Chunk*) malloc(sizeof(struct Mix_Chunk)*(sound_count+1));
		for( i = 0; i < sound_count; i++)
		{
			temp_chunk = Mix_LoadWAV(files[i]);
			if(!temp_chunk)
			{
				printf("Audio File %s could not be loaded \n", files[i]);
				printf("SDL Error : %s \n", SDL_GetError());
				return NULL;
			}
			memcpy(&chunks[i], &temp_chunk, sizeof(struct Mix_Chunk));
		}
		memset(&chunks[sound_count], 0, sizeof(struct Mix_Chunk));
		sound->music = NULL;
	}
	sound->group_id = group;
	return sound;
}

int PlaySound(sound_t* sound, sound_mixer_effects number)
{
	int channel;
	channel=Mix_GroupNewer(sound->group_id);
	if(channel == -1)
	{
		return -1;
	}
	if(number == SOUND_EFFECT_NULL )
	{
		if(sound->music)
		{
			Mix_PlayMusic(sound->music, 0);
		}
	}
	Mix_PlayChannel(channel, &sound->sounds[number], 0);
	return 0;
}

sound_mixer_group StrToSoundType(const char* str)
{
	int i;
	if(!str)
	{
		return SOUND_GROUP_MAX;
	}
	for(i = SOUND_GROUP_PLAYER; i < SOUND_GROUP_MAX; i++)
	{
		if(!strcmp(str, SoundVariableNames[i]))
		{
			return (sound_mixer_group) i;
		}
	}
	return SOUND_GROUP_MAX;
}

int InitAudio()
{
	int frequency, channels;
	Uint16 format;
	int audio_good;

	//Default values
	frequency = 44100;
	format = MIX_DEFAULT_FORMAT;
	channels = 2;

	if(SDL_Init(SDL_INIT_AUDIO))
	{
		printf("SDL can't initialize audio : %s \n", SDL_GetError());
		return -1;
	}
	audio_good = Mix_Init(MIX_INIT_FLAC | MIX_INIT_MP3 | MIX_INIT_OGG );
	if(!audio_good)
	{
		printf("Mixer could initialize : %s \n", SDL_GetError());
		return -1;
	}

	if(Mix_OpenAudio(frequency, format, channels, 1024))
	{
		printf("SDL can't initialize audio : %s \n", SDL_GetError());
		return -1;
	}

	audio_good = Mix_QuerySpec(&frequency, &format, &channels );
	if(!audio_good)
	{
		printf("SDL can't initialize audio : %s \n", SDL_GetError());
		return -1;
	}
	printf("Audio Specifications : \n \
		Frequency : %d \n \
		Format: %d \n \
		Channels: %d \n", frequency, format, channels);
	printf("There are %d sample chunk deocoders available \n", Mix_GetNumChunkDecoders());


	Mix_AllocateChannels(16);
	//Set Group Channels
	Mix_GroupChannels(0,4,SOUND_GROUP_PLAYER);
	Mix_GroupChannels(5,10,SOUND_GROUP_OTHER_ENTS);
	Mix_GroupChannels(11,12,SOUND_GROUP_MUSIC);
	Mix_GroupChannels(13,16,SOUND_GROUP_OTHER);

	
	atexit(ShutdownAudioSys);
	return 0;
}

void ShutdownAudioSys()
{
	Mix_CloseAudio();
}

void PauseAllSound()
{
	Mix_Pause(-1);

	//Keep Music Playing
	Mix_Resume(11);
	Mix_Resume(12);
}

void ResumeAllSound()
{
	Mix_Resume(-1);
}

void FreeSound(sound_t* sound)
{
	int sound_num, i;
	
	if( sound->music )
	{
		if(sound->music)
		{
			Mix_FreeMusic(sound->music);
			sound->music = NULL;
		}
	} 
	if( sound->sounds )
	{
		sound_num = CountMem(sound->sounds, sizeof(struct Mix_Chunk));
		for(i = 0; i < sound_num; i++)
		{
			Mix_FreeChunk(&sound->sounds[i]);
		}
		sound = NULL;
	}
	
}

