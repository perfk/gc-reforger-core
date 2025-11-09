modded class SCR_VonDisplay
{
	override void OnReceive(int playerId, bool isSenderEditor, BaseTransceiver receiver, int frequency, float quality)
	{
		if(!frequency)
			return;
		
		super.OnReceive(playerId, isSenderEditor, receiver, frequency, quality);
	}
}
