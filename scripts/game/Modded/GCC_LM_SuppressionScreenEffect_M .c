/*
modded class LM_SuppressionScreenEffect
{	
	override void UpdateEffect(float timeSlice)
	{
		PS_PlayableControllerComponent controller = PS_PlayableControllerComponent.GetPlayableController();
		if(controller.IsSpectating())
		{
			m_wSuppressionVignette.SetMaskProgress(0);
			m_wSuppressionVignette.SetOpacity(0);
			m_wSuppressionFlinch.SetMaskProgress(0);
			m_wSuppressionFlinch.SetOpacity(0);
			s_fBlurriness = 0;
			return;
		}
		
		super.UpdateEffect(timeSlice);
	}

}
*/