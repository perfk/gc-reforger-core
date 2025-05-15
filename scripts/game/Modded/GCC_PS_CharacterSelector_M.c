modded class PS_CharacterSelector
{
	override void OnClicked(SCR_ButtonBaseComponent button)
	{
		if (m_bStateClickSkip)
		{
			m_bStateClickSkip = false;
			return;
		}
		
		int playerId = m_CoopLobby.GetSelectedPlayer();
		if (m_iPlayerId == -2)
		{
			m_CoopLobby.SetPreviewPlayable(m_iPlayableId, true);
			SCR_UISoundEntity.SoundEvent("SOUND_FE_BUTTON_FAIL");
			return;
		}
		if (m_iPlayerId > 0 && playerId != m_iPlayerId)
		{
			m_CoopLobby.SetPreviewPlayable(m_iPlayableId, true);
			SCR_UISoundEntity.SoundEvent("SOUND_FE_BUTTON_FAIL");
			return;
		}
		PS_PlayableContainer playableContainer = m_PlayableManager.GetPlayableById(m_iPlayableId);
		if (playableContainer.GetDamageState() == EDamageState.DESTROYED)
		{
			m_CoopLobby.SetPreviewPlayable(m_iPlayableId, true);
			SCR_UISoundEntity.SoundEvent("SOUND_FE_BUTTON_FAIL");
			return;
		}
	
		SCR_EGameModeState gameState = m_GameModeCoop.GetState();
		if (!PS_PlayersHelper.IsAdminOrServer())
		{
			RplId playableId = m_PlayableManager.GetPlayableByPlayer(m_iCurrentPlayerId);
			if (gameState == SCR_EGameModeState.BRIEFING && playableId != RplId.Invalid())
			{
				m_CoopLobby.SetPreviewPlayable(m_iPlayableId, true);
				return;
			}
		}
		
		if (playerId != m_iPlayerId)
		{
			if (!CanJoinFaction())
			{
				SCR_ChatPanelManager chatPanelManager = SCR_ChatPanelManager.GetInstance();
				ChatCommandInvoker invoker = chatPanelManager.GetCommandInvoker("lmsg");
				invoker.Invoke(null, "Где баланс?");
				m_CoopLobby.SetPreviewPlayable(m_iPlayableId, true);
				return;
			}
			
			SCR_UISoundEntity.SoundEvent("SOUND_HUD_GADGET_SELECT");
			// GCC m_PlayableControllerComponent.MoveToVoNRoom(playerId, m_sFactionKey, m_sPlayableCallsign);
			
			// GCC
			if (gameState == SCR_EGameModeState.BRIEFING)
				m_PlayableControllerComponent.MoveToVoNRoom(playerId, m_sFactionKey, "#PS-VoNRoom_Command");
			// GCC
			
			m_PlayableControllerComponent.ChangeFactionKey(playerId, m_sFactionKey);
			m_PlayableControllerComponent.SetPlayerState(playerId, PS_EPlayableControllerState.NotReady);	
			m_PlayableControllerComponent.SetPlayerPlayable(playerId, m_iPlayableId);
		} else {
			SCR_UISoundEntity.SoundEvent("SOUND_HUD_GADGET_SELECT");
			// GCC m_PlayableControllerComponent.MoveToVoNRoom(playerId, m_sFactionKey, "#PS-VoNRoom_Faction");
			m_PlayableControllerComponent.ChangeFactionKey(playerId, "");
			m_PlayableControllerComponent.SetPlayerState(playerId, PS_EPlayableControllerState.NotReady);
			m_PlayableControllerComponent.SetPlayerPlayable(playerId, RplId.Invalid());
			if (PS_PlayersHelper.IsAdminOrServer())
				m_PlayableControllerComponent.UnpinPlayer(playerId);
		}
		
		if (PS_PlayersHelper.IsAdminOrServer() && playerId != m_iCurrentPlayerId && gameState == SCR_EGameModeState.GAME)
			m_PlayableControllerComponent.ForceSwitch(playerId);
		if (!PS_PlayersHelper.IsAdminOrServer() && playerId == m_iCurrentPlayerId && gameState == SCR_EGameModeState.BRIEFING)
			m_PlayableControllerComponent.SwitchToMenuServer(SCR_EGameModeState.BRIEFING);
	}

	override void OnStateClicked(SCR_ButtonBaseComponent button)
	{
		m_bStateClickSkip = true;
		switch (m_state)
		{
			case PS_ECharacterState.Pin:
				SCR_UISoundEntity.SoundEvent("SOUND_E_LAYER_BACK");
				m_PlayableControllerComponent.UnpinPlayer(m_iPlayerId);
				break;
			case PS_ECharacterState.Dead:
				break;
			case PS_ECharacterState.Lock:
				SCR_UISoundEntity.SoundEvent("SOUND_FE_BUTTON_FILTER_OFF");
				m_PlayableControllerComponent.SetPlayablePlayer(m_iPlayableId, -1);
				break;
			case PS_ECharacterState.Kick:
				SCR_UISoundEntity.SoundEvent("SOUND_LOBBY_KICK");
				// GCC m_PlayableControllerComponent.MoveToVoNRoom(m_iPlayerId, m_sFactionKey, "#PS-VoNRoom_Faction");
			
				// GCC Move to global / command instead
				SCR_EGameModeState gameState = m_GameModeCoop.GetState();
				if (gameState == SCR_EGameModeState.BRIEFING)
					m_PlayableControllerComponent.MoveToVoNRoom(m_iPlayerId, m_sFactionKey, "#PS-VoNRoom_Command");
	                        
				if (gameState == SCR_EGameModeState.SLOTSELECTION)
	            		m_PlayableControllerComponent.MoveToVoNRoom(m_iPlayerId, m_sFactionKey, "#PS-VoNRoom_Global");
				// GCC
                                
				m_PlayableControllerComponent.ChangeFactionKey(m_iPlayerId, "");
				m_PlayableControllerComponent.SetPlayerState(m_iPlayerId, PS_EPlayableControllerState.NotReady);
				m_PlayableControllerComponent.SetPlayerPlayable(m_iPlayerId, -1);
				if (PS_PlayersHelper.IsAdminOrServer())
					m_PlayableControllerComponent.UnpinPlayer(m_iPlayerId);
				break;
			case PS_ECharacterState.Empty:
				SCR_UISoundEntity.SoundEvent("SOUND_FE_BUTTON_FILTER_ON");
				if (m_iPlayerId > 0)
					m_PlayableControllerComponent.SetPlayerState(m_iPlayerId, PS_EPlayableControllerState.NotReady);
				m_PlayableControllerComponent.SetPlayablePlayer(m_iPlayableId, -2);
			   break;
			case PS_ECharacterState.Player:
				break;
			case PS_ECharacterState.Disconnected:
				SCR_UISoundEntity.SoundEvent("SOUND_LOBBY_KICK");
				m_PlayableControllerComponent.MoveToVoNRoom(m_iPlayerId, m_PlayableManager.GetPlayerFactionKey(m_iPlayerId), "#PS-VoNRoom_Faction");
				m_PlayableControllerComponent.ChangeFactionKey(m_iPlayerId, "");
				m_PlayableControllerComponent.SetPlayerPlayable(m_iPlayerId, RplId.Invalid());
				break;
		}
	}
	
	// --------------------------------------------------------------------------------------------------------------------------------
	// Context menu
	override void OpenContext()
	{
		string playerName = PS_PlayableManager.GetInstance().GetPlayerName(m_iPlayerId);
		PS_ContextMenu contextMenu = PS_ContextMenu.CreateContextMenuOnMousePosition(m_CoopLobby.GetRootWidget(), playerName);
		contextMenu.ActionOpenInventory(m_iPlayableId).Insert(OnActionOpenInventory);
		
		if (m_iPlayerId > 0)
		{
			if (PS_PlayersHelper.IsAdminOrServer())
			{
				contextMenu.ActionGetArmaId(m_iPlayerId);
			}
			if (m_iPlayerId != m_iCurrentPlayerId)
			{
				PermissionState mute = PermissionState.DISALLOWED;
				SocialComponent socialComp = SocialComponent.Cast(GetGame().GetPlayerController().FindComponent(SocialComponent));
				if (socialComp.IsMuted(m_iPlayerId))
					contextMenu.ActionUnmute(m_iPlayerId);
				else
					contextMenu.ActionMute(m_iPlayerId);
				
				if (m_bCanKick && m_iPlayerId >= 0 && m_iPlayerId != m_iCurrentPlayerId)
					contextMenu.ActionFreeSlot(m_iPlayableId).Insert(OnActionFreeSlot);
				
				if (PS_PlayersHelper.IsAdminOrServer())
				{
					contextMenu.ActionDirectMessage(m_iPlayerId);
					contextMenu.ActionKick(m_iPlayerId);
					
					if (m_PlayableManager.GetPlayerPin(m_iPlayerId))
						contextMenu.ActionUnpin(m_iPlayerId);
					else
						contextMenu.ActionPin(m_iPlayerId);
				}
			}
			if (m_CoopLobby.GetSelectedPlayer() != m_iPlayerId && PS_PlayersHelper.IsAdminOrServer())
			{
				contextMenu.ActionPlayerSelect(m_iPlayerId);
			}
		}
		
		
		if (m_PlayableManager.IsPlayerGroupLeader(m_iCurrentPlayerId) || PS_PlayersHelper.IsAdminOrServer())
			/*Print("Perfk: m_iCurrentPlayerId:"+ m_iCurrentPlayerId + "isadmin:" + PS_PlayersHelper.IsAdminOrServer());
			Print("Perfk: " + m_PlayableManager.GetPlayerFactionKey(m_iCurrentPlayerId) + " -- " + m_Faction.GetFactionKey());
			Print("Perfk print :" + m_PlayableManager.IsPlayerCompanyLeader(m_iCurrentPlayerId));*/
			if(PS_PlayersHelper.IsAdminOrServer())
				if (m_iPlayerId != -2)
					contextMenu.ActionLock(m_iPlayableId).Insert(OnActionLock);
				else
					contextMenu.ActionUnlock(m_iPlayableId).Insert(OnActionUnlock);
			else
				if(m_PlayableManager.GetPlayerFactionKey(m_iCurrentPlayerId) == m_Faction.GetFactionKey() && m_PlayableManager.IsPlayerCompanyLeader(m_iCurrentPlayerId))
					if (m_iPlayerId != -2)
						contextMenu.ActionLock(m_iPlayableId).Insert(OnActionLock);
					else
						contextMenu.ActionUnlock(m_iPlayableId).Insert(OnActionUnlock);
	}
	
}