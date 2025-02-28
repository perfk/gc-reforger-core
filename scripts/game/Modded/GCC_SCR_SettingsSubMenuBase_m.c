modded class SCR_SettingsSubMenuBase: SCR_SubMenuBase {
	
	override protected void OnMenuItemChanged(SCR_SettingsBindingBase binding) {
		ApplyLowGraphicsSettings();
		super.OnMenuItemChanged(binding);
	}
		
	void ApplyLowGraphicsSettings() {
		auto settings = GetGame().GetEngineUserSettings();
		if (!settings)
			return;
		
		auto videoSettings = settings.GetModule("VideoUserSettings");
		auto grassSettings = settings.GetModule("GrassMaterialSettings");
		
		int distantShadows;
		if (videoSettings && videoSettings.Get("DistantShadowsQuality", distantShadows) && distantShadows < 1) {
			videoSettings.Set("DistantShadowsQuality", 1);
			GetGame().UserSettingsChanged();
		}
		
		int grassLOD;
		if (grassSettings && grassSettings.Get("Lod", grassLOD) && grassLOD < 2) {
			grassSettings.Set("Lod", 2);
			GetGame().UserSettingsChanged();
		}
	}
}

modded class SCR_BaseGameMode: BaseGameMode {
	override protected void OnGameModeStart() {
		ApplyLowGraphicsSettings();
		super.OnGameModeStart();
	}
	
	void ApplyLowGraphicsSettings() {
		auto settings = GetGame().GetEngineUserSettings();
		if (!settings)
			return;
		
		auto videoSettings = settings.GetModule("VideoUserSettings");
		auto grassSettings = settings.GetModule("GrassMaterialSettings");
		
		int distantShadows;
		if (videoSettings && videoSettings.Get("DistantShadowsQuality", distantShadows) && distantShadows < 1) {
			videoSettings.Set("DistantShadowsQuality", 1);
			GetGame().UserSettingsChanged();
		}
		
		int grassLOD;
		if (grassSettings && grassSettings.Get("Lod", grassLOD) && grassLOD < 2) {
			grassSettings.Set("Lod", 2);
			GetGame().UserSettingsChanged();
		}
	}
}