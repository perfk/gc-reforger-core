modded class PS_PlayableControllerComponent
{
	protected bool m_isSpectating = false;
	
	static PS_PlayableControllerComponent GetPlayableController()
	{
		return PS_PlayableControllerComponent.Cast(GetGame().GetPlayerController().FindComponent(PS_PlayableControllerComponent));
	}
	
	bool IsSpectating()
	{
		return m_isSpectating;
	}
	
	override void SwitchToObserver(IEntity from)
	{
		super.SwitchToObserver(from);
		m_isSpectating = true;
	}
	
	override void SwitchFromObserver()
	{
		super.SwitchFromObserver();
		m_isSpectating = false;
	}
	
	override void UpdatePosition(bool force)
	{
		super.UpdatePosition(force);
		
		if (m_InitialEntity)
		{
			if(m_isSpectating)
			{
				m_InitialEntity.SetOrigin(m_Camera.GetOrigin());
			}
			else
			{
				m_InitialEntity.SetOrigin(m_vVoNPosition);
			}
		}
	}
	
	override void SetVoNPosition(vector VoNPosition)
	{
		m_vVoNPosition = VoNPosition;
		m_InitialEntity.SetOrigin(VoNPosition);
	}
	/*
	Hide players on spawn
	override private void OnControlledEntityChanged(IEntity from, IEntity to)
	{
		super.OnControlledEntityChanged(from, to);

		if (!Replication.IsServer())
			return;
		
		PS_PlayableManager playableManager = PS_PlayableManager.GetInstance();
		if(!playableManager.m_hidePlayersOnSpawn)
			return;
		
		PS_PlayableComponent playableComp = PS_PlayableComponent.Cast(to.FindComponent(PS_PlayableComponent));
		if(!playableComp || playableComp.m_firstTimeSpawned)
			return;
		
		vector transform[4];
		playableComp.GetSpawnTransform(transform);
		Rpc(RPC_SetSpawnPosition, transform[3]);
		playableComp.m_firstTimeSpawned = true;
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RPC_SetSpawnPosition(vector position)
	{
		IEntity player = GetGame().GetPlayerController().GetControlledEntity();
		Physics physics = player.GetPhysics();
		physics.SetVelocity("0 0 0");
		physics.SetAngularVelocity("0 0 0");
		player.SetOrigin(position);
		physics.EnableGravity(true);
	}
	*/
}
/*
modded class PS_VoNRoomsManager
{
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	override void RPC_MoveToRoom(int playerId, int roomId, vector position)
	{
		super.RPC_MoveToRoom(playerId, roomId, position);
		
		PlayerController playerController = GetGame().GetPlayerManager().GetPlayerController(playerId);
		PS_PlayableControllerComponent playableController = PS_PlayableControllerComponent.Cast(playerController.FindComponent(PS_PlayableControllerComponent));
		playableController.SetVoNPosition(position);
	}
}
*/