modded class PS_LobbyVoNComponent
{
	
	void PS_LobbyVoNComponent(IEntityComponentSource src, IEntity ent, IEntity parent)
	{
		GetGame().GetCallqueue().CallLater(DisablePhysicForOwner2, 0, false, ent);
	}
	
	void DisablePhysicForOwner2(IEntity owner)
	{
		Physics physics = owner.GetPhysics();
		if (physics)
		{
			physics.SetVelocity("0 0 0");
			physics.SetAngularVelocity("0 0 0");
			physics.SetMass(0);
			physics.SetDamping(1, 1);
			physics.EnableGravity(false);
			physics.SetActive(ActiveState.INACTIVE);
			physics.SetInteractionLayer(EPhysicsLayerDefs.Unused);
		}
	}
}