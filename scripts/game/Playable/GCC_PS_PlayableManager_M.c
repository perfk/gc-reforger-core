// Delete after next stable RL update
modded class PS_PlayableManager : ScriptComponent
{
	override void RegisterPlayable(PS_PlayableComponent playableComponent)
	{
		//PrintFormat("RegisterPlayable - %1", playableComponent.GetOwner());
		
		RplId playableId = playableComponent.GetId();
		if (m_aPlayables.Contains(playableId))
			return;
		m_aPlayables[playableId] = playableComponent;
		
		GetGame().GetCallqueue().Call(OnPlayableRegisteredLateInvoke, playableId, playableComponent);
		GetGame().GetCallqueue().Remove(UpdatePlayablesSortedWrap);
		GetGame().GetCallqueue().Remove(UpdatePlayablesSorted);
		GetGame().GetCallqueue().Call(UpdatePlayablesSortedWrap);
		
		if (Replication.IsServer())
		{
			SCR_ChimeraCharacter playableCharacter = SCR_ChimeraCharacter.Cast(playableComponent.GetOwner());
			AIControlComponent aiControl = AIControlComponent.Cast(playableCharacter.FindComponent(AIControlComponent));
			SCR_AIGroup playableGroup =  SCR_AIGroup.Cast(aiControl.GetControlAIAgent().GetParentGroup());
			SCR_AIGroup playerGroup;
			
			if (!playableGroup)
				return;
			
			if (!playableGroup.IsSlave())
			{
				SCR_GroupsManagerComponent groupsManagerComponent = SCR_GroupsManagerComponent.GetInstance();
				playerGroup = groupsManagerComponent.CreateNewPlayableGroup(playableGroup.GetFaction());
				playerGroup.SetSlave(playableGroup);
				playerGroup.SetMaxMembers(playableGroup.m_aUnitPrefabSlots.Count());
				playerGroup.SetCustomName(playableGroup.GetCustomName(), -1);
								
				playableGroup.SetCanDeleteIfNoPlayer(false);
				playerGroup.SetCanDeleteIfNoPlayer(false);
			} else {
				playerGroup = playableGroup.GetMaster();
			}
			SetPlayablePlayerGroupId(playableId, playerGroup.GetGroupID());
			GetGame().GetCallqueue().CallLater(UpdateGroupCallsigne, 0, false, playableId, playerGroup, playableGroup);
		}
	}
}