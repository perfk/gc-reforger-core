// Delete after next stable RL update
modded class PS_GameModeCoop : SCR_BaseGameMode
{
	
	[Attribute("1", UIWidgets.CheckBox, "Every member is moved to their factions HQ channel for a common briefing, instead of just leaders. GC Core feature.", category: "Reforger Lobby")]
	bool m_factionBriefings;
	
	override void OnGameStateChanged()
	{
		super.OnGameStateChanged();
		
		if (m_factionBriefings && GetState() == SCR_EGameModeState.BRIEFING)
		{
			array<int> playerIds = new array<int>();
			GetGame().GetPlayerManager().GetPlayers(playerIds);
			
			PS_VoNRoomsManager VoNRoomsManager = PS_VoNRoomsManager.GetInstance();
			PS_PlayableManager playableManager = PS_PlayableManager.GetInstance();
			
			foreach (int playerId: playerIds)
			{
				RplId playableId = playableManager.GetPlayableByPlayer(playerId);
				if (playableId != RplId.Invalid()) VoNRoomsManager.MoveToRoom(playerId, playableManager.GetPlayerFactionKey(playerId), "#PS-VoNRoom_Command");
			}
		}
	}
	
	override void OnGameStart()
	{
		super.OnGameStart();
		GetOnPlayerConnected().Insert(TILW_OnPlayerConnected);
	}

	protected void TILW_OnPlayerConnected(int playerId)
	{
		GetGame().GetCallqueue().CallLater(ShowJIPInfo, 15 * 1000, false, playerId);
	}
	
	protected void ShowJIPInfo(int playerId)
	{
		if (GetState() != SCR_EGameModeState.GAME || PS_PlayableManager.GetInstance().GetPlayableByPlayer(playerId) != RplId.Invalid()) return;
		
		SCR_PlayerController pc = SCR_PlayerController.TILW_GetPCFromPID(playerId);
		if (!pc) return;
		
		if (m_bTeamSwitch && !m_bRemoveRedundantUnits) pc.TILW_ShowJIPInfo();
		else pc.TILW_SendHintToPlayer("JIP Not Available", "This mission does not support join-in-progress, please wait until the next mission starts.", 10);
	}
}
