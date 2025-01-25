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
	}
}