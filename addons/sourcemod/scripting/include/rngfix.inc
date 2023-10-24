#if defined _rngfix_included
	#endinput
#endif
#define _rngfix_included

/**
 * Called when trigger_* is Hooked.
 * 
 * @param entity    	    Entity
 * @param classname     	Clasname
 * @noreturn
 */
forward void FixRNG_OnTriggerDetected(int entity, const char[] classname);

/**
 * Called when trigger_* on StartTouch.
 * 
 * @param entity    	    Entity
 * @param other     	    Activator
 * @noreturn
 */
forward void FixRNG_OnTriggerStartTouch(int entity, int other);

/**
 * Called when trigger_* on EndTouch.
 * 
 * @param entity    	    Entity
 * @param other     	    Activator
 * @noreturn
 */
forward void FixRNG_OnTriggerEndTouch(int entity, int other);

/**
 * Called after trigger_teleport is touched.
 * 
 * @param entity    	    Entity
 * @param other     	    Activator
 * @noreturn
 */
forward void FixRNG_OnTriggerTeleportTouchPost(int entity, int other);

public SharedPlugin __pl_rngfix =
{
	name = "rngfix",
	file = "rngfix.smx",
	#if defined REQUIRE_PLUGIN
	required = 1
	#else
	required = 0
	#endif
};