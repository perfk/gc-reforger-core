modded class PS_PlayableManager
{
	[Attribute("1")]
	bool m_hidePlayersOnSpawn;
	
	override void RegisterPlayable(PS_PlayableComponent playableComponent)
	{
		super.RegisterPlayable(playableComponent);
		
		if(!m_hidePlayersOnSpawn)
			return;
		
		if (Replication.IsServer())
		{
			IEntity entity = playableComponent.GetOwner();
			Physics physics = entity.GetPhysics();
			if (physics)
			{
				physics.EnableGravity(false);
				entity.SetOrigin(entity.GetOrigin() + vector.Up * 100000);
			}
		}
	}
}