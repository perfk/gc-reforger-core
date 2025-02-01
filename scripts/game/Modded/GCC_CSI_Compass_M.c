modded class CSI_Compass : SCR_InfoDisplay
{
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