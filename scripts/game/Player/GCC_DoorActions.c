class GCC_CrackDoorAction : ScriptedUserAction
{
	override void  PerformContinuousAction(IEntity pOwnerEntity, IEntity pUserEntity, float timeSlice)
	{
		DoorComponent doorComponent = DoorComponent.Cast(pOwnerEntity.FindComponent(DoorComponent));
		if (!doorComponent)
			return;

		float control = doorComponent.GetControlValue();
		control = control + 0.02;
		if(control >= 0.95)
			control = 1;
		
		doorComponent.SetActionInstigator(pUserEntity);
		doorComponent.SetControlValue(control);
	}
	
	bool CanBeAction()
	{
		DoorComponent doorComponent = DoorComponent.Cast(GetOwner().FindComponent(DoorComponent));
		if (doorComponent)
		{
			if(doorComponent.GetControlValue() < 1)
				return true;
		}
		
		return false;
	}
	
	override bool CanBePerformedScript(IEntity user)
	{
		return CanBeAction();
	}
	
	override bool CanBeShownScript(IEntity user)
	{
		return CanBeAction();
	}
}

class GCC_EaseDoorAction : ScriptedUserAction
{
	override void PerformContinuousAction(IEntity pOwnerEntity, IEntity pUserEntity, float timeSlice)
	{
		DoorComponent doorComponent = DoorComponent.Cast(pOwnerEntity.FindComponent(DoorComponent));
		if (!doorComponent)
			return;

		float control = doorComponent.GetControlValue();
		control = control - 0.02;
		if(control <= 0.05)
			control = 0;
		
		doorComponent.SetActionInstigator(pUserEntity);
		doorComponent.SetControlValue(control);
	}
	
	bool CanBeAction()
	{
		DoorComponent doorComponent = DoorComponent.Cast(GetOwner().FindComponent(DoorComponent));
		if (doorComponent)
		{
			if(doorComponent.GetControlValue() > 0)
				return true;
		}
		
		return false;
	}
	
	override bool CanBePerformedScript(IEntity user)
	{
		return CanBeAction();
	}
	
	override bool CanBeShownScript(IEntity user)
	{
		return CanBeAction();
	}
}