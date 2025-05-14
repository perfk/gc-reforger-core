modded class PS_SpectatorMenu
{
	protected bool isHDRActive = false;
	protected int hdr = 5;
	
	protected void AdjustHDR(float value)
	{
		if(value == 0)
			return;
		
		if(!isHDRActive)
			return;
		
		int input = Math.ClampInt(hdr + value, 1, 50);
		if(hdr == input)
			return;
		
		hdr = input;
		
		BaseWorld world = GetGame().GetWorld();
		world.SetCameraHDRBrightness(world.GetCurrentCameraId(), input);
	}
	
	protected void ToggleHDR()
	{
		BaseWorld world = GetGame().GetWorld();
		int cameraID = world.GetCurrentCameraId();

		if(isHDRActive)
		{
			world.SetCameraHDRBrightness(cameraID, -1);
		}
		else
		{
			world.SetCameraHDRBrightness(cameraID, hdr);
		}
		isHDRActive = !isHDRActive;
	}
	
	override void OnMenuInit()
	{
		super.OnMenuInit();
		
		GetGame().GetInputManager().AddActionListener("GCC_AdjustHDR", EActionTrigger.DOWN, ToggleHDR);
		GetGame().GetInputManager().AddActionListener("GCC_AdjustHDRWheel", EActionTrigger.VALUE, AdjustHDR);
	}
	
	override void OnMenuClose()
	{
		super.OnMenuClose();
		
		GetGame().GetInputManager().RemoveActionListener("GCC_AdjustHDR", EActionTrigger.DOWN, ToggleHDR);
		GetGame().GetInputManager().RemoveActionListener("GCC_AdjustHDRWheel", EActionTrigger.VALUE, AdjustHDR);	
		
		BaseWorld world = GetGame().GetWorld();
		world.SetCameraHDRBrightness(world.GetCurrentCameraId(), -1);
		isHDRActive = false;
	}
}