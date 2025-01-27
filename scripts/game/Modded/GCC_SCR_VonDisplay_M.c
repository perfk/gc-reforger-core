modded class SCR_VonDisplay
{
	override void OnReceive(int playerId, BaseTransceiver receiver, int frequency, float quality)
	{
		if(!frequency)
			return;
		
		super.OnReceive(playerId, receiver, frequency, quality);
	}
}