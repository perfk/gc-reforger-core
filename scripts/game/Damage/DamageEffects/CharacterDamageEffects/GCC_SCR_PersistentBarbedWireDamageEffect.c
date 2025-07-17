modded class SCR_PersistentBarbedWireDamageEffect : SCR_SpecialCollisionDamageEffect
{
	//------------------------------------------------------------------------------------------------
	override void OnEffectAdded(SCR_ExtendedDamageManagerComponent dmgManager)
	{
		if (!EntityUtils.IsPlayer(dmgManager.GetOwner()))
			return;
		
		super.OnEffectAdded(dmgManager);
	}
}
