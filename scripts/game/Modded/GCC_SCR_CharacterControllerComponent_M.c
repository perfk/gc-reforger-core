modded class SCR_CharacterControllerComponent : CharacterControllerComponent
{
	override void ActionUnequipItem(float value = 0.0, EActionTrigger trigger = 0)
	{
		BaseWeaponManagerComponent weaponManager = GetWeaponManagerComponent();
		if(!weaponManager)
			return;
		
		if (weaponManager.GetCurrentWeapon())
			TryEquipRightHandItem(null, EEquipItemType.EEquipTypeUnarmedContextual, true);
	}
}