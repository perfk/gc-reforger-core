// Delete after next stable RL update
modded class PS_GameModeCoop : SCR_BaseGameMode
{
	override void OnGameStart()
	{
		super.OnGameStart();
		GetOnPlayerConnected().Insert(TILW_OnPlayerConnected);
	}

	protected void TILW_OnPlayerConnected(int playerId)
	{
		GetGame().GetCallqueue().CallLater(ShowJIPInfo, 5 * 1000, false, playerId);
	}
	
	protected void ShowJIPInfo(int playerId)
	{
		if (GetState() != SCR_EGameModeState.GAME || PS_PlayableManager.GetInstance().GetPlayableByPlayer(playerId) != RplId.Invalid()) return;
		
		SCR_PlayerController pc = SCR_PlayerController.TILW_GetPCFromPID(playerId);
		if (!pc) return;
		
		if (m_bTeamSwitch && !m_bRemoveRedundantUnits) pc.TILW_ShowJIPInfo();
		else pc.TILW_SendHintToPlayer("JIP Not Available", "This mission does not support join-in-progress, please wait until the next mission starts.", 10);
	}
	
	override void OnGameStateChanged()
	{
		super.OnGameStateChanged();
		
		SetupTimer();
	}
	
	int m_briefingTimer = 0;
	protected void SetupTimer()
	{
		SCR_EGameModeState state = GetState();
		if(state == SCR_EGameModeState.GAME)
			return GetGame().GetCallqueue().Remove(UpdateTimer);
		
		if(state != SCR_EGameModeState.BRIEFING)
			return;

		GetGame().GetCallqueue().CallLater(UpdateTimer, 1000, true);
	}
	
	protected void UpdateTimer()
	{
		m_briefingTimer++;
	}
	
	override bool RplSave(ScriptBitWriter writer)
	{
		super.RplSave(writer);
		
		writer.WriteInt(m_briefingTimer);
		
		return true;
	}

	override bool RplLoad(ScriptBitReader reader)
	{
		super.RplLoad(reader);
		
		reader.ReadInt(m_briefingTimer);

		return true;
	}
}