//! Script entry for garbage system modding
modded class SCR_GarbageSystem : GarbageSystem
{
	override protected float OnInsertRequested(IEntity entity, float lifetime)
	{
		// Prevent insertion of playable characters
		if (SCR_ChimeraCharacter.Cast(entity))
		{
			Managed m = entity.FindComponent(PS_PlayableComponent);
			if (m)
			{
				PS_PlayableComponent plC = PS_PlayableComponent.Cast(m);
				if (plC.GetPlayable())
					return -1;
			}
		}

		return super.OnInsertRequested(entity, lifetime);
	}
}