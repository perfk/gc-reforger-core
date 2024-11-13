// Delete after next stable RL update
modded class PS_VoiceRoomHeader : SCR_ButtonBaseComponent
{
	override void UpdateInfo()
	{
		PS_VoNRoomsManager VoNRoomsManager = PS_VoNRoomsManager.GetInstance();
		PS_PlayableManager playableManager = PS_PlayableManager.GetInstance();
		PlayerController playerController = GetGame().GetPlayerController();
		int playerId = playerController.GetPlayerId();
		
		if (VoNRoomsManager.GetPlayerRoom(playerId) == m_iRoomId) {
			m_wJoinRoomImage.SetVisible(false);
			return;
		}
		m_wJoinRoomImage.SetVisible(true);
		
		// Bad hardcoded staff here
		if (m_sRoomName == "#PS-VoNRoom_Command")
		{
			PS_GameModeCoop gamemode = PS_GameModeCoop.Cast(GetGame().GetGameMode());
			if (!playableManager.IsPlayerGroupLeader(playerId) && !gamemode.m_factionBriefings) m_wJoinRoomImage.LoadImageFromSet(0, m_sImageSetPS, "Lock");
			else m_wJoinRoomImage.LoadImageFromSet(0, m_sImageSetPS, "RoomEnter");
			return;
		}
		
		m_wJoinRoomImage.LoadImageFromSet(0, m_sImageSetPS, "RoomEnter");
	}

	// -------------------- Buttons events --------------------
	override void JoinRoomButtonClicked(SCR_ButtonBaseComponent joinRoomButton)
	{
		PS_PlayableManager playableManager = PS_PlayableManager.GetInstance();
		
		PlayerController playerController = GetGame().GetPlayerController();
		int playerId = playerController.GetPlayerId();
		PS_PlayableControllerComponent playableController = PS_PlayableControllerComponent.Cast(playerController.FindComponent(PS_PlayableControllerComponent));
		
		// Bad hardcoded staff here
		if (m_sRoomName == "#PS-VoNRoom_Command")
		{
			PS_GameModeCoop gamemode = PS_GameModeCoop.Cast(GetGame().GetGameMode());
			if (!playableManager.IsPlayerGroupLeader(playerId) && !gamemode.m_factionBriefings)
				return;
		}
		
		FactionKey factionKey = "";
		if (m_fFaction) factionKey = m_fFaction.GetFactionKey();
		playableController.MoveToVoNRoom(playerId, factionKey, m_sRoomName);
	}
}