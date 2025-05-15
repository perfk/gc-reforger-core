modded class PS_CoopLobby
{
	protected TextWidget m_wRatioCounter;
	
	override void OnPlayerConnected(int playerId)
	{
		super.OnPlayerConnected(playerId);
		SetRatio();
	}
	
	override void OnPlayerDisconnected(int playerId)
	{
		super.OnPlayerDisconnected(playerId);
		SetRatio();
	}
	
	protected void SetRatio()
	{
		int totalPlayers = m_PlayerManager.GetPlayerCount();
		array<int> playersPerRatio();
	
		if(m_mFactions.Count() < 2)
		{
			TextWidget ratioHeader = TextWidget.Cast(m_wRoot.FindAnyWidget("RatioHeader"));
			ratioHeader.SetVisible(false);
			return;
		}
		
		int totalSlots = 0;
		foreach (SCR_Faction faction, PS_FactionSelector selector : m_mFactions)
		{
			totalSlots += selector.GetMaxCount();
		}
		
		array<float> ratios = {};
		foreach (SCR_Faction faction, PS_FactionSelector selector : m_mFactions)
		{
			float ratio = selector.GetMaxCount() / totalSlots;
			ratios.Insert(ratio);
		}

		int remainingPlayers = totalPlayers;
		for (int i = 0; i < ratios.Count(); i++)
		{
			int playersForThisRatio = Math.Round((float)ratios[i] * totalPlayers);
			playersPerRatio.Insert(playersForThisRatio);
			
			remainingPlayers -= playersForThisRatio;
		}
		
		if (remainingPlayers > 0)
		{
			int maxIndex = 0;
			for (int i = 1; i < playersPerRatio.Count(); i++)
			{
				if (ratios[i] > ratios[maxIndex])
					maxIndex = i;
			}
			playersPerRatio[maxIndex] = playersPerRatio[maxIndex] + remainingPlayers;
		}
		
		string formattedRatio = "";
		for (int i = 0; i < playersPerRatio.Count(); i++)
		{
			formattedRatio += playersPerRatio[i].ToString();

			if (i < playersPerRatio.Count() - 1)
				formattedRatio += " : ";
		}
		
		m_wRatioCounter.SetText(formattedRatio);
	}
	
	override void OnMenuOpen()
	{
		super.OnMenuOpen();
		
		m_wRatioCounter = TextWidget.Cast(m_wRoot.FindAnyWidget("RatioCounter"));
		SetRatio();
	}
	

	
	

	override void InitPlayables()
	{
		array<PS_PlayableContainer> playables = m_PlayableManager.GetPlayablesSorted();
		map<RplId, ref PS_PlayableVehicleContainer> playableVehicles = m_PlayableManager.GetPlayableVehicles();
		map<SCR_Faction, ref Tuple3<int, int, int>> factions = new map<SCR_Faction, ref Tuple3<int, int, int>>();
		
		
		
		foreach (PS_PlayableContainer playable : playables)
		{
			AddPlayable(playable);
			
			
			int playerId = m_PlayableManager.GetPlayerByPlayable(playable.GetRplId());
			
			if (!PK_MyData.IsIDProcessed(playable.GetRplId())) {
				PK_MyData.MarkIDProcessed(playable.GetRplId());
				m_PlayableControllerComponent.SetPlayablePlayer(playable.GetRplId(), -2);
				playerId = -2;
			}
			
			
			int playerAdded = 0;
			int playerAddedMax = 1;
			int playerAddedLocked = 0;
			if (playerId >= 0)
				playerAdded = 1;
			if (playerId == -2)
				playerAddedLocked = 1;


			
			
			SCR_Faction faction = playable.GetFaction();
			if (!factions.Contains(faction))
			{
				factions.Insert(faction, new Tuple3<int, int, int>(playerAdded, playerAddedMax, playerAddedLocked));
			}
			else
			{
				Tuple3<int, int, int> tuple = factions.Get(faction);
				tuple.param1 += playerAdded;
				tuple.param2 += playerAddedMax;
				tuple.param3 += playerAddedLocked;
			}
			

		}


		
			
		
		foreach (RplId rplId, PS_PlayableVehicleContainer playableVehicleContainer : playableVehicles)
		{
			AddPlayableVehicle(playableVehicleContainer);
		}
		
		foreach (SCR_Faction faction, Tuple3<int, int, int> count : factions)
		{
			AddFaction(faction, count.param1, count.param2, count.param3);
		}
		
		// Added in runtime
		m_PlayableManager.GetOnPlayableRegistered().Remove(OnPlayableRegistered);
		m_PlayableManager.GetOnPlayableRegistered().Insert(OnPlayableRegistered);
	}
	
	
	
	
}