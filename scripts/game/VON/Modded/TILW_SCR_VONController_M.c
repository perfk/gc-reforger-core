modded class SCR_VONController : ScriptComponent
{
 	override protected void OnVONChannel(float value, EActionTrigger reason)
	{
		if (reason == EActionTrigger.DOWN)
		{
			if (m_ActiveEntry)
			{
				AudioSystem.TerminateSound(m_AudioHandleRadioSqawk);
				AudioSystem.TerminateSound(m_AudioHandleRadioSqawkRadioSqawkOff);
				m_AudioHandleRadioSqawk = AudioSystem.PlaySound(m_sRadioSound);	
			}
			VONChannel(true);
		}
		else
		{
			if (m_ActiveEntry)
			{
				AudioSystem.TerminateSound(m_AudioHandleRadioSqawk);
				AudioSystem.TerminateSound(m_AudioHandleRadioSqawkRadioSqawkOff);
				m_AudioHandleRadioSqawkRadioSqawkOff = AudioSystem.PlaySound(m_sRadioSoundEnd);
			}
			VONChannel(false);
		}
	}
	
	override protected void OnVONSwitch(float value, EActionTrigger reason)
	{
		vanilla.OnVONSwitch(value, reason);
	}
};
