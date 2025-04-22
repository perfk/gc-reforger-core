modded enum ChimeraMenuPreset
{
	GCC_ArtyMenu,
}

class GCC_ArtyMenu : ChimeraMenuBase
{
	
	ref array<ref GCC_ArtyData> artyData;
	
	override void OnMenuInit()
	{
		super.OnMenuInit();
		
		PlayerController pc = GetGame().GetPlayerController();
		if (!pc)
			return;

		IEntity ce = GetGame().GetPlayerController().GetControlledEntity();
		if (!ce)
			return;

		SCR_ChimeraCharacter player = SCR_ChimeraCharacter.Cast(ce);
		if (!player)
			return;

		if (!player.m_pFactionComponent)
			return;

		Faction faction = player.m_pFactionComponent.GetAffiliatedFaction();
		if (!faction)
			return;
		
		SCR_ComboBoxComponent combo = SCR_ComboBoxComponent.GetComboBoxComponent("ShellType", GetRootWidget());
		combo.ClearAll();
		combo.m_OnChanged.Insert(UpdateRound);
		
		GCC_Arty manager = GCC_Arty.GetInstance();		
		artyData = manager.GetRoundsFaction(faction.GetFactionKey());
		if(artyData.IsEmpty())
			return;
		
		foreach(GCC_ArtyData round : artyData)
		{
			combo.AddItem(round.m_roundName);
		}
		
		combo.SetCurrentItem(0);
		UpdateRound(null, 0);
	}

	void UpdateRound(SCR_ComboBoxComponent widget, int index)
	{
		GCC_ArtyData round = artyData[index];
		Widget root = GetRootWidget();
		
		//RoundCount
		TextWidget roundCount = TextWidget.Cast(root.FindAnyWidget("RoundCount"));
		roundCount.SetText(round.m_roundCount.ToString());
		
		//GunCount
		TextWidget gunCount = TextWidget.Cast(root.FindAnyWidget("GunCount"));
		gunCount.SetText(round.m_gunCount.ToString());
		
		//GunSelected
		EditBoxWidget gunSelected = EditBoxWidget.Cast(root.FindAnyWidget("GunSelected"));
		gunSelected.SetText(round.m_gunCount.ToString());
	}
	
	override void OnMenuClose()
	{
		super.OnMenuClose();
		
		SCR_ComboBoxComponent combo = SCR_ComboBoxComponent.GetComboBoxComponent("ShellType", GetRootWidget());
		combo.m_OnChanged.Remove(UpdateRound);
	}
}

class GCC_CloseMenuOnClick : ScriptedWidgetComponent
{
	override bool OnClick(Widget w, int x, int y, int button)
	{
		MenuManager menuManager = GetGame().GetMenuManager();
		GCC_ArtyMenu menu = GCC_ArtyMenu.Cast(menuManager.FindMenuByPreset(ChimeraMenuPreset.GCC_ArtyMenu));
		if(!menu)
			return true;
		
		return menuManager.CloseMenu(menu);
	}
}

class GCC_FireButton : ScriptedWidgetComponent
{
	override bool OnClick(Widget w, int x, int y, int button)
	{
		SCR_PlayerController pc = SCR_PlayerController.Cast(GetGame().GetPlayerController());
		if (!pc)
			return true;

		IEntity ce = GetGame().GetPlayerController().GetControlledEntity();
		if (!ce)
			return true;

		SCR_ChimeraCharacter player = SCR_ChimeraCharacter.Cast(ce);
		if (!player)
			return true;

		if (!player.m_pFactionComponent)
			return true;

		Faction faction = player.m_pFactionComponent.GetAffiliatedFaction();
		if (!faction)
			return true;
		
		MenuManager menuManager = GetGame().GetMenuManager();
		GCC_ArtyMenu menu = GCC_ArtyMenu.Cast(menuManager.FindMenuByPreset(ChimeraMenuPreset.GCC_ArtyMenu));
		if(!menu)
			return true;

		Widget root = menu.GetRootWidget();

		SCR_ComboBoxComponent combo = SCR_ComboBoxComponent.GetComboBoxComponent("ShellType", root);
		EditBoxWidget gridCords = EditBoxWidget.Cast(root.FindAnyWidget("GridCords"));

		EditBoxWidget roundsSelected = EditBoxWidget.Cast(root.FindAnyWidget("RoundSelected"));
		string roundsText = roundsSelected.GetText();
		if(roundsText.IsEmpty())
			return true;
		int rounds = roundsText.ToInt();

		EditBoxWidget gunsSelected = EditBoxWidget.Cast(root.FindAnyWidget("GunSelected"));
		string gunsText = gunsSelected.GetText();
		if(gunsText.IsEmpty())
			return true;
		int guns = gunsText.ToInt();
	
		EditBoxWidget dispSelected = EditBoxWidget.Cast(root.FindAnyWidget("Dispersion"));
		string dispersionText = dispSelected.GetText();
		int dispersion = 0;
		if(!dispersionText.IsEmpty())
			dispersion = dispersionText.ToInt();
		
		string text = gridCords.GetText();
		if(text.IsEmpty())
			return true;
		
		vector position = GridToWorldVector(text);
		if(position == vector.Zero)
			return true;
		
		pc.GCC_ArtyFire(faction.GetFactionKey(), combo.GetCurrentIndex(), rounds, guns, dispersion, position);
		
		return true;
	}
	
	vector GridToWorldVector(string gridRef)
	{
	    int len = gridRef.Length();

	    if (len % 2 != 0)
	    {
	        Print("Invalid grid format: " + gridRef);
	        return vector.Zero;
	    }
	    
	    int digitsPerCoordinate = len / 2;
	    float multiplier = Math.Pow(10, 5 - digitsPerCoordinate);
	
	    // Convert each substring to a float
	    float xVal = gridRef.Substring(0, digitsPerCoordinate).ToFloat();
	    float yVal = gridRef.Substring(digitsPerCoordinate, digitsPerCoordinate).ToFloat();
	    
	    float x = xVal * multiplier;
	    float y = yVal * multiplier;
		
	    float z = GetGame().GetWorld().GetSurfaceY(x, y);
		
	    return Vector(x, z, y);
	}
}
