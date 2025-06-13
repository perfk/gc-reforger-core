class GC_BushCollisionEffect : SCR_SpecialCollisionDamageEffect
{
	[Attribute(defvalue: "1", uiwidget: UIWidgets.Auto, desc: "Time in seconds to slow max", params: "0 9999 0.001")]
	protected float m_timeToMaxSlow;	

	protected float m_timeActive = 0;
	protected float m_currentLimit;
	
	override event void EOnFrame(float timeSlice, SCR_ExtendedDamageManagerComponent dmgManager)
	{
		m_timeActive += timeSlice;
		
		ApplyMobilityLimits(dmgManager);
	}

	override void OnEffectAdded(SCR_ExtendedDamageManagerComponent dmgManager);
	
	override void OnEffectRemoved(SCR_ExtendedDamageManagerComponent dmgManager)
	{
		ResetMobility(dmgManager);
	}

	override void ApplyMobilityLimits(SCR_ExtendedDamageManagerComponent dmgManager)
	{
		ChimeraCharacter character = ChimeraCharacter.Cast(dmgManager.GetOwner());
		if (!character)
		{
			Terminate();
			return;
		}
		
		m_Controller = SCR_CharacterControllerComponent.Cast(character.GetCharacterController());
		if (!m_Controller)
		{
			Terminate();
			return;
		}
		
		float multiplier = Math.Clamp(m_timeActive / m_timeToMaxSlow, 0.0, 1.0);
        float limit = Math.Lerp(1.0, m_fSpecialCollisionMaxSpeedLimitScaled, multiplier);
		if(limit == m_currentLimit)
			return;
		m_currentLimit = limit;
		
		array<ref SCR_PersistentDamageEffect> effects = {};
		effects = dmgManager.GetAllPersistentEffectsOfType(GC_BushCollisionEffect, true);
		foreach (SCR_PersistentDamageEffect effect : effects)
		{
			GC_BushCollisionEffect bushEffect = GC_BushCollisionEffect.Cast(effect);
			if (!bushEffect)
				continue;
			
			if (m_currentLimit > bushEffect.GetCurrentLimit())
				return;
		}

		m_Controller.OverrideMaxSpeed(m_currentLimit);
	}
	
	void ResetMobility(SCR_ExtendedDamageManagerComponent dmgManager)
	{
		ChimeraCharacter character = ChimeraCharacter.Cast(dmgManager.GetOwner());
		if (!character)
			return;
		
		m_Controller = SCR_CharacterControllerComponent.Cast(character.GetCharacterController());
		if (!m_Controller)
			return;
		
		array<ref SCR_PersistentDamageEffect> effects = {};
		effects = dmgManager.GetAllPersistentEffectsOfType(GC_BushCollisionEffect, true);
		if(effects.IsEmpty())
			m_Controller.OverrideMaxSpeed(1);

		m_currentLimit = 1;
		m_timeActive = 0;
	}
	
	float GetCurrentLimit()
	{
		return m_currentLimit;
	}
}
