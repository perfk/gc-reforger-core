//------------------------------------------------------------------------------------------------
modded class ACE_Medical_CardiovascularSystem : ACE_Medical_BaseSystem
{
	
	//------------------------------------------------------------------------------------------------
	override protected void Update(IEntity entity, float timeSlice)
	{
		if (!entity)
		{
			Unregister(entity);
			return;
		}
		
		super.Update(entity, timeSlice);
	}
	
}