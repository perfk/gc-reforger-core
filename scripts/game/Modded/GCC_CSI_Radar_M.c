modded class CSI_Radar
{
	override void Update()
	{			
		SCR_ChimeraCharacter localCharacter = m_HUDManager.GetLocalPlayerCharacter();
		if (!localCharacter)
			return;
		
		int groupCount = m_HUDManager.GetLocalGroupCount();
        float localYaw = m_HUDManager.GetLocalYaw();
		vector localOrigin = localCharacter.GetOrigin();

		if (groupCount > 1 && m_SettingsManager.GetSettingBool(CSI_GameSettings.RADAR_VISIBLE))
		{
			foreach (int i, int playerID : m_HUDManager.GetLocalGroupPlayerIds())
			{
				float x, y, opacity, rotation, disT, dis, searchRadius;
				SCR_ChimeraCharacter playerCharacter = SCR_ChimeraCharacter.Cast(GetGame().GetPlayerManager().GetPlayerControlledEntity(playerID));
				
				if (!playerCharacter)
				{
					UpdatePlayerRadarWidget(i, -1, 1, 0, 0, 0, 0);
					continue;
				};
				
				if (playerCharacter.GetPrefabData().GetPrefabName() == "{ADDE38E4119816AB}Prefabs/InitialPlayer_Version2.et")
					continue;
				
				vector playerCharacterOrigin = playerCharacter.GetOrigin();

				// Get Distance
				dis = vector.Distance(localOrigin, playerCharacterOrigin);
				
				if (m_HUDManager.GetIsLocalPlayerInVehicle())
					searchRadius = 3.5;
				else
					searchRadius = 24;
				
				if (dis > searchRadius)
				{
					UpdatePlayerRadarWidget(i, -1, 1, 0, 0, 0, 0);
					continue;
				};

				if (playerID != SCR_PlayerController.GetLocalPlayerId())
				{
					// Get Direction
					float dir = vector.Direction(playerCharacterOrigin, localOrigin).ToYaw();
		
					// Get Relative Direction
					float relDir = Math.Mod(((dir - localYaw) + 360), 360);
					relDir = Math.Mod(relDir - (dir * 2), 360);
					relDir = relDir * Math.DEG2RAD;
					
					disT = (dis * (132 / (searchRadius * 2.725)));
					x = (Math.Sin(relDir) * disT);
					y = (Math.Cos(relDir) * disT);
				};

				opacity = Math.Map(dis, (0.8*searchRadius), searchRadius, 0.6, 0);
				rotation = -Math.Mod((CSI_ChararcterHelper.GetCharacterYaw(playerCharacter) - localYaw), 360);

				UpdatePlayerRadarWidget(i, playerID, ICON_WIDTH_AND_HEIGHT, opacity, x, y, rotation);
			};
		} else
			groupCount = 0;

		for (int e = groupCount; e <= 24; e++)
			UpdatePlayerRadarWidget(e, -1, 1, 0, 0, 0, 0);
	}

}