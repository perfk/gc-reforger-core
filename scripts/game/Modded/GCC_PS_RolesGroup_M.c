modded class PS_RolesGroup
{
	override void OpenContext()
	{
		PS_ContextMenu contextMenu = PS_ContextMenu.CreateContextMenuOnMousePosition(m_CoopLobby.GetRootWidget(), m_wRolesGroupName.GetText());
		contextMenu.ActionJoinVoice().Insert(OnClickedVoiceJoin);
		if (PS_PlayersHelper.IsAdminOrServer() || m_PlayableManager.IsPlayerCompanyLeader(m_iCurrentPlayerId))
		{
			if (m_iLockedCount != m_mCharacters.Count())
				contextMenu.ActionLock(0).Insert(OnClickedLock);
			else
				contextMenu.ActionUnlock(1).Insert(OnClickedLock);
		}
	}
	
	// --------------------------------------------------------------------------------------------------------------------------------
	// Buttons

	
	
	override void OnClickedLock(PS_ContextAction contextAction, PS_ContextActionDataPlayable contextActionDataPlayable)
	{
		m_bInnerButtonClicked = true;
		
		
		Print("Perfk: IsPlayerCompanyLeader" + m_PlayableManager.IsPlayerCompanyLeader(m_iCurrentPlayerId) + " - " + m_iCurrentPlayerId);
		
		//Print("Perfk:FactioNey: " + m_PlayableManager.GetFactionKey(m_iCurrentPlayerId));
		//if (!PS_PlayersHelper.IsAdminOrServer() && !m_PlayableManager.IsPlayerCompanyLeader(m_iCurrentPlayerId))
		if (!m_PlayableManager.IsPlayerCompanyLeader(m_iCurrentPlayerId))
			return;
		
//Print("Perfk Faction: "+ contextActionDataPlayable.ToString());
	//if (!PS_PlayersHelper.IsAdminOrServer() && !m_PlayableManager.IsPlayerCompanyLeader(m_iCurrentPlayerId) && !m_PlayableManager.GetPlayerFactionKey(m_iCurrentPlayerId) == true)
		
		
		
		bool unlock = contextActionDataPlayable.GetPlayableId();
		foreach (PS_PlayableContainer playable, PS_CharacterSelector characterSelector : m_mCharacters)
		{
			int playerId = characterSelector.GetPlayerId();
			if (unlock)
			{
				if (playerId == -2)
				{
					m_PlayableControllerComponent.SetPlayablePlayer(playable.GetRplId(), -1);
				}
			}
			else
			{
				if (playerId != -2)
				{
					PS_EPlayableControllerState state = m_PlayableManager.GetPlayerState(playerId);
					if (state == PS_EPlayableControllerState.Ready)
						m_PlayableControllerComponent.SetPlayerState(playerId, PS_EPlayableControllerState.NotReady);
					m_PlayableControllerComponent.SetPlayablePlayer(playable.GetRplId(), -2);
				}
			}
		}
		
		foreach (PS_PlayableVehicleContainer vehicle, PS_VehicleSelector vehicleSelector : m_mVehicles)
		{
			if (unlock)
				vehicleSelector.UnlockVehicle(null, new PS_ContextActionDataPlayable(vehicle.GetRplId()));
			else
				vehicleSelector.LockVehicle(null, new PS_ContextActionDataPlayable(vehicle.GetRplId()));
		}
		
		if (unlock) {
			m_wLockImage.LoadImageFromSet(0, m_sImageSet, "server-unlocked");
			m_iLockedCount = 0;
			SCR_UISoundEntity.SoundEvent("SOUND_FE_BUTTON_FILTER_OFF");
		} else {
			m_wLockImage.LoadImageFromSet(0, m_sImageSet, "server-locked");
			m_iLockedCount = m_mCharacters.Count();
			SCR_UISoundEntity.SoundEvent("SOUND_FE_BUTTON_FILTER_ON");
		}
	}
	
}






