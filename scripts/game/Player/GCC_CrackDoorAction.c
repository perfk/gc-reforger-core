class GCC_CrackDoorAction : ScriptedUserAction
{
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		DoorComponent doorComponent = DoorComponent.Cast(pOwnerEntity.FindComponent(DoorComponent));
		if (!doorComponent)
			return;
		
		float control = doorComponent.GetControlValue() + 0.1;

		doorComponent.SetActionInstigator(pUserEntity);
		doorComponent.SetControlValue(control);
	}
	
	override bool CanBePerformedScript(IEntity user)
	{
		DoorComponent doorComponent = DoorComponent.Cast(GetOwner().FindComponent(DoorComponent));
		if (doorComponent)
		{
			if(doorComponent.GetControlValue() < 1)
				return true;
		}

		return false;
	}
}