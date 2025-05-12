modded class SCR_PlayerController : PlayerController
{
	void TILW_ShowJIPInfo()
	{
		Rpc(TILW_RpcDo_ShowJIPInfo);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void TILW_RpcDo_ShowJIPInfo()
	{
		if (GetGame().GetInputManager().IsUsingMouseAndKeyboard()) TILW_ShowHint("You can join-in-progress by holding U, choosing a slot and clicking 'Ready' in the bottom right.\nOnce ingame, double-press U for JIP TP.", "JIP Available", 30);
		else TILW_ShowHint("You can join-in-progress by holding RB + Left on D-Pad, choosing a slot and selecting 'Ready' in the bottom right.", "JIP Available", 40);
	
		PS_GameModeCoop gm = PS_GameModeCoop.Cast(GetGame().GetGameMode());
		if(gm)
			gm.Action_OpenLobby();
	}
	
	void GCC_ArtyFire(string faction, int index, int roundCount, int gunCount, int roundDispersion, vector position)
	{
		Rpc(GCC_RpcAsk_ArtyFire, faction, index, roundCount, gunCount, roundDispersion, position);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void GCC_RpcAsk_ArtyFire(string faction, int index, int roundCount, int gunCount, int roundDispersion, vector position)
	{
		GCC_Arty manager = GCC_Arty.GetInstance();
		if(!manager)
			return;
		
		manager.CallMission(faction, index, roundCount, gunCount, roundDispersion, position);
	}
}