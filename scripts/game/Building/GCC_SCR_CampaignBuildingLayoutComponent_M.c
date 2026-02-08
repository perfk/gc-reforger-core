modded class SCR_CampaignBuildingLayoutComponent
{
	override int GetBuildingValue(int prefabID)
	{
		PS_GameModeCoop gm = PS_GameModeCoop.Cast(GetGame().GetGameMode());
		if (!gm)
			return super.GetBuildingValue(prefabID);
		return gm.GetActiveTrenchMultiplier() *  super.GetBuildingValue(prefabID); 
	}
}