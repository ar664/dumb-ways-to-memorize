#ifndef __AUDIO__H
#define __AUDIO__H

#include "SDL_mixer.h"
#define MAX_SOUNDS	100

extern char *SoundVariableNames[];

/** Defines an enum for sound mixer groups */
typedef enum
{
	SOUND_GROUP_PLAYER,		/**<  Channels 0-4 */
	SOUND_GROUP_OTHER_ENTS, /**< Channels 5-10 */
	SOUND_GROUP_MUSIC,		/**< Channels 11-12 */
	SOUND_GROUP_OTHER,		/**< Channels 13-16 */
	SOUND_GROUP_MAX
}sound_mixer_group;

/** Defines an alias representing the enum for different sound effects */
typedef enum
{
	SOUND_EFFECT_NULL,
	SOUND_EFFECT_WALK,
	SOUND_EFFECT_ATTACK,
	SOUND_EFFECT_JUMP,
	SOUND_EFFECT_DIE,
}sound_mixer_effects;

typedef struct sound_s sound_t;

/**
 * The structure that is used for sounds, bot effects and Music
 *
 * @author	Anthony Rios
 * @date	4/8/2016
 */

struct sound_s
{
	sound_mixer_group group_id; /**< Identifier for the sound group */
	Mix_Chunk *sounds;			/**< The sounds to play from */
	Mix_Music *music;			/**< The music , limit this to one */
};

sound_t *LoadSound(char **files, sound_mixer_group group);


int PlaySound(sound_t *sound, sound_mixer_effects number);

sound_mixer_group StrToSoundType(const char *str);

void InitAudioSys();
void ShutdownAudioSys();
void PauseAllSound();
void ResumeAllSound();
void FreeSound(sound_t *sound);

#endif