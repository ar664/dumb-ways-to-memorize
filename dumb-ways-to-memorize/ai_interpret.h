#ifndef __AI_INTERPRET_H
#define __AI_INTERPRET_H

#include "globals.h"
#include "parseobject.h"


#define MAX_AI					255
#define	AI_PATH					"AI/"
#define AI_FUNCTION_OBJECT		"Think"
#define AI_VAR_G_STR			"Variables"
#define AI_TYPE_STR				"AI_Type"
#define AI_TYPE_PRESET_STR		"preset"
#define AI_TYPE_VARIABLE_STR	"variable"
#define AI_TYPE_RECURSIVE_STR	"AI_Recursive"
#define AI_VAR_GRAVITY_STR		"gravity"

#define AI_VAR_STR				"variables"
extern char *gAI_Variables[];   /**< The variables to be parsed for ai data */

extern char *gAI_Actions[];		/**< The actions that are related to ai functions */

#define AI_CHECK_STR			"condition"
extern char *gAI_Conditions[];  /**< The conditions for the ai function to be executed */

#define AI_BASE_JUMP			-5
#define AI_BASE_SPEED			1
#define AI_BASE_THINK_FRAMES	1
#define AI_BASE_DAMAGE			5

/** Defines the enum for ai_types*/
typedef enum
{
	AI_TYPE_NULL,
	AI_TYPE_PRESET = 0x1,   /**< An enum constant representing the ai type preset option */
	AI_TYPE_VARIABLE,   /**< An enum constant representing the ai type variable , which states there needs to be more values given */
	AI_TYPE_RECURSIVE   /**< An enum constant representing the ai type recursive option, which values turn into more functions */
}ai_type_t;

/** Defines the enum for the variables in mVariables */
typedef enum
{
	AI_VAR_NULL,	
	AI_VAR_SPEED,	/**< An enum constant representing the ai variable speed */
	AI_VAR_FRAMES,  /**< An enum constant representing the ai variable for the amount of frames this action happens*/
	AI_VAR_TIME,	/**< An enum constant representing the ai variable for the amount of times to repeat this action */
	AI_VAR_DAMAGE,  /**< An enum constant representing the ai variable for the damage this action should be */
	AI_VAR_DIR_X,   /**< An enum constant representing the ai variable for the x direction */
	AI_VAR_DIR_Y,   /**< An enum constant representing the ai variable for the y direction */
	AI_VAR_CHECK,   /**< An enum constant representing the ai variable for whether there is a condition that needs to be met b4 the action can be done */
	AI_VAR_MAX

}ai_variables_t;

/** Defines the enum for what flags are set on the ai */
typedef enum
{
	AI_FLAG_CHECK_PLAYER = 0x1,
	AI_FLAG_CHECK_OBJECT = 0x2,
	AI_FLAG_GRAVITY	= 0x4,

}ai_flags_t;

/** Defines the enum for action types */
typedef enum
{
	AI_ACTION_NOTHING,
	AI_ACTION_MOVE,
	AI_ACTION_WALK,
	AI_ACTION_JUMP,
	AI_ACTION_ATTACK,
	AI_ACTION_MAX

}ai_actions_t;

typedef enum
{
	AI_CONDITION_PLAYER_DISTANCE,
	AI_CONDITION_OBJECT_DISTANCE,
	AI_CONDITION_OBJECT_NAME,
	AI_CONDITION_LINK_AI,
	AI_CONDITION_LINK_ACTION,
	AI_CONDITION_MAX

}ai_conditions_t;

typedef struct ai_function_s ai_function_t;

/**
 * The structure for ai, pointer to this is declared in each entity.
 * FighterThink is the only think that uses this structure.
 *
 * @author	Anthony Rios
 * @date	3/30/2016
 */

struct ai_function_s
{
	ai_type_t mType;		/**< The type of ai (preset, variable, recursive)*/
	ai_actions_t mAction;   /**< The action type of this function, to get the action think function for entity */
	int mFlags;				/**< The flags defined in ai_flags_t */
	int mVariables[7];  /**< The 7 currently parsed or default values in this function
						1 - speed; 2 - frames; 3 - time; 4 - damage; [5,6] - direction;  7 - check;*/
	char *mObject;			/**< Optional - The object/entity to spawn during an action */
	char *mObjectCheck;		/**< Optional - The object/entity that you need to find, in order for condition to be valid */
	ai_function_t *mLink;   /**< The link to the next ai_function to set */

};

extern ai_function_t *gVariableAIs; /**< The AI's of type variable */
extern ai_function_t *gPresetAIs;	/**< The AI's of type preset */

/**
 * Parses AI behavior using the variables given and the AI obj/file specified.
 *
 * @param [in,out]	obj		 	If non-null, the object.
 * @param [in,out]	g_str	 	If non-null, the string.
 * @param [in,out]	variables	If non-null, the variables.
 *
 * @return	null if it fails, else a pointer to an ai_function_t.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */
ai_function_t *ParseAI(object_t *obj, char *g_str, char **variables);

/**
 * Parse a preset AI, with values determined by whats in the file.
 *
 * @param [in,out]	obj  	If non-null, the object.
 * @param [in,out]	g_str	If non-null, the string.
 *
 * @return	null if it fails, else a pointer to an ai_function_t.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */
ai_function_t *ParsePresetAI(object_t *obj, char *g_str);

/**
 * Sets AI variables in ai_function->mVariables.
 *
 * @param [in,out]	function	If non-null, the function.
 * @param [in,out]	data_str	If non-null, the data string.
 * @param [in,out]	var_type 	If non-null, the variable type.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */
void SetAI_Var(ai_function_t* function, char * data_str, ai_variables_t var_type);

/**
 * Sets AI action defined in ai_actions to the ai_function
 *
 * @param [in,out]	function  	If non-null, the function.
 * @param [in,out]	obj		  	If non-null, the object.
 * @param [in,out]	tok		  	If non-null, the tok.
 * @param [in,out]	g_str	  	If non-null, the string.
 * @param [in,out]	action_type	If non-null, the action type.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */
void SetAI_Action(ai_function_t* function, object_t * obj, jsmntok_t* tok, char* g_str, ai_actions_t action_type);

/**
 * Sets checks that need to occur for the ai_function to be called.
 *
 * @param [in,out]	function	 	If non-null, the function.
 * @param [in,out]	variables_str	If non-null, the variables string.
 * @param [in,out]	data_str	 	If non-null, the data string.
 * @param [in,out]	check_str	 	If non-null, the condition type.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */
void SetAI_Check(ai_function_t* function, char** variables_str, char * data_str, ai_conditions_t condition);

/**
 * Init AI system, similar to Entity init, for now....
 *
 * @return	An int.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */
int InitAISystem();

/**
 * Shutdown AI system, just frees the global variables simple.
 *
 * @author	Anthony Rios
 * @date	3/30/2016
 */

void ShutdownAISystem();

/**
 * Converts a str to an AI type.
 *
 * @param	str	The string.
 *
 * @return	str as an ai_type_t.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */
ai_type_t StrToAI_Type(const char *str);

ai_actions_t StrToAI_Action(const char *str);

ai_conditions_t StrToAI_Condition(const char *str);

ai_variables_t StrToVariableType(const char *str);

#endif