modded class ServerAdminTools_Stats
{
	override bool Save(string path, bool statsSaveConnectedPlayers)
	{
		m_stats.Set("fps", System.GetFPS());
		m_stats.Set("uptime_seconds", System.GetTickCount() / 1000);
		m_stats.Set("updated", System.GetUnixTime());
		
		TryGetPlayerCount();
		TryGetAICharacters();
		TryGetEditableEntities();
		
		// if fps is <15, fire an event
		if (apiEventEnabled && m_stats.Get("fps") < 15)
		{
			ServerAdminTools_EventsApiServerFPSLowEvent.Event(m_stats.Get("fps"), m_stats.Get("players"), m_stats.Get("ai_characters"));
		}
		
		SCR_JsonSaveContext context = new SCR_JsonSaveContext();

		for (int x = 0; x < m_stats.Count(); x++)
		{
			string key = m_stats.GetKey(x);
			int value = m_stats.Get(key);
			
			context.WriteValue(key, value);
		};

		/*
		if (logPlayerCoordinates)
		{
			TrySavePlayerCoords();
			context.WriteValue("player_coords", player_coords);
		};
		*/
		
		if (statsSaveConnectedPlayers) {
			TrySaveConnectedPlayers();
			context.WriteValue("connected_players", m_connectedPlayers);
		}
		
		context.WriteValue("events", m_events);
		
		SCR_MissionHeader missionHeader = SCR_MissionHeader.Cast(GetGame().GetMissionHeader());
		if (missionHeader)
		{
			context.WriteValue("mission", missionHeader.m_sName);
		}
		
		return context.SaveToFile(path);
	};
}