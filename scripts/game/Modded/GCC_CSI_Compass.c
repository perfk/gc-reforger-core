modded class CSI_Compass : SCR_InfoDisplay
{
	//------------------------------------------------------------------------------------------------
	override protected void UpdateValues(IEntity owner, float timeSlice)
	{
		super.UpdateValues(owner, timeSlice);
		
		m_ChimeraCharacter = SCR_ChimeraCharacter.Cast(GetGame().GetPlayerController().GetControlledEntity());
		
		if (!m_ChimeraCharacter) 
			return;

		if (!m_AuthorityComponent || !m_ClientComponent || !m_GroupsManagerComponent || !m_wBearing || !m_wCompass || !m_PlayerManager) 
		{
			m_AuthorityComponent = CSI_AuthorityComponent.GetInstance();
			m_ClientComponent = CSI_ClientComponent.GetInstance();
			m_GroupsManagerComponent = SCR_GroupsManagerComponent.GetInstance();
			m_wCompass = ImageWidget.Cast(m_wRoot.FindAnyWidget("Compass"));
			m_wBearing = TextWidget.Cast(m_wRoot.FindAnyWidget("Bearing"));
			m_PlayerManager = GetGame().GetPlayerManager();
			return;
		};
		
		// Get players current group.
		m_PlayersGroup = m_GroupsManagerComponent.GetPlayerGroup(SCR_PlayerController.GetLocalPlayerId());

		string compassVisible = m_ClientComponent.ReturnLocalCSISettings()[0];
		string squadRadarVisible = m_ClientComponent.ReturnLocalCSISettings()[1];
		string hudAutoHidden = m_ClientComponent.ReturnLocalCSISettings()[14];

		if (compassVisible == "false" || (hudAutoHidden == "true" && !hudToggled)) 
		{
			if (m_wCompass.GetOpacity() > 0) 
			{
				m_wCompass.SetOpacity(0);
				m_wBearing.SetOpacity(0);
			};
		} else {
			if (m_wCompass.GetOpacity() < 0.625) 
			{
				m_wCompass.SetOpacity(0.625);
				m_wBearing.SetOpacity(0.625);
			
				m_sCompassTexture = "{378C1E24252C207D}UI/Textures/HUD/Modded/Compasses/compass_shadow360_nodirs.edds";
				m_wCompass.LoadImageTexture(0, m_sCompassTexture);
			};
		};
		
		SetBearingAndCompass(compassVisible, hudAutoHidden);

		if ((squadRadarVisible == "false" || (hudAutoHidden == "true" && !hudToggled)) || !m_PlayersGroup) 
			ClearSquadRadar(-1);
		else
			SquadRadarSearch();
	}

	//------------------------------------------------------------------------------------------------
	override protected bool ProcessEntity(IEntity processEntity)
	{
		SCR_ChimeraCharacter playerCharacter = SCR_ChimeraCharacter.Cast(processEntity);
		if (!playerCharacter || playerCharacter == m_ChimeraCharacter) 
			return true;

		if (playerCharacter.GetFactionKey() != m_PlayersGroup.m_faction)
			return true;

		int processEntityID = m_PlayerManager.GetPlayerIdFromControlledEntity(playerCharacter);
		if (!processEntityID || !m_PlayersGroup.IsPlayerInGroup(processEntityID)) 
			return true;

		m_aAllPlayersWithinRange.Insert(playerCharacter);

		return true;
	}
}