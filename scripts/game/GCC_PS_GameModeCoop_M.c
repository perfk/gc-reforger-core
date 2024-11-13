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
}
