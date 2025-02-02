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
		if (m_iPlayerId > 0 && playerId != m_iPlayerId)
		{
			AudioSystem.PlaySound("{C97850E4341F0CF9}Sounds/UI/Samples/Menu/UI_Button_Fail.wav");
			return;
		}
	
		SCR_EGameModeState gameState = m_GameModeCoop.GetState();
		if (!PS_PlayersHelper.IsAdminOrServer())
		{
			RplId playableId = m_PlayableManager.GetPlayableByPlayer(m_iCurrentPlayerId);
			if (gameState == SCR_EGameModeState.BRIEFING && playableId != RplId.Invalid())
				return;
		}
		
		if (playerId != m_iPlayerId)
		{
			if (!CanJoinFaction())
			{
				SCR_ChatPanelManager chatPanelManager = SCR_ChatPanelManager.GetInstance();
				ChatCommandInvoker invoker = chatPanelManager.GetCommandInvoker("lmsg");
				invoker.Invoke(null, "Где баланс?");
				return;
			}
			AudioSystem.PlaySound("{9500A96BBA3B0581}Sounds/UI/Samples/Menu/UI_Gadget_Select.wav");
			// Disable group von room moving
			//m_PlayableControllerComponent.MoveToVoNRoom(playerId, m_sFactionKey, m_sPlayableCallsign);
			if (gameState == SCR_EGameModeState.BRIEFING)
				m_PlayableControllerComponent.MoveToVoNRoom(playerId, m_sFactionKey, "#PS-VoNRoom_Command");
			
			m_PlayableControllerComponent.ChangeFactionKey(playerId, m_sFactionKey);
			m_PlayableControllerComponent.SetPlayerState(playerId, PS_EPlayableControllerState.NotReady);	
			m_PlayableControllerComponent.SetPlayerPlayable(playerId, m_iPlayableId);
		} else {
			AudioSystem.PlaySound("{9500A96BBA3B0581}Sounds/UI/Samples/Menu/UI_Gadget_Select.wav");
			// Disable faction von room moving (unclicking a slot)
			//m_PlayableControllerComponent.MoveToVoNRoom(playerId, m_sFactionKey, "#PS-VoNRoom_Faction");
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
		SCR_EGameModeState gameState = m_GameModeCoop.GetState();
		switch (m_state)
		{
			case PS_ECharacterState.Pin:
				AudioSystem.PlaySound("{63DB9ACDD10DB801}Sounds/UI/Samples/Editor/UI_E_Layer_Edit_Back.wav");
				m_PlayableControllerComponent.UnpinPlayer(m_iPlayerId);
				break;
			case PS_ECharacterState.Dead:
				break;
			case PS_ECharacterState.Lock:
				AudioSystem.PlaySound("{E495F2DA6A44D0BB}Sounds/UI/Samples/Menu/UI_Button_Filter_Off.wav");
				m_PlayableControllerComponent.SetPlayablePlayer(m_iPlayableId, -1);
				break;
			case PS_ECharacterState.Kick:
				AudioSystem.PlaySound("{5EF75EB4A836831F}Sounds/Explosions/_SharedData/Bodies/Explosion_Body_TNT_Far_01.wav");
				//m_PlayableControllerComponent.MoveToVoNRoom(m_iPlayerId, m_sFactionKey, "#PS-VoNRoom_Faction");
				if (gameState == SCR_EGameModeState.BRIEFING)
					m_PlayableControllerComponent.MoveToVoNRoom(m_iPlayerId, m_sFactionKey, "#PS-VoNRoom_Command");
			
				if (gameState == SCR_EGameModeState.SLOTSELECTION)
					m_PlayableControllerComponent.MoveToVoNRoom(m_iPlayerId, m_sFactionKey, "#PS-VoNRoom_Global");
				
				m_PlayableControllerComponent.ChangeFactionKey(m_iPlayerId, "");
				m_PlayableControllerComponent.SetPlayerState(m_iPlayerId, PS_EPlayableControllerState.NotReady);
				m_PlayableControllerComponent.SetPlayerPlayable(m_iPlayerId, -1);
				if (PS_PlayersHelper.IsAdminOrServer())
					m_PlayableControllerComponent.UnpinPlayer(m_iPlayerId);
				break;
			case PS_ECharacterState.Empty:
				AudioSystem.PlaySound("{B6008DBCA565E5E1}Sounds/UI/Samples/Menu/UI_Button_Filter_On.wav");
				if (m_iPlayerId > 0)
					m_PlayableControllerComponent.SetPlayerState(m_iPlayerId, PS_EPlayableControllerState.NotReady);
				m_PlayableControllerComponent.SetPlayablePlayer(m_iPlayableId, -2);
			   break;
			case PS_ECharacterState.Player:
				break;
			case PS_ECharacterState.Disconnected:
				AudioSystem.PlaySound("{5EF75EB4A836831F}Sounds/Explosions/_SharedData/Bodies/Explosion_Body_TNT_Far_01.wav");
				m_PlayableControllerComponent.MoveToVoNRoom(m_iPlayerId, m_PlayableManager.GetPlayerFactionKey(m_iPlayerId), "#PS-VoNRoom_Faction");
				m_PlayableControllerComponent.ChangeFactionKey(m_iPlayerId, "");
				m_PlayableControllerComponent.SetPlayerPlayable(m_iPlayerId, RplId.Invalid());
				break;
		}
	}
}