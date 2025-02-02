modded class PS_PlayableManager
{
	[Attribute("1")]
	protected bool m_hidePlayersOnSpawn;
	
	override void RegisterPlayable(PS_PlayableComponent playableComponent)
	{
		super.RegisterPlayable(playableComponent);
		
		if(!m_hidePlayersOnSpawn)
			return;
		
		if (Replication.IsServer())
		{
			IEntity entity = playableComponent.GetOwner();
			Physics physics = entity.GetPhysics();
			if (physics)
			{
				physics.EnableGravity(false);
				entity.SetOrigin(entity.GetOrigin() + vector.Up * 100000);
			}
		}
	}
	
	override void ApplyPlayable(int playerId)
	{
		PlayerManager playerManager = GetGame().GetPlayerManager();
		SCR_PlayerController playerController = SCR_PlayerController.Cast(playerManager.GetPlayerController(playerId));
		if (!playerController)
			return;
		PS_PlayableControllerComponent playableController = PS_PlayableControllerComponent.Cast(playerController.FindComponent(PS_PlayableControllerComponent));
		SCR_GroupsManagerComponent groupsManagerComponent = SCR_GroupsManagerComponent.GetInstance();
		
		SetPlayerState(playerId, PS_EPlayableControllerState.Playing);
		
		RplId playableId = GetPlayableByPlayer(playerId);	
		if (playableId != RplId.Invalid())
		{
			IEntity character = GetPlayableById(playableId).GetOwner();
			if (character)
			{
				SCR_DamageManagerComponent damageManager = SCR_DamageManagerComponent.GetDamageManager(character);
				if (damageManager)
				{
					if (damageManager.GetState() == EDamageState.DESTROYED)
					{
						GetGame().GetCallqueue().CallLater(WrapSwitch, 1000, false, playerId);
					}
				}
			}
		}
		
		IEntity entity;
		if (playableId == RplId.Invalid() || playableId == -1) {
			// Remove group
			SCR_AIGroup currentGroup = groupsManagerComponent.GetPlayerGroup(playableId);
			if (currentGroup) currentGroup.RemovePlayer(playerId);
			SetPlayerFactionKey(playerId, "");
			
			entity = playableController.GetInitialEntity();
			if (!entity)
			{
				Resource resource = Resource.Load("{ADDE38E4119816AB}Prefabs/InitialPlayer_Version2.et");
				EntitySpawnParams params = new EntitySpawnParams();
				entity = GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld(), params);
				playableController.SetInitialEntity(entity);
			}
			
			SetSpawnTransform(playerController);
			GetGame().GetCallqueue().CallLater(playerController.SetInitialMainEntity, 1, false, entity);
			return;
		} else entity = GetPlayableById(playableId).GetOwner();		 
		
		
		// Delete VoN
		IEntity vonEntity = playableController.GetInitialEntity();
		if (vonEntity) SCR_EntityHelper.DeleteEntityAndChildren(vonEntity);
	
		SetSpawnTransform(playerController);
		GetGame().GetCallqueue().CallLater(playerController.SetInitialMainEntity, 1, false, entity);

		// Set new player faction
		SCR_ChimeraCharacter playableCharacter = SCR_ChimeraCharacter.Cast(entity);
		SCR_Faction faction = SCR_Faction.Cast(playableCharacter.GetFaction());
		SetPlayerFactionKey(playerId, faction.GetFactionKey());
		
		GetGame().GetCallqueue().CallLater(ChangeGroup, 0, false, playerId, playableId);
	}
	
	void SetSpawnTransform(SCR_PlayerController playerController)
	{
		IEntity player = playerController.GetControlledEntity();
		PS_PlayableComponent playableComp = PS_PlayableComponent.Cast(player.FindComponent(PS_PlayableComponent));
		if(m_hidePlayersOnSpawn && playableComp && !playableComp.firstTimeSpawned)
		{
			Physics physics = player.GetPhysics();
			physics.SetVelocity("0 0 0");
			physics.SetAngularVelocity("0 0 0");
			physics.EnableGravity(true);
			
			vector transform[4];
			playableComp.GetSpawnTransform(transform);
			player.SetOrigin(transform[3]);
			player.Update();
			playableComp.firstTimeSpawned = true;
		}
		
		GetGame().GetCallqueue().CallLater(CheckPlayerTeleported, 200, false, playerController);
	}
	
	void CheckPlayerTeleported(SCR_PlayerController playerController)
	{
		IEntity player = playerController.GetControlledEntity();
		vector position = player.GetOrigin();
		if(player && position[1] > 90000)
		{
			PS_PlayableComponent playableComp = PS_PlayableComponent.Cast(player.FindComponent(PS_PlayableComponent));
			vector transform[4];
			playableComp.GetSpawnTransform(transform);
			playerController.Teleport(transform[3]);
			GetGame().GetCallqueue().CallLater(CheckPlayerTeleported, 200, false, playerController);
		}
	}
}